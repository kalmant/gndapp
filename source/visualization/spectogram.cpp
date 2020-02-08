// License for the original WaterfallItem code that this class is based on:
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

#include "spectogram.h"

void Spectogram::clear() {
    complex_sample_count = 0;
    real_sample_count = 0;

    // Clearing the screen
    image->fill(QColor(255, 255, 255));
    image_line = image->height();
    image2->fill(QColor(255, 255, 255));
    image_node_texture2_expired = true;
    update();
}

void Spectogram::setIsRunning(bool is_running) {
    if (this->is_running != is_running) {
        this->is_running = is_running;
        emit isRunningChanged();
    }
}

QVector<float> Spectogram::getAmplitudes(SpectogramMode mode, fftw_complex *fftw_out) const {
    QVector<float> results;
    switch (mode) {
    case SpectogramMode::radio:
        results.reserve(64);
        static constexpr int offset = 7;
        for (int i = offset; i < offset + 64; i++) {
            results.append(20 * std::log10(sqrt(fftw_out[i][0] * fftw_out[i][0] + fftw_out[i][1] * fftw_out[i][1])));
        }
        return results;
    case SpectogramMode::sdr:
        // Note: We have 1024 samples but only 512 can be drawn
        for (int i = 0; i < 512; i++) {
            results.append(20 * std::log10(sqrt(fftw_out[2 * i][0] * fftw_out[2 * i][0] +
                                                fftw_out[2 * i][1] * fftw_out[2 * i][1]) +
                                           sqrt(fftw_out[2 * i + 1][0] * fftw_out[2 * i + 1][0] +
                                                fftw_out[2 * i + 1][1] * fftw_out[2 * i + 1][1])));
        }
        return results;
    default:
        qCritical() << "mode is unset";
        return results;
    }
}

Spectogram::Spectogram(QQuickItem *parent) : QQuickItem(parent) {
    this->setWidth(item_width);
    this->setHeight(item_height);

    this->setVisible(true);
    this->setFlag(QQuickItem::ItemHasContents);

    image.reset(new QImage((int) this->width(), (int) this->height(), QImage::Format_RGB32));
    image->fill(QColor(255, 255, 255));
    image_line = image->height();
    image_node = nullptr;
    image2.reset(new QImage((int) this->width(), (int) this->height(), QImage::Format_RGB32));
    image2->fill(QColor(255, 255, 255));
    image_node2 = nullptr;
    image_node_texture2_expired = false;
    clip_node = nullptr;

    // Generate displayable colors
    QImage img(item_width, 1, QImage::Format_RGB32);
    colors.reset(new QRgb[item_width]);
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

    gradient.setSpread(QGradient::PadSpread);
    painter.fillRect(QRect(0, 0, img.width(), 1), QBrush(gradient));
    painter.end();
    for (int i = 0; i < img.width(); i++) {
        colors[i] = 0xff000000 | img.pixel(i, 0);
    }

    // Repaint this item
    update();

    // Complex FFTW
    // http://www.fftw.org/doc/Complex-One_002dDimensional-DFTs.html
    complex_samples = (fftw_complex *) fftw_malloc(sizeof(fftw_complex) * (sample_target));
    complex_fftw_out = (fftw_complex *) fftw_malloc(sizeof(fftw_complex) * (sample_target));
    complex_plan = fftw_plan_dft_1d(sample_target, complex_samples, complex_fftw_out, FFTW_FORWARD, FFTW_ESTIMATE);

    // Real FFTW
    real_samples = (double *) fftw_malloc(sizeof(double) * (sample_target));
    real_fftw_out = (fftw_complex *) fftw_malloc(sizeof(fftw_complex) * (sample_target / 2 + 1));
    real_plan = fftw_plan_dft_r2c_1d(sample_target, real_samples, real_fftw_out, FFTW_ESTIMATE);
}

Spectogram::~Spectogram() {
    fftw_destroy_plan(complex_plan);
    fftw_free(complex_fftw_out);
    fftw_free(complex_samples);

    fftw_destroy_plan(real_plan);
    fftw_free(real_fftw_out);
    fftw_free(real_samples);

    fftw_cleanup();
}

bool Spectogram::isRunning() const {
    return is_running;
}

bool Spectogram::inSDRMode() const {
    return current_mode == SpectogramMode::sdr;
}

bool Spectogram::inRadioMode() const {
    return current_mode == SpectogramMode::radio;
}

long Spectogram::minimumFrequency() const {
    return minimum_frequency;
}

long Spectogram::maximumFrequency() const {
    return maximum_frequency;
}

void Spectogram::switchToRadioMode() {
    changeSettings(SpectogramMode::radio);
}

void Spectogram::switchToSDRMode() {
    changeSettings(SpectogramMode::sdr);
}

void Spectogram::startSpectogram() {
    if (current_mode == SpectogramMode::unset) {
        qWarning() << "Can not start spectogram in unset mode";
        return;
    }
    changeSettings(current_mode);
    setIsRunning(true);
}

void Spectogram::stopSpectogram() {
    setIsRunning(false);
}

