#include "uploadcontroller.h"

UploadController::UploadController(
    const QUrl &uploadUrl, QString uploadQueueDir, QString uploadFileName, QString rejectedFileName, QObject *parent)
: QObject(parent) {
    Q_ASSERT(!uploadUrl.isEmpty());
    isLoggedIn_priv = false;
    isCurrentlyUploading_priv = false;
    isCurrentlyLoggingIn_priv = false;
    remaining_priv = 0;
    errorsInARow_priv = 0;
    UW_priv.reset(new UploadWorker(uploadUrl, uploadQueueDir, uploadFileName, rejectedFileName, &workerThread_priv));
    connect(this, &UploadController::newPacketForWorker, UW_priv.data(), &UploadWorker::newPacket);
    connect(this, &UploadController::loginInWorker, UW_priv.data(), &UploadWorker::loginSlot);
    connect(this, &UploadController::uploadInWorker, UW_priv.data(), &UploadWorker::upload);
    connect(UW_priv.data(),
        &UploadWorker::isCurrentlyUploadingChanged,
        this,
        &UploadController::isCurrentlyUploadingChangedInWorker);
    connect(UW_priv.data(), &UploadWorker::isLoggedInChanged, this, &UploadController::isLoggedInChangedInWorker);
    connect(UW_priv.data(),
        &UploadWorker::isCurrentlyLoggingInChanged,
        this,
        &UploadController::isCurrentlyLoggingInInWorker);
    connect(UW_priv.data(), &UploadWorker::remainingChanged, this, &UploadController::remainingChangedInWorker);
    connect(UW_priv.data(), &UploadWorker::errorsInARowChanged, this, &UploadController::errorsInARowChangedInWorker);
    connect(UW_priv.data(), &UploadWorker::error, this, &UploadController::errorInWorker);
    connect(UW_priv.data(),
        &UploadWorker::invalidLoginCredentials,
        this,
        &UploadController::invalidLoginCredentialsInWorker);
    connect(UW_priv.data(), &UploadWorker::serverDown, this, &UploadController::serverDownInWorker);
    connect(UW_priv.data(), &UploadWorker::timedOut, this, &UploadController::timedOutInWorker);
    connect(this, &UploadController::stopTimersInWorker, UW_priv.data(), &UploadWorker::stopTimers);
    connect(this, &UploadController::logoutInWorker, UW_priv.data(), &UploadWorker::logout);

    workerThread_priv.start();
}

UploadController::~UploadController() {
    emit stopTimersInWorker();
    workerThread_priv.wait(10); // waiting for a short while until the timer stops
    // The above line doesn't really work in this case, if there is an ongoing login for example
    // The slot processes the signal more than a second after it was emitted in the above case.
    // For this reason, timers are stopped through main.qml by calling logout
    workerThread_priv.quit();
    workerThread_priv.wait();
}

void UploadController::upload() {
    if (!isCurrentlyUploading_priv && isLoggedIn_priv) {
        emit uploadInWorker();
    }
}

void UploadController::login(QString username, QString password) {
    Q_ASSERT(username != nullptr);
    Q_ASSERT(password != nullptr);
    if (username.length() > 0 && password.length() > 0) {
        emit loginInWorker(username, password);
    }
}

void UploadController::logout() {
    emit logoutInWorker();
}

void UploadController::newPacket(QString packetQString, SatelliteChanger::Satellites satellite) {
    Q_ASSERT(packetQString != nullptr);
    emit newPacketForWorker(packetQString, satellite);
}

void UploadController::errorInWorker(QString errorString) {
    emit uploadError(errorString);
}

void UploadController::isCurrentlyUploadingChangedInWorker(bool newValue) {
    if (newValue != isCurrentlyUploading_priv) {
        isCurrentlyUploading_priv = newValue;
        emit isCurrentlyUploadingChanged();
    }
}

void UploadController::isCurrentlyLoggingInInWorker(bool newValue) {
    if (newValue != isCurrentlyLoggingIn_priv) {
        isCurrentlyLoggingIn_priv = newValue;
        emit isCurrentlyLoggingInChanged();
    }
}

void UploadController::isLoggedInChangedInWorker(bool newValue) {
    if (newValue != isLoggedIn_priv) {
        isLoggedIn_priv = newValue;
        emit isLoggedInChanged();
    }
}

void UploadController::remainingChangedInWorker(int newValue) {
    if (newValue != remaining_priv) {
        remaining_priv = newValue;
        emit remainingChanged();
    }
}

void UploadController::errorsInARowChangedInWorker(int newValue) {
    if (newValue != errorsInARow_priv) {
        errorsInARow_priv = newValue;
        emit errorsInARowChanged();
    }
}

void UploadController::invalidLoginCredentialsInWorker() {
    emit invalidLoginCredentials();
}

void UploadController::serverDownInWorker() {
    emit serverDown();
}

void UploadController::timedOutInWorker() {
    emit timedOut();
}
