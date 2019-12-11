
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

#include "waterfallitem.h"
#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>
#include <QtGui/QPainter>
#include <QtGui/QPixmap>
#include <QtQuick/QQuickWindow>
#include <QtQuick/QSGClipNode>
#include <QtQuick/QSGNode>
#include <QtQuick/QSGSimpleTextureNode>
#include <QtQuick/QSGTexture>
#include <cfloat>
#include <climits>
#include <cmath>

/**
 * @brief Constructor for the class.
 *
 * Initializes certain variables, properties and creates the necessary connections.
 *
 * @param parent The parent QQuickItem, should be left empty.
 */
WaterfallItem::WaterfallItem(QQuickItem *parent) : QQuickItem(parent) {

    imageNode_priv = nullptr;

    // Initialize connections
    QObject::connect(&sampler_priv, &AudioSampler::samplesCollected, this, &WaterfallItem::samplesCollected);
    QObject::connect(this, &WaterfallItem::samplesToWaitChanged, &sampler_priv, &AudioSampler::samplesToWaitChanged);
    QObject::connect(
        this, &WaterfallItem::packetsAreBeingReadSignal, &sampler_priv, &AudioSampler::packetsAreBeingReadChangedSlot);
    QObject::connect(
        this, &WaterfallItem::waterfallRunningSignal, &sampler_priv, &AudioSampler::waterfallRunningChangedSlot);

    // 504 Frequencies are used at 8192 sample count so that was chosen for the width
    this->setWidth(504);
    this->setHeight(150);

    // Initialize properties
    this->setVisible(true);
    this->setFlag(QQuickItem::ItemHasContents);
    samplesToWait_priv = sampler_priv.samplesToWait();

    image_priv.reset(new QImage((int) this->width(), (int) this->height(), QImage::Format_RGB32));
    image_priv->fill(QColor(255, 255, 255));
    imageLine_priv = image_priv->height();
    imageNode_priv = nullptr;
    image2_priv.reset(new QImage((int) this->width(), (int) this->height(), QImage::Format_RGB32));
    image2_priv->fill(QColor(255, 255, 255));
    imageNode2_priv = nullptr;
    imageNodeDestroy2_priv = false;
    clipNode_priv = nullptr;

    sensitivity_priv = 1;

    // Generate displayable colors
    colorsCount_priv = 504;
    QImage img(colorsCount_priv, 1, QImage::Format_RGB32);
    colors_priv.reset(new QRgb[colorsCount_priv]);
    QPainter painter;
    painter.begin(&img);
    QLinearGradient gradient;
    gradient.setStart(0, 0);
    gradient.setFinalStop(img.width(), 0);
    gradient.setColorAt(0, QColor(0, 0, 255));
    gradient.setColorAt(1.0 / 7, QColor(0, 128, 255));
    gradient.setColorAt(2.0 / 7, QColor(0, 255, 255));
    gradient.setColorAt(3.0 / 7, QColor(0, 255, 128));
    gradient.setColorAt(4.0 / 7, QColor(0, 255, 0));
    gradient.setColorAt(5.0 / 7, QColor(255, 255, 0));
    gradient.setColorAt(6.0 / 7, QColor(255, 128, 0));
    gradient.setColorAt(1, QColor(255, 0, 0));

    minAmplitudes_priv.reset(new float[amplitudeCount_priv]);
    maxAmplitudes_priv.reset(new float[amplitudeCount_priv]);
    minAmplitudesIndex_priv = 0;
    maxAmplitudesIndex_priv = 0;

    gradient.setSpread(QGradient::PadSpread);
    painter.fillRect(QRect(0, 0, img.width(), 1), QBrush(gradient));
    painter.end();
    for (int i = 0; i < img.width(); i++) {
        colors_priv[i] = 0xff000000 | img.pixel(i, 0);
    }

    timer_priv.setTimerType(Qt::CoarseTimer);
    timer_priv.setInterval(1000);
    timer_priv.setSingleShot(false);
    connect(&timer_priv, &QTimer::timeout, this, &WaterfallItem::maxValueChanged);
    connect(&timer_priv, &QTimer::timeout, this, &WaterfallItem::minValueChanged);

    // Repaint this item
    update();
}

