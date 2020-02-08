#ifndef MAGICDEMODULATOR_H
#define MAGICDEMODULATOR_H

#include "newsmog1dem.h"
#include <QDateTime>
#include <QObject>
#include <QVector>
#include <QtDebug>
#include <complex>

class MagicDemodulator : public QObject {
    Q_OBJECT
public:
    MagicDemodulator(long sampling_rate, long datarate, QString source_string);
    ~MagicDemodulator();
    void addSample(std::complex<float> sample);
    void clear();

private:
    static constexpr int PACKET_SIZES_COUNT = 3;

    long sampling_rate;
    long datarate;
    QString source_string;

    int packet_lengths[3] = {260, 333, 514};
    QVector<char> packet_buffers[3] = {QVector<char>(), QVector<char>(), QVector<char>()};

    AveragingVariables avg_vars;
    AveragingDecVariables avg_dec_vars;
    DemodulationVariables demod_vars;
    DecisionVariables dec_vars[3];

    std::complex<float> dem_a;
    std::complex<float> dem_b;
    bool dem_a_set = false;
    void reinitialize();

signals:
    void dataReady(QDateTime timestamp, QString source, QString packetUpperHexString);
};

#endif // MAGICDEMODULATOR_H
