#include <QDebug>
#include "eventfilter.h"

#if defined(Q_OS_WIN)
#include <windows.h>
#include <dbt.h>
#endif


/**
 * @brief A filter that reads OS events on Windows and handles automatic device discovery.
 * @param[in] deviceDisc The DeviceDiscovery object that handles the audio input device and USB device discovery.
 */
EventFilter::EventFilter(DeviceDiscovery *deviceDisc): QAbstractNativeEventFilter()
{
#if defined(Q_OS_WIN)
    QObject::connect(&timer, &QTimer::timeout, deviceDisc, &DeviceDiscovery::updateAudioDevices);
    QObject::connect(&timer, &QTimer::timeout, deviceDisc, &DeviceDiscovery::updateSerialPorts);
#else
    (void)deviceDisc;
#endif
}

/**
 * @brief A filter that checks the \p message and starts \p timer if a VM_DEVICECHANGE was received.
 *
 * \p result and \p eventType are unused but are part of the interface.
 *
 * @param[in] eventType Check Qt documentation for QAbstractNativeEventFilter class.
 * @param[in] message Check Qt documentation for QAbstractNativeEventFilter class.
 * @param[in] result Check Qt documentation for QAbstractNativeEventFilter class.
 * @return Returns false, since every event is let through.
 */
bool EventFilter::nativeEventFilter(const QByteArray &eventType, void *message, long *result)
{
(void)result;
(void)eventType;
#if defined(Q_OS_WIN)
    MSG* ev = static_cast<MSG *>(message);
 //   WM_DEVICECHANGE  -USB/AUDIO
//    DBT_DEVICEARRIVAL - USB ON
//    DBT_DEVICEREMOVECOMPLETE - USB OFF
    if (ev->message == WM_DEVICECHANGE){
        timer.setSingleShot(true);
        timer.start(600);
    }
#else
    (void)message;
#endif
    return false;
}

