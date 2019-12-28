#include "uploadworker.h"

UploadWorker::UploadWorker(const QUrl &serverAPIUrl,
    QString uploadQueueDir,
    QString uploadFileName,
    QString rejectedFileName,
    QThread *thread,
    QObject *parent)
: QObject(parent) {
    Q_ASSERT(!serverAPIUrl.isEmpty());
    Q_ASSERT(thread != nullptr);
    this->uploadDirString = uploadQueueDir;
    moveToThread(thread);
    uploadingTimeoutTimer_priv.moveToThread(thread);
    loginTimeoutTimer_priv.moveToThread(thread);
    upload_mgr_priv.moveToThread(thread);
    login_mgr_priv.moveToThread(thread);
    serverAPIUrl_priv = serverAPIUrl;
    username_priv = QString("UNSET");
    password_priv = QString("UNSET");
    auth_token = QString("UNSET");
    setRandomPrefix(); // Different instances will log into different files
    this->fileName = uploadDirString + this->prefix + "_" + uploadFileName;
    this->rejectedFileName = uploadDirString + rejectedFileName;
    this->fileNameEnding = "_" + uploadFileName;
    isCurrentlyUploading_priv = false;
    isLoggedIn_priv = false;
    isCurrentlyLoggingIn_priv = false;
    remaining_priv = 0;
    errorsInARow_priv = 0;
    pjw_priv.reset(new PacketsJSONWrapper());
    QTimer::singleShot(0, [&] { loadUnsentPackets(); });
    uploadingTimeoutTimer_priv.setSingleShot(true);
    loginTimeoutTimer_priv.setSingleShot(true);
    QObject::connect(&upload_mgr_priv, &QNetworkAccessManager::finished, [&](QNetworkReply *reply) {
        QScopedPointer<QNetworkReply, QScopedPointerDeleteLater> reply_QSP(reply);
        stopUploadTimeoutTimer();
        if (reply_QSP->error() != QNetworkReply::NoError) {
            if (reply_QSP->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() == 401) {
                this->setIsLoggedIn(false);
                this->login();
            }
            else if (reply_QSP->error() == QNetworkReply::RemoteHostClosedError) {
                setErrorsInARow(errorsInARow_priv + 1);
                emit serverDown();
            }
            else {
                setErrorsInARow(errorsInARow_priv + 1);
                emit error(reply_QSP->errorString());
            }
        }
        else {
            setErrorsInARow(0);
            QByteArray received = reply_QSP->readAll();
            processUploadReply(received);
        }
        // We don't want to send packets too quickly because it would overload the processor
        QTimer::singleShot(200, [&](){setIsCurrentlyUploading(false);});

    });
    QObject::connect(&login_mgr_priv, &QNetworkAccessManager::finished, [&](QNetworkReply *reply) {
        QScopedPointer<QNetworkReply, QScopedPointerDeleteLater> reply_QSP(reply);
        stopLoginTimeoutTimer();
        if (reply_QSP->error() != QNetworkReply::NoError) {
            setIsCurrentlyLoggingIn(false);
            this->setIsLoggedIn(false);
            if (reply_QSP->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() == 401) {
                emit invalidLoginCredentials();
            }
            else if (reply_QSP->error() == QNetworkReply::RemoteHostClosedError) {
                setErrorsInARow(errorsInARow_priv + 1);
                emit serverDown();
            }
            else {
                setErrorsInARow(errorsInARow_priv + 1);
                emit error(reply_QSP->errorString());
            }
        }
        else {
            QByteArray received = reply_QSP->readAll();
            auto result = processLoginReply(received);
            if (!result) {
                setIsCurrentlyLoggingIn(false);
                this->setIsLoggedIn(false);
                setErrorsInARow(errorsInARow_priv + 1);
            }
            else {
                QTimer::singleShot(200, [&](){
                    setIsCurrentlyLoggingIn(false);
                    setIsLoggedIn(true);
                });
            }
        }
    });
    QObject::connect(&uploadingTimeoutTimer_priv, &QTimer::timeout, [&]() {
        setErrorsInARow(errorsInARow_priv + 1);
        upload_reply->abort();
        emit timedOut();
        // NO need to call deleteLater since finished will trigger upon abort!!!
    });
    QObject::connect(&loginTimeoutTimer_priv, &QTimer::timeout, [&]() {
        setErrorsInARow(errorsInARow_priv + 1);
        login_reply->abort();
        emit timedOut();
        // NO need to call deleteLater since finished will trigger upon abort!!!
    });
}

void UploadWorker::upload() {
    if (!isCurrentlyUploading_priv && isLoggedIn_priv && pjw_priv->getPacketCount() > 0) {
        setIsCurrentlyUploading(true);
        QUrl uploadUrl = this->serverAPIUrl_priv.url(QUrl::UrlFormattingOption::None) + "packets/bulk";
        QNetworkRequest request(uploadUrl);
        request.setHeader(QNetworkRequest::KnownHeaders::ContentTypeHeader, "application/json");
        QString authorization_header = QString("Bearer ") + this->auth_token;
        request.setRawHeader(QByteArray("Authorization"), authorization_header.toLocal8Bit());
        this->upload_reply = upload_mgr_priv.post(request, pjw_priv->writeToJSON(MAXPACKETSTOSEND));
        uploadingTimeoutTimer_priv.start(7000);
    }
}

