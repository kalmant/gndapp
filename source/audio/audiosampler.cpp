
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
// This file is part of the frequency-analyzer application.
// It is licensed to you under the terms of the MIT license.
// http://opensource.org/licenses/MIT
//
// Copyright (c) 2014 Timur Kristóf

#include "audiosampler.h"
#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>
#include <QtCore/QtEndian>

/**
 * @brief Constructor for the class.
 *
 * @param parent
 */
AudioSampler::AudioSampler(QObject *parent) : QIODevice(parent) {
    started_priv = false;
}

/**
 * @brief Starts the audio device at \p deviceIndex index.
 *
 * Sets the necessary variables and opens the device.
 *
 * @param deviceIndex The index of the device in the list of available devices.
 * @return Returns true if it was successfully started, false otherwise.
 */
bool AudioSampler::start(int deviceIndex) {
    if (started_priv)
        return true;

    // Setting up the audio format
    format_priv.setSampleRate(samplingFrequency());
    format_priv.setChannelCount(1);
    format_priv.setSampleSize(16);
    format_priv.setSampleType(QAudioFormat::SignedInt);
    format_priv.setByteOrder(QAudioFormat::LittleEndian);
    format_priv.setCodec("audio/pcm");

    // Selecting the audio device based on deviceIndex
    if (deviceIndex >= QAudioDeviceInfo::availableDevices(QAudio::AudioInput).length()) {
        // No such device exists
        return false;
    }

    device_priv = QAudioDeviceInfo::availableDevices(QAudio::AudioInput).at(deviceIndex);

    if (device_priv.isNull()) {
        qDebug() << "Default audio device is null, maybe there isn't one on this computer?";
        return false;
    }

    // The specified format should be widely supported and available on the user's device
    if (!device_priv.isFormatSupported(format_priv)) {
        qWarning() << "QAudioInput: requested format not supported";
        return false;
    }

    state_priv = QAudio::IdleState;
    input_priv.reset(new QAudioInput(device_priv, format_priv, this));
    QObject::connect(input_priv.data(), &QAudioInput::stateChanged, this, &AudioSampler::audioInputStateChanged);
    this->open(QIODevice::WriteOnly);
    input_priv->start(this);

    started_priv = true;
    return true;
}

/**
 * @brief Returns true if the device has been started, false otherwise.
 * @return Returns the value of \p started_priv .
 */
bool AudioSampler::isStarted() const {
    return started_priv;
}

/**
 * @brief Stops the device and clears the samples.
 */
void AudioSampler::stop() {
    if (!started_priv)
        return;

    input_priv->stop();
    this->close();

    started_priv = false;
}

/**
 * @brief Returns the sampling frequency.
 * @return Returns 44100.
 */
quint32 AudioSampler::samplingFrequency() const {
    return 44100;
}

/**
 * @brief UNUSED function that has to implemented when subclassing QIODevice.
 * @param data -
 * @param maxlen -
 * @return -
 */
qint64 AudioSampler::readData(char *data, qint64 maxlen) {
    // NOTE: you are not supposed to read from this object
    Q_UNUSED(data)
    Q_UNUSED(maxlen)

    return 0;
}

/**
 * @brief Function that is called whenever some data was read from the device.
 *
 * It emits to the audio demodulating threads and updates \p in.
 * @param data Data received from the device.
 * @param len The length of \p data. There are \p len / 2 number of samples, since they are 16 bit.
 * @return
 */
qint64 AudioSampler::writeData(const char *data, qint64 len) {
    if (!started_priv)
        return len;
    if (state_priv != QAudio::ActiveState)
        return len;

    // Assuming little endian, 1-channel, 16-bit signed integer samples
    Q_ASSERT(len % 2 == 0);
    const std::int16_t *samples = reinterpret_cast<const std::int16_t *>(data);

    // Since data is read as int16_t and received as char with length
    // Only half as many samples are available

    // Emitting to the demodulating thread and letting them know that len/2 samples are available
    QScopedArrayPointer<std::int16_t> dataFor1250(new std::int16_t[len / 2]);
    std::copy(samples, samples + len / 2, dataFor1250.data());
    emit this->audioSamplesReadyFor1250(dataFor1250.take(), len / 2);

    // Emitting to the spectogram
    QScopedArrayPointer<std::int16_t> audioSamples(new std::int16_t[len / 2]);
    std::copy(samples, samples + len / 2, audioSamples.data());
    emit this->audioSamples(audioSamples.take(), len / 2);

    return len;
}

/**
 * @brief Called whenever \p state_priv has changed.
 * @param state The new value for \p state_priv .
 */
void AudioSampler::audioInputStateChanged(QAudio::State state) {

    state_priv = state;
}
