#ifndef COMMON_H
#define COMMON_H

#include <QString>
#include <QThread>
#include <QTimeZone>
#include <bitset>

#define INVALIDATE 50000

/**
 * @brief Returns a QString  on a QDateTime object created from \p time using QTimeZone::utc()
 * @param time The uint32_t representing the time
 * @return The formatted QString
 */
const QString getDTSFromUint32UTC(uint32_t time);

struct QScopedPointerThreadDeleter {
    static inline void cleanup(QThread *pointer) {
        pointer->quit();
        pointer->wait();
        delete pointer;
    }
};

inline unsigned int count_ones(uint8_t byte) {
    return std::bitset<8>(byte).count();
}

#endif // COMMON_H
