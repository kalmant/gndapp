#ifndef MESSAGEPROXY_H
#define MESSAGEPROXY_H

#include <QObject>

//used to catch certain messages
/**
 * @brief Forwards certain messages to QML.
 *
 * Handles forwarding audio device failure messages to QML.
 */
class MessageProxy : public QObject
{
    Q_OBJECT
public:
    explicit MessageProxy(QObject *parent = 0);

    void audioDeviceDisc();
    void audioDeviceFailedToOpen();
signals:
    /**
     * @brief Signal that is emitted when an audio device has become unavailable.
     */
    void audioDeviceDisconnected();

    /**
     * @brief Signal that is emitted when the audio device has failed to open.
     */
    void audioDeviceFailed();
public slots:
};

#endif // MESSAGEPROXY_H