void Spectogram::changeSettings(SpectogramMode mode) {
    switch (mode) {
    case SpectogramMode::radio:
        // Sampling rate is 44.1kHz for audio
        minimum_frequency = 300;  // Not exactly accurate
        maximum_frequency = 3050; // Not exactly accurate
        break;
    case SpectogramMode::sdr:
        // Sampling rate is 2.5kHz for SDR spectogram
        minimum_frequency = -1250;
        maximum_frequency = 1250; // Not exactly accurate
        break;
    default:
        qWarning() << "Can not set mode to unset";
        return;
    }

    current_mode = mode;
    emit minimumFrequencyChanged();
    emit maximumFrequencyChanged();
    clear();
}

void Spectogram::realSamplesReceived(int16_t *samples, int sample_count) {
    QScopedArrayPointer<std::int16_t> samples_received(samples);
    if (!is_running || current_mode != SpectogramMode::radio) {
        return;
    }
    for (int i = 0; i < sample_count; i++) {
        real_samples[real_sample_count] = static_cast<double>(samples_received[i]);
        real_sample_count++;
        if (real_sample_count == sample_target) {
            fftw_execute(real_plan);
            draw(getAmplitudes(current_mode, real_fftw_out));
            real_sample_count = 0;
        }
    }
}

void Spectogram::complexSampleReceived(std::complex<float> sample) {
    if (!isRunning() || current_mode != SpectogramMode::sdr) {
        return;
    }

    complex_samples[complex_sample_count][0] = static_cast<double>(sample.real());
    complex_samples[complex_sample_count][1] = static_cast<double>(sample.imag());
    complex_sample_count++;
    if (complex_sample_count == sample_target) {
        fftw_execute(complex_plan);
        auto amplitudes = getAmplitudes(current_mode, complex_fftw_out);
        draw(getAmplitudes(current_mode, complex_fftw_out));
        complex_sample_count = 0;
    }
}

QSGNode *Spectogram::updatePaintNode(QSGNode *mainNode, UpdatePaintNodeData *) {
    if (nullptr == mainNode) {
        mainNode = new QSGNode();
        mainNode->setFlag(QSGNode::OwnedByParent);
    }

    if (nullptr == clip_node) {
        clip_node = new QSGClipNode();
        clip_node->setFlag(QSGNode::OwnedByParent);
        clip_node->setIsRectangular(true);
        clip_node->setClipRect(QRectF(0, 0, this->width(), this->height()));
        mainNode->appendChildNode(clip_node);
    }

    QQuickWindow::CreateTextureOptions textureOptions = 0;
#if (QT_VERSION >= QT_VERSION_CHECK(5, 6, 0))
    textureOptions = QQuickWindow::TextureIsOpaque;
#endif

    image_node_texture.reset(this->window()->createTextureFromImage(*image, textureOptions));

    // Scene graph node of first texture
    if (nullptr == image_node) {
        image_node = new QSGSimpleTextureNode();
        image_node->setFlag(QSGNode::OwnedByParent);
        clip_node->appendChildNode(image_node);
    }

    image_node->setTexture(image_node_texture.data());
    qreal ycoord = -image_line;
    image_node->setRect(0, ycoord, this->width(), this->height());

    if (nullptr == image_node_texture2 || image_node_texture2_expired) {
        image_node_texture2.reset(this->window()->createTextureFromImage(*image2, textureOptions));
    }

    image_node_texture2_expired = false;

    // Scene graph node of second texture
    if (nullptr == image_node2) {
        image_node2 = new QSGSimpleTextureNode();
        image_node2->setFlag(QSGNode::OwnedByParent);
        clip_node->appendChildNode(image_node2);
    }

    image_node2->setTexture(image_node_texture2.data());
    image_node2->setRect(0, ycoord + image->height(), this->width(), this->height());

    return mainNode;
}

void Spectogram::draw(QVector<float> processed_amplitudes) {
    // Get raw image data (1 pixel = 0xffRRGGBB)
    QRgb *imgData = (QRgb *) image->bits();

    int yystart = image_line - unit_height;
    if (yystart < 0) {
        yystart = 0;
    }
    int yyend = image_line;
    int imgWidth = image->width();
    int unit_width = imgWidth / processed_amplitudes.size();

    float min, max;
    min = max = processed_amplitudes[0];
    for (auto amplitude : processed_amplitudes) {
        if (amplitude < min) {
            min = amplitude;
        }
        if (amplitude > max) {
            max = amplitude;
        }
    }

    for (unsigned int x = 0; x < processed_amplitudes.size(); x++) {
        float amplitude = processed_amplitudes[x];
        int color_index;

        color_index = (int) ((color_count - 1) / (max - min) * (amplitude - min));

        if (color_index < 0) {
            color_index = 0;
        }
        else if (color_index >= color_count) {
            color_index = color_count - 1;
        }

        // Draw a colored rectangle by directly manipulating the image pixels
        // It is implemented this way in order to increase performance

        int xxstart = x * unit_width;
        int xxend = xxstart + unit_width;
        QRgb pixelColor = colors[color_index];

        for (int yy = yystart; yy < yyend; yy++) {
            QRgb *line = imgData + (yy * imgWidth);
            for (int xx = xxstart; xx < xxend; xx++) {
                *(line + xx) = pixelColor;
            }
        }
    }

    image_line -= unit_height;

    // When first image is full, swap them
    if (image_line <= 0) {
        image.swap(image2);
        image_line = image->height();
        image_node_texture2_expired = true;
    }

    // Redraw the item
    update();
}