void UploadWorker::login() {
    if (this->isCurrentlyLoggingIn_priv) {
        return;
    }
    setIsCurrentlyLoggingIn(true);
    QUrl loginUrl = this->serverAPIUrl_priv.url(QUrl::UrlFormattingOption::None) + "tokens";
    QNetworkRequest request(loginUrl);
    request.setHeader(QNetworkRequest::KnownHeaders::ContentTypeHeader, "application/json");
    QString concatenated = this->username_priv + ":" + this->password_priv;
    QByteArray data = concatenated.toLocal8Bit().toBase64();
    QString headerData = "Basic " + data;
    request.setRawHeader("Authorization", headerData.toLocal8Bit());
    this->login_reply = login_mgr_priv.post(request, QByteArray());
    loginTimeoutTimer_priv.start(7000);
}

void UploadWorker::newPacket(QString packet, SatelliteChanger::Satellites satellite) {
    pjw_priv->addPacket(packet, satellite);
    setRemaining(pjw_priv->getPacketCount());
    auto saved = pjw_priv->writeContentsToFile(this->fileName);
    if (!saved) {
        qWarning() << "UploadWorker could not open the file for writing";
    }
}

void UploadWorker::loginSlot(QString username, QString password) {
    this->username_priv = username;
    this->password_priv = password;
    this->login();
}

void UploadWorker::stopUploadTimeoutTimer() {
    uploadingTimeoutTimer_priv.stop();
}

void UploadWorker::stopLoginTimeoutTimer() {
    loginTimeoutTimer_priv.stop();
}

void UploadWorker::stopTimers() {
    uploadingTimeoutTimer_priv.stop();
    loginTimeoutTimer_priv.stop();
}

void UploadWorker::logout()
{
    this->setIsLoggedIn(false);
    this->stopTimers();
}

void UploadWorker::setRandomPrefix() {
    const QString possibleCharacters("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789");
    const int randomStringLength = 6;

    auto randomGenerator = QRandomGenerator::securelySeeded();

    QString randomString;
    for (int i = 0; i < randomStringLength; ++i) {
        int index = randomGenerator.bounded(possibleCharacters.length());
        QChar nextChar = possibleCharacters.at(index);
        randomString.append(nextChar);
    }
    this->prefix = randomString;
}

void UploadWorker::loadUnsentPackets() {
    auto directory = QDir(uploadDirString);
    QStringList fileNameFilters = {"*" + this->fileNameEnding};
    auto fileNames = directory.entryList(fileNameFilters, QDir::Files | QDir::Readable | QDir::Writable);
    for (auto fileName : fileNames) {
        QString fn = uploadDirString + fileName;
        auto loaded = pjw_priv->appendContentsFromFile(fn);
        auto saved = pjw_priv->writeContentsToFile(this->fileName);
        if (loaded && saved) {
            QFile(fn).remove();
        }
    }
    setRemaining(pjw_priv->getPacketCount());
}

void UploadWorker::setIsCurrentlyUploading(bool newValue) {
    if (newValue != isCurrentlyUploading_priv) {
        isCurrentlyUploading_priv = newValue;
        emit isCurrentlyUploadingChanged(isCurrentlyUploading_priv);
    }
}

void UploadWorker::setIsCurrentlyLoggingIn(bool newValue) {
    if (newValue != isCurrentlyLoggingIn_priv) {
        isCurrentlyLoggingIn_priv = newValue;
        emit isCurrentlyLoggingInChanged(isCurrentlyLoggingIn_priv);
    }
}

void UploadWorker::setIsLoggedIn(bool newValue) {
    if (newValue != isLoggedIn_priv) {
        isLoggedIn_priv = newValue;
        emit isLoggedInChanged(isLoggedIn_priv);
    }
}

void UploadWorker::setRemaining(int newValue) {
    if (newValue != remaining_priv) {
        remaining_priv = newValue;
        emit remainingChanged(remaining_priv);
    }
}

void UploadWorker::setErrorsInARow(int newValue) {
    if (newValue != errorsInARow_priv) {
        errorsInARow_priv = newValue;
        emit errorsInARowChanged(errorsInARow_priv);
    }
}

void UploadWorker::processUploadReply(QByteArray reply) {
    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(reply, &parseError);
    if (parseError.error == QJsonParseError::NoError) {
        if (doc.isObject()) {
            QJsonObject obj = doc.object();
            if (obj.contains("results")) {
                QList<bool> results;
                for (const auto ob : obj["results"].toArray()) {
                    auto result = ob.toObject();
                    // It is important to note that we are always indexing with 0
                    // Because we are always either saving/requeueing the first one
                    // Or removing it (when successfully uploaded)
                    if (result.contains("message")) {
                        emit error(obj["message"].toString());
                        auto saved = pjw_priv->writePacketToFile(this->rejectedFileName, 0);
                        if (!saved) {
                            pjw_priv->requeuePacket(0);
                        }
                    }
                    pjw_priv->removePacket(0);
                }
                pjw_priv->writeContentsToFile(fileName);
                setRemaining(pjw_priv->getPacketCount());
            }
            else {
                qWarning() << obj << " doesn't contain the results";
                emit error("Parsing error during reply processing");
            }
        }
        else {
            qWarning() << "Can not be parsed to a QJsonObject";
            emit error("Parsing error during reply processing");
        }
    }
    else {
        emit error("Parsing error during reply processing");
    }
}

bool UploadWorker::processLoginReply(QByteArray reply) {
    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(reply, &parseError);
    if (parseError.error == QJsonParseError::NoError) {
        if (doc.isObject()) {
            QJsonObject obj = doc.object();
            if (obj.contains("token")) {
                this->auth_token = obj["token"].toString();
                return true;
            }
            else {
                qWarning() << obj << " doesn't contain the token";
                emit error("Parsing error during reply processing");
                return false;
            }
        }
        else {
            qWarning() << "Can not be parsed to a QJsonObject";
            emit error("Parsing error during reply processing");
            return false;
        }
    }
    else {
        emit error("Parsing error during reply processing");
        return false;
    }
}
