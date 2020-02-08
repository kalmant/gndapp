#ifndef SPECTOGRAM_H
#define SPECTOGRAM_H

#include <../../3rdparty/fftw3/fftw3.h>
#include <QImage>
#include <QPainter>
#include <QQuickItem>
#include <QQuickWindow>
#include <QSGClipNode>
#include <QSGNode>
#include <QSGSimpleTextureNode>
#include <QSGTexture>
#include <QScopedPointer>
#include <complex>

enum SpectogramMode { radio, sdr, unset };

/**
 * @brief This class handles FFTW operations and the drawing of the result.
 *
 * This component runs FFTW when the given number of samples have arrived and performs no other checking related to
 * samples.
 */
class Spectogram : public QQuickItem {
    Q_OBJECT
    Q_PROPERTY(bool running READ isRunning NOTIFY isRunningChanged)
    Q_PROPERTY(bool inRadioMode READ inRadioMode)
    Q_PROPERTY(bool inSDRMode READ inSDRMode)
    Q_PROPERTY(long minimumFrequency READ minimumFrequency NOTIFY minimumFrequencyChanged)
    Q_PROPERTY(long maximumFrequency READ maximumFrequency NOTIFY maximumFrequencyChanged)

    bool is_running = false;
    SpectogramMode current_mode = SpectogramMode::unset;

    long minimum_frequency = 0.0f;
    void setMinimumFrequency(long minimum_frequency);
    long maximum_frequency = 0.0f;
    void setMaximumFrequency(long maximum_frequency);
    void changeSettings(SpectogramMode mode);

    // FFTW related variables
    fftw_plan complex_plan;
    fftw_complex *complex_samples = nullptr;
    int complex_sample_count = 0;
    fftw_complex *complex_fftw_out;

    fftw_plan real_plan;
    double *real_samples = nullptr;
    int real_sample_count = 0;
    fftw_complex *real_fftw_out;

    static constexpr int sample_target = 1024;

    // Drawing related variables
    static constexpr unsigned int item_width = 512;  //!< The height of the item
    static constexpr unsigned int item_height = 150; //!< The height of the item
    static constexpr int unit_height = 4;            //!< The height of each row in pixels

    QScopedArrayPointer<QRgb> colors;
    static constexpr int color_count = item_width;
    QSGClipNode *clip_node; //!< Pointer to the SQGClipNode object, its parent will delete it
    QScopedPointer<QImage> image;
    QSGSimpleTextureNode *image_node; // This class is deprecated! Its parent will delete it
    QScopedPointer<QSGTexture, QScopedPointerDeleteLater> image_node_texture;
    int image_line;
    QScopedPointer<QImage> image2;
    QSGSimpleTextureNode *image_node2; // This class is deprecated! Its parent will delete it
    QScopedPointer<QSGTexture, QScopedPointerDeleteLater> image_node_texture2;
    bool image_node_texture2_expired;

    void clear(); //!< Clears the image
    void setIsRunning(bool is_running);
    QVector<float> getAmplitudes(SpectogramMode mode, fftw_complex *fftw_out) const;
    void draw(QVector<float> processed_amplitudes);

public:
    explicit Spectogram(QQuickItem *parent = nullptr);
    ~Spectogram();
    QSGNode *updatePaintNode(QSGNode *, UpdatePaintNodeData *);

    bool isRunning() const;
    bool inSDRMode() const;
    bool inRadioMode() const;
    long minimumFrequency() const;
    long maximumFrequency() const;

    Q_INVOKABLE void switchToRadioMode();
    Q_INVOKABLE void switchToSDRMode();
    Q_INVOKABLE void startSpectogram();
    Q_INVOKABLE void stopSpectogram();

public slots:
    void realSamplesReceived(int16_t *samples, int sample_count);
    void complexSampleReceived(std::complex<float> sample);
signals:
    void isRunningChanged();
    void minimumFrequencyChanged();
    void maximumFrequencyChanged();
};

#endif // SPECTOGRAM_H
