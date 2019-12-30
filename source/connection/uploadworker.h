#ifndef UPLOADWORKER_H
#define UPLOADWORKER_H

#include "../utilities/satellitechanger.h"
#include "packetsjsonwrapper.h"
#include <QDir>
#include <QFile>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QObject>
#include <QRandomGenerator>
#include <QScopedPointer>
#include <QThread>
#include <QTimer>
#include <QUrl>

#define MAXPACKETSTOSEND 20

class UploadWorker : public QObject {
    Q_OBJECT
private:
    QUrl serverAPIUrl_priv;
    QString uploadDirString;
    QString fileName;
    QString fileNameEnding;
    QString rejectedFileName;
    QString username_priv;
    QString password_priv;
    QString prefix;
    QString auth_token;
    bool isCurrentlyUploading_priv;
    bool isLoggedIn_priv;
    bool isCurrentlyLoggingIn_priv;
    int remaining_priv;
    int errorsInARow_priv;
    QNetworkAccessManager upload_mgr_priv;
    QTimer uploadingTimeoutTimer_priv;
    QNetworkReply *upload_reply;
    bool login_aborted;
    QNetworkAccessManager login_mgr_priv;
    QTimer loginTimeoutTimer_priv;
    QNetworkReply *login_reply;
    QScopedPointer<PacketsJSONWrapper> pjw_priv;

    void setRandomPrefix();
    void loadUnsentPackets();
    void setIsCurrentlyUploading(bool newValue);
    void setIsCurrentlyLoggingIn(bool newValue);
    void setIsLoggedIn(bool newValue);
    void setRemaining(int newValue);
    void setErrorsInARow(int newValue);
    bool processLoginReply(QByteArray reply);
    void processUploadReply(QByteArray reply);
    void login();

public:
    explicit UploadWorker(const QUrl &uploadUrl,
        QString uploadQueueDir,
        QString uploadFileName,
        QString rejectedFileName,
        QThread *thread,
        QObject *parent = nullptr);

signals:
    void isCurrentlyUploadingChanged(bool newValue);
    void isLoggedInChanged(bool newValue);
    void remainingChanged(int newValue);
    void errorsInARowChanged(int newValue);
    void error(QString errorString);
    void invalidLoginCredentials();
    void isCurrentlyLoggingInChanged(bool newValue);
    void serverDown();
    void timedOut();

public slots:
    void upload();
    void newPacket(QString packet, SatelliteChanger::Satellites satellite);
    void loginSlot(QString username, QString password);
    void stopUploadTimeoutTimer();
    void stopLoginTimeoutTimer();
    void stopTimers();
    void logout();
};

#endif // UPLOADWORKER_H
