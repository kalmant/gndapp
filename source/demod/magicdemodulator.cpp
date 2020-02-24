#include "magicdemodulator.h"

MagicDemodulator::MagicDemodulator(long sampling_rate, long datarate, QString source_string, QObject *parent)
: QObject(parent), starting_index(datarate != 1250) {
    this->sampling_rate = sampling_rate;
    this->datarate = datarate;
    this->source_string = source_string;

    if (sampling_rate % datarate != 0) {
        qCritical() << "Invalid sampling_rate and datarate pair:" << sampling_rate << " - " << datarate;
    }

    for (int i = starting_index; i < PACKET_SIZES_COUNT; i++) {
        packet_buffers[i].reserve(packet_lengths[i]);
    }

    reinitialize();
}

MagicDemodulator::~MagicDemodulator() {
    free(avg_vars.buf);
    free(avg_dec_vars.buf);
}

void MagicDemodulator::addSample(std::complex<float> sample) {
    auto avgd = average(&avg_vars, sample);
    std::complex<float> avg_dec_output;
    bool avg_dec_performed = average_dec(&avg_dec_vars, avgd, &avg_dec_output);
    if (avg_dec_performed) {
        auto demod_output = smog_atl_demodulate(&demod_vars, avg_dec_output);
        if (dem_a_set) {
            dem_b = demod_output;
            dem_a_set = false;
            for (int i = starting_index; i < PACKET_SIZES_COUNT; i++) {
                packet_buffers[i].reserve(packet_lengths[i]);

                auto decd = make_hard_decision(&dec_vars[i], dem_a, dem_b, packet_lengths[i]);
                if (decd != -1) {
                    packet_buffers[i].append(char(decd));
                    if (packet_buffers[i].length() == packet_lengths[i]) {
                        QDateTime timestamp = QDateTime::currentDateTimeUtc();
                        QString packetUpperHexString = QString(
                            QByteArray(reinterpret_cast<char *>(packet_buffers[i].data()), packet_lengths[i]).toHex())
                                                           .toUpper();
                        emit dataReady(timestamp, source_string, packetUpperHexString);
                        packet_buffers[i].clear();
                    }
                }
            }
        }
        else {
            dem_a = demod_output;
            dem_a_set = true;
        }
    }
}

void MagicDemodulator::clear() {
    reinitialize();
}

void MagicDemodulator::reinitialize() {
    for (int i = starting_index; i < PACKET_SIZES_COUNT; i++) {
        packet_buffers[i].clear();
    }
    reinitialize_avg_vars(&avg_vars, sampling_rate / datarate);
    reinitialize_avg_dec_vars(&avg_dec_vars, sampling_rate / datarate / 2); // 2 samples / bit
    reinitialize_demod_vars(&demod_vars);
    for (int i = starting_index; i < PACKET_SIZES_COUNT; i++) {
        reinitialize_dec_vars(&dec_vars[i]);
    }
}

void MagicDemodulator::reset() {
    clear();
}
