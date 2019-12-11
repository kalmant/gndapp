#ifndef PACKETSJSONWRAPPER_H
#define PACKETSJSONWRAPPER_H

#include "../utilities/satellitechanger.h"
#include <QByteArray>
#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QList>

class PacketsJSONWrapper {
private:
    static constexpr char const *DATA_JSON = "packet";
    static constexpr char const *SATELLITE_JSON = "satellite";
    static constexpr char const *PACKETS_JSON = "packets";

    struct Packet {
        QString data;
        QString satelliteName;

        Packet() = delete;
        Packet(QString dataCTR, QString satelliteCTR);
        void write(QJsonObject &json) const;
    };
    QList<Packet> packets_priv;

    QString getPacketData(int index) const;
    QString getPacketSatelliteName(int index) const;
    void reset();

public:
    PacketsJSONWrapper();

    int getPacketCount() const;

    void addPacket(QString data, SatelliteChanger::Satellites satellite);
    void requeuePacket(int index);
    void removePacket(int index);
    void clearPackets();

    QByteArray writeToJSON(
        uint numberOfPackets = 0, QJsonDocument::JsonFormat format = QJsonDocument::JsonFormat::Compact) const;

    bool appendContentsFromFile(QString fileName);
    bool writeContentsToFile(QString fileName) const;
    bool writePacketToFile(QString fileName, int index) const;
};

#endif // PACKETSJSONWRAPPER_H