/**
 * @brief Destructor for the class.
 *
 * Deletes the objects created with new.
 */
WaterfallItem::~WaterfallItem() {
    // Every resource created on the heap is automatically managed
}

/**
 * @brief Our own implementation of the QQuickItem function.
 * @param mainNode Check Qt documentation
 * @return Check Qt documentation
 */
QSGNode *WaterfallItem::updatePaintNode(QSGNode *mainNode, UpdatePaintNodeData *) {
    if (nullptr == mainNode) {
        mainNode = new QSGNode();
        mainNode->setFlag(QSGNode::OwnedByParent);
    }

    if (nullptr == clipNode_priv) {
        clipNode_priv = new QSGClipNode();
        clipNode_priv->setFlag(QSGNode::OwnedByParent);
        clipNode_priv->setIsRectangular(true);
        // We set this one a few lines later, may be unnecessary
        clipNode_priv->setClipRect(QRectF(0, 0, this->width(), this->height()));
        mainNode->appendChildNode(clipNode_priv);
    }

    // Update clip rectangle
    clipNode_priv->setClipRect(QRectF(0, 0, this->width(), this->height()));

    QQuickWindow::CreateTextureOptions textureOptions = 0;
#if (QT_VERSION >= QT_VERSION_CHECK(5, 6, 0))
    textureOptions = QQuickWindow::TextureIsOpaque;
#endif

    imageNodeTexture_priv.reset(this->window()->createTextureFromImage(*image_priv, textureOptions));

    // Scene graph node of first texture
    if (nullptr == imageNode_priv) {
        imageNode_priv = new QSGSimpleTextureNode();
        imageNode_priv->setFlag(QSGNode::OwnedByParent);
        clipNode_priv->appendChildNode(imageNode_priv);
    }

    imageNode_priv->setTexture(imageNodeTexture_priv.data());
    qreal ycoord = -imageLine_priv;
    imageNode_priv->setRect(0, ycoord, this->width(), this->height());

    if (nullptr == imageNodeTexture2_priv || imageNodeDestroy2_priv) {
        imageNodeTexture2_priv.reset(this->window()->createTextureFromImage(*image2_priv, textureOptions));
    }

    imageNodeDestroy2_priv = false;

    // Scene graph node of second texture
    if (nullptr == imageNode2_priv) {
        imageNode2_priv = new QSGSimpleTextureNode();
        imageNode2_priv->setFlag(QSGNode::OwnedByParent);
        clipNode_priv->appendChildNode(imageNode2_priv);
    }

    imageNode2_priv->setTexture(imageNodeTexture2_priv.data());
    imageNode2_priv->setRect(0, ycoord + image_priv->height(), this->width(), this->height());

    return mainNode;
}

/**
 * @brief Starts the waterfall diagram.
 *
 * Starts the audio sampler if it was not already started.
 * Starts \p timer_priv.
 *
 * @param deviceIndex Index of the audio input device.
 * @return True the device was successfully started, false otherwise.
 */
bool WaterfallItem::startWaterfall(int deviceIndex) {
    bool result = true;
    if (!sampler_priv.isStarted()) {
        result = sampler_priv.start(deviceIndex);
        if (!result) {
            return result;
        }
        emit isStartedChanged();
    }
    waterfallRunning_priv = true;
    timer_priv.start();
    emit isWaterfallActiveChanged();
    emit waterfallRunningSignal(true);
    clear();
    return result;
}

/**
 * @brief Starts packet demodulation.
 *
 * Starts the audio sampler if it was not already started and tells the demodulator to use \p packetSize.
 *
 * @param deviceIndex Index of the audio input device.
 * @return True the device was successfully started, false otherwise.
 */
bool WaterfallItem::startPackets(int deviceIndex) {
    readingPackets_priv = true;
    bool result = true;
    if (!sampler_priv.isStarted()) {
        result = sampler_priv.start(deviceIndex);
        emit isStartedChanged();
    }
    emit packetsAreBeingReadSignal(true);
    return result;
}

