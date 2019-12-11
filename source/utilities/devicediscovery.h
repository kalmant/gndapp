#ifndef DEVICEDISCOVERY_H
#define DEVICEDISCOVERY_H

#include <QObject>
#include <QScopedPointer>
#include <QSocketNotifier>
#include <QStringList>
#include <QStringListModel>
#include <QTimer>

/**
 * @brief Discovers audio input devices and serial ports.
 *
 * Automatically handles changes in device list on Linux and MAC OSX.
 * Windows requires EventFilter class.
 */
class DeviceDiscovery : public QObject {
    Q_OBJECT
    Q_PROPERTY(QStringListModel *audioDevices READ audioDevices NOTIFY audioDevicesChanged)
    Q_PROPERTY(int audioDeviceCount READ audioDeviceCount NOTIFY audioDeviceCountChanged)
    Q_PROPERTY(QStringListModel *serialPorts READ serialPorts NOTIFY serialPortsChanged)
    Q_PROPERTY(int serialPortCount READ serialPortCount NOTIFY serialPortCountChanged)
    QTimer timer;                       //!< Timer that will trigger update events.
    QStringListModel audioDevicesModel; //!< List of the names of the currently available audio input devices.
    QStringListModel serialPortsModel;  //!< List of the names of the currently available serial ports.
#if defined(Q_OS_LINUX)
    QScopedPointer<QSocketNotifier> notifier;
#endif
public:
    explicit DeviceDiscovery(QObject *parent = 0);
    QStringListModel *audioDevices();
    int audioDeviceCount();
    QStringListModel *serialPorts();
    int serialPortCount();

public slots:
    void updateAudioDevices();
    void updateSerialPorts();
#if defined(Q_OS_LINUX)
    void notifierActivated(int fd);
#endif
#if defined(Q_OS_LINUX)
    void initializeNotifier();
#elif defined(Q_OS_WIN)
#else
    void startTimer();
#endif

signals:
    /**
     * @brief This signal is emitted when the list of audio input devices has changed.
     */
    void audioDevicesChanged();

    /**
     * @brief This signal is emitted when the number of audio input devices has changed.
     */
    void audioDeviceCountChanged();

    /**
     * @brief This signal is emitted when the list of serial ports has changed.
     */
    void serialPortsChanged();

    /**
     * @brief This signal is emitted when the number of serial ports has changed.
     */
    void serialPortCountChanged();
};

#endif // DEVICEDISCOVERY_H
