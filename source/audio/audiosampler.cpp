
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
#include <QtCore/QMetaMethod>
#include <QtCore/QtEndian>
#include <cmath>

/**
 * @brief Constructor for the class.
 *
 * Initializes every necessary variable.
 * The default \p samplesToWait_priv is 4096, thus \p AmplitudeArray are initialized for 4096.
 *
 * @param parent
 */
AudioSampler::AudioSampler(QObject *parent) : QIODevice(parent) {

    // Initialising for a sample count of 4096
    started_priv = false;
    samplesToWait_priv = 4096;

    // Using FFTW according to its documentation
    currentSampleCount = 0;
    in = (double *) fftw_malloc(sizeof(double) * samplesToWait_priv);
    out = (fftw_complex *) fftw_malloc(sizeof(fftw_complex) * (samplesToWait_priv / 2 + 1));
    my_plan = fftw_plan_dft_r2c_1d(samplesToWait_priv, in, out, FFTW_ESTIMATE);

    // By default we use a sample count of 4096 and 44.1kHz sampling rate, so 300-3kHz translates to 252
    AmplitudeArray.reset(new double[252]);
}

/**
 * @brief Destructor for the class.
 */
AudioSampler::~AudioSampler() {
    fftw_destroy_plan(my_plan);
    fftw_free(in);
    fftw_free(out);
    fftw_cleanup();
}

/**
 * @brief The function is called whenever enough samples have been collected for FFTW.
 *
 * Executes \p my_plan , calculates the amplitudes and emits AudioSampler::samplesCollected().
 */
void AudioSampler::elapsed() {
    if (this->isSignalConnected(QMetaMethod::fromSignal(&AudioSampler::samplesCollected))) {

        // Executing FFTW and producing values to out
        fftw_execute(my_plan);

        int startIndex = 0;
        int lastIndex = 0;
        int len = 0;

        // Setting indices for the 4 preset sample counts
        switch (samplesToWait_priv) {
        case 1024:
            startIndex = 7;
            lastIndex = 69;
            len = 63;
            break;
        case 2048:
            startIndex = 14;
            lastIndex = 139;
            len = 126;
            break;
        case 4096:
            startIndex = 28;
            lastIndex = 279;
            len = 252;
            break;
        case 8192:
            startIndex = 56;
            lastIndex = 559;
            len = 504;
            break;
        default:
            qCritical() << "For some reason the sample count was set to an undefined value. Shouldn't happen!!!";
        }

        // Calculating amplitudes based on out, which contains the values calculated by FFTW
        for (int i = startIndex; i <= lastIndex; i++) {
            double rN = out[i][0] / samplesToWait_priv;
            double iN = out[i][1] / samplesToWait_priv;
            AmplitudeArray[i - startIndex] = 2 * sqrt(rN * rN + iN * iN);
        }

        // Emitting the calculated values
        emit this->samplesCollected(AmplitudeArray.data(), len);
    }

    // resetting the samplecount, so that we once again read from the input
    currentSampleCount = 0;
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

    // Stopping the device and clearing the samples
    input_priv->stop();
    currentSampleCount = 0;
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
 * @brief Returns the number of samples that should be bundled together before executing FFTW.
 * @return Returns the value of \p samplesToWait_priv .
 */
quint32 AudioSampler::samplesToWait() const {
    return samplesToWait_priv;
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

    // When packets are being read...
    // Emitting to the thread and letting them know that len/2 samples are available
    if (packetsAreBeingRead_priv) {
        QScopedArrayPointer<std::int16_t> dataFor1250(new std::int16_t[len / 2]);
        std::copy(samples, samples + len / 2, dataFor1250.data());
        emit this->audioSamplesReadyFor1250(dataFor1250.take(), len / 2);
    }

    // When waterfall is running...
    if (waterfallRunning_priv) {
        for (qint64 i = 0; i < len / 2; i++) {
            in[currentSampleCount] = ((double) samples[i]);
            currentSampleCount++;

            if (currentSampleCount >= samplesToWait_priv) {
                elapsed();
            }
        }
    }
    return len;
}

/**
 * @brief Called whenever \p state_priv has changed.
 * @param state The new value for \p state_priv .
 */
void AudioSampler::audioInputStateChanged(QAudio::State state) {

    state_priv = state;
}

/**
 * @brief Called whenever \p samplesToWait_priv has changed.
 *
 * It modifies certain variables based on the change.
 *
 * @param value The new value for \p samplesToWait_priv .
 */
void AudioSampler::samplesToWaitChanged(int value) {

    // Stopping the device, resetting the necessary variables and initialising some others
    this->stop();
    samplesToWait_priv = value;
    fftw_destroy_plan(my_plan);
    fftw_free(in);
    fftw_free(out);
    fftw_cleanup();
    in = (double *) fftw_malloc(sizeof(double) * samplesToWait_priv);
    out = (fftw_complex *) fftw_malloc(sizeof(fftw_complex) * (samplesToWait_priv / 2 + 1));
    my_plan = fftw_plan_dft_r2c_1d(samplesToWait_priv, in, out, FFTW_ESTIMATE);
    switch (samplesToWait_priv) {
    case 1024:
        AmplitudeArray.reset(new double[63]);
        break;
    case 2048:
        AmplitudeArray.reset(new double[126]);
        break;
    case 4096:
        AmplitudeArray.reset(new double[252]);
        break;
    case 8192:
        AmplitudeArray.reset(new double[504]);
        break;
    default:
        qInfo() << "Only the 4 preset values for sample count should be used!!";
    }
}

/**
 * @brief Called whenever \p packetsAreBeingRead_priv has changed.
 * @param value The new value for \p packetsAreBeingRead_priv .
 */
void AudioSampler::packetsAreBeingReadChangedSlot(bool value) {
    packetsAreBeingRead_priv = value;
}

/**
 * @brief Called whenever \p waterfallRunning_priv has changed.
 * @param value The new value for \p waterfallRunning_priv .
 */
void AudioSampler::waterfallRunningChangedSlot(bool value) {
    waterfallRunning_priv = value;
}