/**
 * @brief Stops the packet demodulation.
 */
void WaterfallItem::stopPackets() {
    readingPackets_priv = false;
    emit packetsAreBeingReadSignal(false);
    if (!waterfallRunning_priv && sampler_priv.isStarted()) {
        sampler_priv.stop();
        emit isStartedChanged();
    }
}

/**
 * @brief Stops the waterfall diagram.
 */
void WaterfallItem::stopWaterfall() {
    waterfallRunning_priv = false;
    timer_priv.stop();
    emit isWaterfallActiveChanged();
    emit waterfallRunningSignal(false);
    if (!readingPackets_priv && sampler_priv.isStarted()) {
        sampler_priv.stop();
        emit this->isStartedChanged();
    }
    minAmplitudesIndex_priv = 0;
    maxAmplitudesIndex_priv = 0;
    averageCount_priv = 0;
    minValue_priv = 0.0f;
    maxValue_priv = 0.0f;
    minSum_priv = 0.0f;
    maxSum_priv = 0.0f;
}

/**
 * @brief Gets the current state of the audio sampler.
 * @return Returns true if the audio sampler is running.
 */
bool WaterfallItem::isStarted() const {
    return sampler_priv.isStarted();
}

/**
 * @brief Gets the current state of the waterfall diagram.
 * @return Returns true if the waterfall diagram is running.
 */
bool WaterfallItem::isWaterfallActive() const {
    return waterfallRunning_priv;
}

/**
 * @brief Clears the waterfall diagram.
 */
void WaterfallItem::clear() {
    image_priv->fill(QColor(255, 255, 255));
    imageLine_priv = image_priv->height();
    image2_priv->fill(QColor(255, 255, 255));
    imageNodeDestroy2_priv = true;
    update();
}

// Adatptive coloring uses the maximum and minimum values from each line on the screen
// and uses two averages to color lines as they appear
/**
 * @brief Set the state of adaptive coloring to \p value.
 * @param value New setting for adaptive coloring.
 */
void WaterfallItem::setAdaptiveColoring(bool value) {
    if (value != adaptiveColoring_priv) {
        adaptiveColoring_priv = value;
    }
}

// Sensitivity is used as a linear scaling factor for non-adaptive coloring
/**
 * @brief Get the linear scaling factor for non-adaptive coloring
 * @return Returns the value of \p sensitivity_priv .
 */
float WaterfallItem::sensitivity() const {
    return sensitivity_priv;
}

/**
 * @brief Sets the linear scaling factor for non-adaptive coloring
 * @param value New scaling factor.
 */
void WaterfallItem::setSensitivity(float value) {
    sensitivity_priv = value;
}

/**
 * @brief Get the number of samples that are FFTW'd together.
 * @return Returns the value of \p samplesToWait_priv .
 */
int WaterfallItem::samplesToWait() const {
    return samplesToWait_priv;
}

// minValue and maxValue are used as labels when adaptivecoloring is on
/**
 * @brief Get the absolute minimum stored amplitude.
 * @return Returns the absolute minimum stored amplitude.
 */
float WaterfallItem::minValue() const {
    return minValue_priv;
}

/**
 * @brief Get the absolute maximum stored amplitude.
 * @return Returns the absolute maximum stored amplitude.
 */
float WaterfallItem::maxValue() const {
    return maxValue_priv;
}

/**
 * @brief Get the state of adaptive coloring.
 * @return Returns \p adaptiveColoring_priv.
 */
bool WaterfallItem::adaptiveColoring() const {
    return adaptiveColoring_priv;
}

/**
 * @brief Get the pointer to the audio sampler object.
 * @return Returns the pointer to \p sampler_priv.
 */
AudioSampler *WaterfallItem::getAS() {
    return &sampler_priv;
}

/**
 * @brief Set the number of samples to FFTW together.
 * @param value The new number of samples.
 */
