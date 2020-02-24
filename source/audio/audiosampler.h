
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

#ifndef AUDIOSAMPLER_H
#define AUDIOSAMPLER_H

#include <QScopedArrayPointer>
#include <QScopedPointer>
#include <QtCore/QIODevice>
#include <QtCore/QScopedPointer>
#include <QtMultimedia/QAudioBuffer>
#include <QtMultimedia/QAudioDeviceInfo>
#include <QtMultimedia/QAudioFormat>
#include <QtMultimedia/QAudioInput>

/**
 * @brief A class that handles audio input devices.
 *
 * It reads from a device and emits data to a demodulator and the spectogram
 */
class AudioSampler : public QIODevice {
private:
    Q_OBJECT
    QAudioFormat format_priv;               //!< The variable storing the QAudioFormat used for the device
    QAudioDeviceInfo device_priv;           //!< The variable storing the QAudioDeviceInfo for the device being used
    QScopedPointer<QAudioInput> input_priv; //!< QAudioInput pointer to the currently opened port
    bool started_priv;                      //!< True if the device has been started
    QAudio::State state_priv;               //!< Current state of the device

public:
    explicit AudioSampler(QObject *parent = 0);

    Q_INVOKABLE bool start(int deviceIndex);
    bool isStarted() const;
    Q_INVOKABLE void stop();
    quint32 samplingFrequency() const;
    quint32 samplesToWait() const;

protected:
    qint64 readData(char *data, qint64 maxlen);
    qint64 writeData(const char *data, qint64 len);

signals:
    /**
     * @brief Signal that is emitted to the thread that demodulates at 1250 bps.
     *
     * The ownership of \p samples is passed to the one (1!!!) receiving slot.
     *
     * @param samples Pointer to the array that contains the samples
     * @param numberOfSamples Number of samples in \p samples.
     */
    void audioSamplesReadyFor1250(std::int16_t *samples, int numberOfSamples);

    void audioSamples(std::int16_t *samples, int sampleCount);

private slots:
    void audioInputStateChanged(QAudio::State state);
};

#endif // AUDIOSAMPLER_H
