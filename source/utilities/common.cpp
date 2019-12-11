#include "common.h"

const QString getDTSFromUint32UTC(uint32_t time) {
    return QDateTime::fromSecsSinceEpoch(time, QTimeZone::utc()).toString(Qt::ISODate);
}
