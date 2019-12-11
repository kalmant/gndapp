#ifndef EVENTFILTER_H
#define EVENTFILTER_H
#include "devicediscovery.h"
#include <QAbstractNativeEventFilter>
#include <QTimer>

/**
 * @brief A class that filters windows events and triggers a device discovery when necessary.
 *
 * Events(VM_DEVICECHANGE) that suggest a device change of some sort start a timer.
 * That timer emits signals to a DeviceDiscovery object. That object in turn updates the USB device and audio input
 * device lists.
 */
class EventFilter : public QAbstractNativeEventFilter {
public:
    EventFilter(DeviceDiscovery *deviceDisc);
    bool nativeEventFilter(const QByteArray &eventType, void *message, long *result);
    QTimer
        timer; //!< Timer, that is used to trigger a device discovery when there has been a message that implies change.
};

#endif // EVENTFILTER_H
