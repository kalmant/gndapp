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

#ifndef WATERFALLITEM_H
#define WATERFALLITEM_H

#include "audiosampler.h"
#include "fftw3.h"
#include <QImage>
#include <QQuickItem>
#include <QSGClipNode>
#include <QSGNode>
#include <QSGSimpleTextureNode>
#include <QSGTexture>
#include <QScopedArrayPointer>
#include <QScopedPointer>
#include <QTimer>
#include <vector>

/**
 * @brief The class that is capable of drawing and displaying a waterfall diagram.
 */
class WaterfallItem : public QQuickItem {
    Q_OBJECT
    Q_PROPERTY(bool isStarted READ isStarted NOTIFY isStartedChanged)
    Q_PROPERTY(bool isWaterfallActive READ isWaterfallActive NOTIFY isWaterfallActiveChanged)
    Q_PROPERTY(float sensitivity READ sensitivity WRITE setSensitivity)
    Q_PROPERTY(int samplesToWait READ samplesToWait WRITE setSamplesToWait NOTIFY samplesToWaitChanged)
    Q_PROPERTY(float minValue READ minValue NOTIFY minValueChanged)
    Q_PROPERTY(float maxValue READ maxValue NOTIFY maxValueChanged)
    Q_PROPERTY(bool adaptiveColoring READ adaptiveColoring)

    constexpr static int newRowHeight = 4; //!< The height of each row in pixels
    QScopedPointer<QImage> image_priv;     //!< Pointer to the first QImage object
    QSGSimpleTextureNode *imageNode_priv;  //!< Pointer to the first QSGSimpleTextureNode object
    QScopedPointer<QSGTexture, QScopedPointerDeleteLater> imageNodeTexture_priv; //!< First QSGTexture object
    int imageLine_priv; //!< An integer that keeps track of which segment of the \p image_priv has has been drawn last
    QScopedPointer<QImage> image2_priv;    //!< Pointer to the second QImage object
    QSGSimpleTextureNode *imageNode2_priv; //!< Pointer to the second QSGSimpleTextureNode object
    QScopedPointer<QSGTexture, QScopedPointerDeleteLater> imageNodeTexture2_priv; //!< Second QSGTexture object
    bool imageNodeDestroy2_priv; //!< Indicated whether \p imageNodeTexture2_priv has to be reinitialized
    QSGClipNode *clipNode_priv;  //!< Pointer to the SQGClipNode object

    QScopedArrayPointer<QRgb> colors_priv; //!< QRgb array that is used to determine the color for pixels
    int colorsCount_priv;                  //!< The number of distinct colors that can be used to color the diagram
    AudioSampler sampler_priv;             //!< The AudioSampler object that handles the audio input device and FFTW
    QTimer timer_priv; //!< Timer connected to WaterfallItem::maxValueChanged() and WaterfallItem::minValueChanged

    float minValue_priv = 0.0f; //!< The current minimum value in the row, used for adaptive coloring
    float maxValue_priv = 0.0f; //!< The current maximum value in the row, used for adaptive coloring
    int samplesToWait_priv; //!< The number of samples that was used when FFT-ing the data coming from the audio input
                            //!< device
    float sensitivity_priv; //!< Linear scaling factor for non-adaptive coloring
    bool adaptiveColoring_priv = true; //!< True if adaptive coloring is enabled, false otherwise
    static constexpr int amplitudeCount_priv =
        30; //!< Adaptive coloring determines colors based on this many high and low values
    QScopedArrayPointer<float> minAmplitudes_priv; //!< Stores the min. amplitudes for \p amplitudeCount_priv rows
    int minAmplitudesIndex_priv;                   //!< The index for the minimum amplitude that will be deleted from \p
                                                   //!< minAmplitudes_priv next.
    QScopedArrayPointer<float> maxAmplitudes_priv; //!< Stores the max. amplitudes for \p amplitudeCount_priv rows
    int maxAmplitudesIndex_priv;                   //!< The index for the maximum amplitude that will be deleted from \p
                                                   //!< maxAmplitudes_priv next.
    unsigned int averageCount_priv = 0; //!< The number of amplitudes stored for both minimum and maximum amplitudes
    float minSum_priv = 0.0f;           //!< The sum of the contents of \p minAmplitudes_priv
    float maxSum_priv = 0.0f;           //!< The sum of the contents of \p maxAmplitudes_priv

    bool readingPackets_priv = false;   //!< True, if packet demodulation is currently running, false otherwise
    bool waterfallRunning_priv = false; //!< True, if waterfall diagram is currently running, false otherwise

public:
    explicit WaterfallItem(QQuickItem *parent = 0);
    virtual ~WaterfallItem();
    QSGNode *updatePaintNode(QSGNode *, UpdatePaintNodeData *);
    bool isStarted() const;
    bool isWaterfallActive() const;
    float sensitivity() const;
    void setSensitivity(float value);
    void setSamplesToWait(int value);
    int samplesToWait() const;
    float minValue() const;
    float maxValue() const;
    bool adaptiveColoring() const;

    AudioSampler *getAS();

    Q_INVOKABLE bool startWaterfall(int deviceIndex);
    Q_INVOKABLE bool startPackets(int deviceIndex);
    Q_INVOKABLE void stopWaterfall();
    Q_INVOKABLE void stopPackets();
    Q_INVOKABLE void clear();
    Q_INVOKABLE void setAdaptiveColoring(bool value);

private slots:
    void samplesCollected(double *amplitudeArray, unsigned int size);

signals:
    /**
     * @brief The signal is emitted whenever \p sampler_priv 's isStarted has changed.
     */
    void isStartedChanged();

    /**
     * @brief The signal is emitted whenever the state of the waterfall diagram has changed (running or not)
     */
    void isWaterfallActiveChanged();

    /**
     * @brief The signal is emitted whenver the number of samples that are FFTW'd together changes.
     * @param value The new number of samples.
     */
    void samplesToWaitChanged(const int value);

    /**
     * @brief The signal is emitted whenever the absolute minimum amplitude has changed.
     */
    void minValueChanged();

    /**
     * @brief The signal is emitted whenever the absolute maximum amplitude has changed.
     */
    void maxValueChanged();

    /**
     * @brief The signal is emitted whenever the packet demodulation state changes.
     * @param value The new state for packet demodulation.
     */
    void packetsAreBeingReadSignal(bool value);

    /**
     * @brief The signal is emitted whenever the waterfall diagram state changes.
     * @param value The new state for waterfall diagram.
     */
    void waterfallRunningSignal(bool value);
};

#endif // WATERFALLITEM_H
