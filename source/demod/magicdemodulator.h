#ifndef MAGICDEMODULATOR_H
#define MAGICDEMODULATOR_H

#include "../utilities/satellitechanger.h"
#include "newsmog1dem.h"
#include <QDateTime>
#include <QObject>
#include <QVector>
#include <QtDebug>
#include <complex>

class MagicDemodulator : public QObject {
    Q_OBJECT
public:
    MagicDemodulator(long sampling_rate, long datarate, QString source_string, QObject *parent = 0);
    ~MagicDemodulator();
    void addSample(std::complex<float> sample);
    void clear();

private:
    static constexpr int PACKET_SIZES_COUNT = 5;
    static constexpr int SMOG1_OFFSET = 5;

    int current_offset = 0;

    long sampling_rate;
    long datarate;
    QString source_string;
    const int starting_index; // Ensures that only 1250 BPS demodulates sync packet lengthed packets

    int packet_lengths[10] = {70, 260, 333, 514, 650, 70, 268, 341, 522, 658};
    QVector<char> packet_buffers[10] = {QVector<char>(),
        QVector<char>(),
        QVector<char>(),
        QVector<char>(),
        QVector<char>(),
        QVector<char>(),
        QVector<char>(),
        QVector<char>(),
        QVector<char>(),
        QVector<char>()};

    AveragingVariables avg_vars;
    AveragingDecVariables avg_dec_vars;
    DemodulationVariables demod_vars;
    DecisionVariables dec_vars[10];

    std::complex<float> dem_a;
    std::complex<float> dem_b;
    bool dem_a_set = false;
    void reinitialize();

signals:
    void dataReady(QDateTime timestamp, QString source, QString packetUpperHexString);

public slots:
    void reset();
    void newSatelliteSlot(SatelliteChanger::Satellites satellite);
};

#endif // MAGICDEMODULATOR_H