void WaterfallItem::setSamplesToWait(int value) {
    if (samplesToWait_priv != value) {
        samplesToWait_priv = value;
        this->stopWaterfall();
        emit this->samplesToWaitChanged(value);
    }
}

// The draw surface refreshes upon receiving a batch of new amplitude values
/**
 * @brief A slot that is called whenever a new batch of amplitudes has been processed.
 * @param amplitudeArray The new amplitude values.
 * @param size The number of amplitudes in the array.
 */
void WaterfallItem::samplesCollected(double *amplitudeArray, unsigned int size) {

    if (adaptiveColoring_priv) {
        float min = 20 * std::log10(amplitudeArray[0]);
        float max = min;

        for (unsigned int x = 1; x < size; x++) {
            float amplitude = 20 * std::log10(amplitudeArray[x]);

            if (amplitude < min) {
                min = amplitude;
            }
            else if (amplitude > max) {
                max = amplitude;
            }
        }

        // The first min of 8192 is usually -Inf and that has to be handled properly.
        if (!(std::isinf(min) || std::isnan(min) || std::isinf(max) || std::isnan(max))) {
            if (averageCount_priv < amplitudeCount_priv) {
                minAmplitudes_priv[averageCount_priv] = min;
                maxAmplitudes_priv[averageCount_priv] = max;

                averageCount_priv++;
            }
            else {
                minSum_priv -= minAmplitudes_priv[minAmplitudesIndex_priv];
                maxSum_priv -= maxAmplitudes_priv[maxAmplitudesIndex_priv];
                minAmplitudes_priv[minAmplitudesIndex_priv] = min;
                maxAmplitudes_priv[maxAmplitudesIndex_priv] = max;

                minAmplitudesIndex_priv++;
                if (minAmplitudesIndex_priv >= amplitudeCount_priv) {
                    minAmplitudesIndex_priv = 0;
                }
                maxAmplitudesIndex_priv++;
                if (maxAmplitudesIndex_priv >= amplitudeCount_priv) {
                    maxAmplitudesIndex_priv = 0;
                }
            }

            minSum_priv += min;
            maxSum_priv += max;

            if (averageCount_priv != 0) {
                minValue_priv = minSum_priv / averageCount_priv;
                maxValue_priv = maxSum_priv / averageCount_priv;
            }
            else {
                minValue_priv = min;
                maxValue_priv = max;
            }
        }
    }

    // Get raw image data (1 pixel = 0xffRRGGBB)
    QRgb *imgData = (QRgb *) image_priv->bits();

    int xxlength = (8192 / samplesToWait_priv);
    int yystart = imageLine_priv - newRowHeight;
    if (yystart < 0) {
        yystart = 0;
    }
    int yyend = imageLine_priv;
    int imgWidth = image_priv->width();

    for (unsigned int x = 0; x < size; x++) {
        float amplitude = 20 * std::log10(amplitudeArray[x]);
        int value;

        if (adaptiveColoring_priv) {
            value = (int) ((503 / (maxValue_priv - minValue_priv)) * (amplitude - minValue_priv));
        }
        else {
            value = (int) (amplitude * sensitivity_priv * 0.005 * (float) colorsCount_priv);
        }

        if (value < 0) {
            value = 0;
        }
        else if (value >= colorsCount_priv) {
            value = colorsCount_priv - 1;
        }

        // Draw a colored rectangle by directly manipulating the image pixels
        // It is implemented this way in order to increase performance

        int xxstart = x * xxlength;
        int xxend = xxstart + xxlength;
        QRgb pixelColor = colors_priv[value];

        for (int yy = yystart; yy < yyend; yy++) {
            QRgb *line = imgData + (yy * imgWidth);
            for (int xx = xxstart; xx < xxend; xx++) {
                *(line + xx) = pixelColor;
            }
        }
    }

    imageLine_priv -= newRowHeight;

    // When first image is full, swap them
    if (imageLine_priv <= 0) {
        image_priv.swap(image2_priv);
        imageLine_priv = image_priv->height();
        imageNodeDestroy2_priv = true;
    }

    // Redraw the item
    update();
}
