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
    MagicDemodulator(long sampling_rate, long datarate, QString source_string, QObject *parent = 0);
    ~MagicDemodulator();
    void addSample(std::complex<float> sample);
    void clear();

private:
    static constexpr int PACKET_SIZES_COUNT = 5;

    long sampling_rate;
    long datarate;
    QString source_string;

    int packet_lengths[5] = {70, 260, 333, 514, 650};
    QVector<char> packet_buffers[5] = {
        QVector<char>(), QVector<char>(), QVector<char>(), QVector<char>(), QVector<char>()};

    AveragingVariables avg_vars;
    AveragingDecVariables avg_dec_vars;
    DemodulationVariables demod_vars;
    DecisionVariables dec_vars[5];

    std::complex<float> dem_a;
    std::complex<float> dem_b;
    bool dem_a_set = false;
    void reinitialize();

signals:
    void dataReady(QDateTime timestamp, QString source, QString packetUpperHexString);
};

#endif // MAGICDEMODULATOR_H
