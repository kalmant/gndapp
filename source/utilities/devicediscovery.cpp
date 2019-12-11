#include "devicediscovery.h"

#include <QAudioDeviceInfo>
#include <QDebug>
#include <QSerialPortInfo>
#include <QSocketNotifier>
#include <cassert>

#if defined(Q_OS_LINUX)
#include <libudev.h>
#include <unistd.h>
#endif

#if defined(Q_OS_LINUX)
struct udev *udevInst = nullptr;
udev_monitor *udevMonitor = nullptr;
int udevMonitorFd = 0;
#endif

/**
 * @brief Constructor for the class.
 *
 * Sets up certain parameters based on operating system.
 *
 * @param[in] parent The parent QObject, should be left empty.
 */
DeviceDiscovery::DeviceDiscovery(QObject *parent) : QObject(parent) {
#if defined(Q_OS_LINUX)
    if (udevInst == nullptr) {
        udevInst = udev_new();

        if (udevInst == nullptr) {
            qInfo() << "Can't initialize udev.";
        }
        else {
            // Create udev monitor
            udevMonitor = udev_monitor_new_from_netlink(udevInst, "udev");

            // Set up udev filters
            udev_monitor_filter_add_match_subsystem_devtype(udevMonitor, "usb", NULL);

            // Enable udev monitor
            udev_monitor_enable_receiving(udevMonitor);
            udevMonitorFd = udev_monitor_get_fd(udevMonitor);
        }
    }
#elif defined(Q_OS_WIN)
// EventFilter handles auto-detection of changes in devices on Windows systems.
#endif

    QObject::connect(&timer, &QTimer::timeout, this, &DeviceDiscovery::updateAudioDevices);
    QObject::connect(&timer, &QTimer::timeout, this, &DeviceDiscovery::updateSerialPorts);

    updateAudioDevices();
    updateSerialPorts();
}

#if defined(Q_OS_LINUX)
/**
 * @brief Handles updating devices on Linux after there has been a change in them.
 *
 * Whenever a change has been made to devices, this slot receives a signal.
 * The slot filters based on action and starts a timer that will update the list of devices available.
 *
 * @param[in] fd The socket identifier passed by QSocketNotifier::activated(). It identifies the file descriptor.
 */
void DeviceDiscovery::notifierActivated(int fd) {
    assert(fd == udevMonitorFd);
    struct udev_device *dev = udev_monitor_receive_device(udevMonitor);
    if (dev) {
        QString action = QString::fromUtf8(udev_device_get_action(dev));
        if (action == "add" || action == "remove") {
            timer.setSingleShot(true);
            timer.start(600);
        }

        udev_device_unref(dev);
    }
}
#endif

#if defined(Q_OS_LINUX)
/**
 * @brief Initializes the QSocketNotifier.
 */
void DeviceDiscovery::initializeNotifier() {
    if (udevInst != nullptr) {
        notifier.reset(new QSocketNotifier(udevMonitorFd, QSocketNotifier::Read, this));
        QObject::connect(notifier.data(), &QSocketNotifier::activated, this, &DeviceDiscovery::notifierActivated);
        notifier->setEnabled(true);
    }
}
#elif defined(Q_OS_WIN)
#else
/**
 * @brief Starts device polling for auto device detection on OS X.
 */
void DeviceDiscovery::startTimer() {
    timer.start(3000);
}
#endif

/**
 * @brief Returns the QStringListModel pointer for the current audio devices.
 * @return Returns a pointer to \p audioDevicesModel.
 */
QStringListModel *DeviceDiscovery::audioDevices() {
    return &audioDevicesModel;
}

/**
 * @brief Returns the number of audio devices.
 * @return Returns the row count of \p audioDevicesModel.
 */
int DeviceDiscovery::audioDeviceCount() {
    return audioDevicesModel.rowCount();
}

/**
 * @brief This slot updates the currently available audio input devices.
 *
 * Updates \p audioDevicesModel as well.
 */
void DeviceDiscovery::updateAudioDevices() {
    QStringList ret;
    qInfo() << "Updating audio devices";
    for (auto temp : QAudioDeviceInfo::availableDevices(QAudio::AudioInput)) {
        ret.append(temp.deviceName());
    }
    audioDevicesModel.setStringList(ret);
    emit this->audioDeviceCountChanged();
}

/**
 * @brief Returns the QStringListModel pointer for the current serial ports.
 * @return Returns a pointer to \p serialPortsModel.
 */
QStringListModel *DeviceDiscovery::serialPorts() {
    return &serialPortsModel;
}

/**
 * @brief Returns the number of serial ports.
 * @return Returns the row count of \p serialPortsModel.
 */
int DeviceDiscovery::serialPortCount() {
    return serialPortsModel.rowCount();
}

/**
 * @brief This slot updates the currently available serial ports.
 *
 * Updates \p serialPortsModel as well.
 */
void DeviceDiscovery::updateSerialPorts() {
    qInfo() << "Updating serial ports";
    QStringList ret;
    for (auto temp : QSerialPortInfo::availablePorts()) {
        ret.append(temp.portName());
    }
    serialPortsModel.setStringList(ret);
    emit this->serialPortCountChanged();
}
