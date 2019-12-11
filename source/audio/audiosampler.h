
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

#include "fftw3.h"
#include <QScopedArrayPointer>
#include <QScopedPointer>
#include <QtCore/QIODevice>
#include <QtCore/QScopedPointer>
#include <QtCore/QTimer>
#include <QtMultimedia/QAudioBuffer>
#include <QtMultimedia/QAudioDeviceInfo>
#include <QtMultimedia/QAudioFormat>
#include <QtMultimedia/QAudioInput>
#include <vector>

/**
 * @brief A class that handles audio input devices.
 *
 * It reads from a device, has the capability of FFTW-ing and emitting to WaterfallItem.
 * It is also capable of emitting the data read from the device to demodulators.
 */
class AudioSampler : public QIODevice {
private:
    Q_OBJECT
    QAudioFormat format_priv;               //!< The variable storing the QAudioFormat used for the device
    QAudioDeviceInfo device_priv;           //!< The variable storing the QAudioDeviceInfo for the device being used
    QScopedPointer<QAudioInput> input_priv; //!< QAudioInput pointer to the currently opened port
    bool started_priv;                      //!< True if the device has been started
    QAudio::State state_priv;               //!< Current state of the device
    quint32 samplesToWait_priv;             //!< Number of samples that should be bundled together for FFTW execution

    // Necessary for FFTW
    double *in;        //!< Pointer to the double array that stores the values read from the audio input device
    fftw_complex *out; //!< Pointer to the fftw_complex array that stores the result of the fftw execution
    fftw_plan my_plan; //!< The fftw_plan that is executed whenever FFTW is used
    unsigned int currentSampleCount; //!< Variable that keeps track of how many samples have been collected from the
                                     //!< audio input device

    // Amplitudes after FFTW
    QScopedArrayPointer<double>
        AmplitudeArray; //!< Pointer to the array that contains the amplitudes for the frequency segments

    bool packetsAreBeingRead_priv = false; //!< True if packages are being demodulated
    bool waterfallRunning_priv = false;    //!< True if the waterfall diagram is running

public:
    explicit AudioSampler(QObject *parent = 0);
    ~AudioSampler();

    bool start(int deviceIndex);
    bool isStarted() const;
    void stop();
    quint32 samplingFrequency() const;
    quint32 samplesToWait() const;

protected:
    qint64 readData(char *data, qint64 maxlen);
    qint64 writeData(const char *data, qint64 len);

signals:
    /**
     * @brief Signal that is emitted whenever a new batch of data has been processed with FFTW
     *
     * \p amplitudeArray contains the amplitudes and should be deleted by the function that receives the signal.
     *
     * @param amplitudeArray The pointer to the array of amplitudes for the frequency segments
     * @param size The number of frequency segments in \p ampliteArray
     */
    void samplesCollected(double *amplitudeArray, unsigned int size);

    /**
     * @brief Signal that is emitted to the thread that demodulates at 1250 bps.
     *
     * The ownership of \p samples is passed to the one (1!!!) receiving slot.
     *
     * @param samples Pointer to the array that contains the samples
     * @param numberOfSamples Number of samples in \p samples.
     */
    void audioSamplesReadyFor1250(std::int16_t *samples, int numberOfSamples);

private slots:
    void elapsed();
    void audioInputStateChanged(QAudio::State state);
public slots:
    void samplesToWaitChanged(int value);
    void packetsAreBeingReadChangedSlot(bool value);
    void waterfallRunningChangedSlot(bool value);
};

#endif // AUDIOSAMPLER_H
