#ifndef UPLOADCONTROLLER_H
#define UPLOADCONTROLLER_H

#include "../utilities/satellitechanger.h"
#include "uploadworker.h"
#include <QObject>
#include <QScopedPointer>
#include <QString>
#include <QThread>
#include <QUrl>

#define PROPERTY(type, name)                             \
private:                                                 \
    Q_PROPERTY(type name READ name NOTIFY name##Changed) \
private:                                                 \
    type name##_priv;                                    \
                                                         \
public:                                                  \
    inline const type &name() const {                    \
        return name##_priv;                              \
    }                                                    \
                                                         \
private:

/**
 * @brief The class that is used to upload the packets to the server.
 */
class UploadController : public QObject {
    Q_OBJECT
    PROPERTY(bool, isLoggedIn)
    PROPERTY(bool, isCurrentlyUploading)
    PROPERTY(bool, isCurrentlyLoggingIn)
    PROPERTY(int, remaining)
    PROPERTY(int, errorsInARow)
private:
    QThread workerThread_priv;
    QScopedPointer<UploadWorker> UW_priv;

public:
    UploadController() = delete;
    explicit UploadController(const QUrl &serverAPIUrl,
        QString uploadQueueDir,
        QString uploadFileName,
        QString rejectedFileName,
        QObject *parent = nullptr);
    ~UploadController();

    Q_INVOKABLE void upload();
    Q_INVOKABLE void login(QString username, QString password);
    Q_INVOKABLE void logout();

signals:
    void isLoggedInChanged();
    void isCurrentlyUploadingChanged();
    void isCurrentlyLoggingInChanged();
    void remainingChanged();
    void errorsInARowChanged();
    void uploadError(QString errorString);
    void newPacketForWorker(QString packet, SatelliteChanger::Satellites satellite);
    void loginInWorker(QString username, QString password);
    void uploadInWorker();
    void stopTimersInWorker();
    void newSatelliteInWorker(SatelliteChanger::Satellites satellite);
    void invalidLoginCredentials();
    void serverDown();
    void timedOut();
    void logoutInWorker();

public slots:
    void newPacket(QString packetQString, SatelliteChanger::Satellites satellite);
    void errorInWorker(QString errorString);
    void isCurrentlyUploadingChangedInWorker(bool newValue);
    void isLoggedInChangedInWorker(bool newValue);
    void isCurrentlyLoggingInInWorker(bool newValue);
    void remainingChangedInWorker(int newValue);
    void errorsInARowChangedInWorker(int newValue);
    void invalidLoginCredentialsInWorker();
    void serverDownInWorker();
    void timedOutInWorker();
};

#endif // UPLOADCONTROLLER_H
