#include "messageproxy.h"

/**
 * @brief Empty constructor for the class.
 * @param[in] parent Parent QObject, should be left empty.
 */
MessageProxy::MessageProxy(QObject *parent) : QObject(parent)
{

}

/**
 * @brief Called when an audio device has disconnected during an operation.
 *
 * Emits MessageProxy::audioDeviceDisconnected().
 */
void MessageProxy::audioDeviceDisc()
{
    emit audioDeviceDisconnected();
}

/**
 * @brief Called when an audio device has failed to open.
 *
 * Emits MessageProxy::audioDeviceFailed().
 */
void MessageProxy::audioDeviceFailedToOpen()
{
    emit audioDeviceFailed();
}
