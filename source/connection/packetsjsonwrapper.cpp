#include "packetsjsonwrapper.h"
#include <QFile>

PacketsJSONWrapper::PacketsJSONWrapper() {
}

int PacketsJSONWrapper::getPacketCount() const {
    return packets_priv.length();
}

QString PacketsJSONWrapper::getPacketData(int index) const {
    Q_ASSERT((index < 0) || (index >= packets_priv.length()));
    return packets_priv.at(index).data;
}

QString PacketsJSONWrapper::getPacketSatelliteName(int index) const {

    Q_ASSERT((index < 0) || (index >= packets_priv.length()));
    return packets_priv.at(index).satelliteName;
}

void PacketsJSONWrapper::addPacket(QString data, SatelliteChanger::Satellites satellite) {
    Q_ASSERT(data != nullptr);
    switch (satellite) {
    case SatelliteChanger::Satellites::SMOG1:
        packets_priv.append(Packet(data, "smog1"));
        break;
    case SatelliteChanger::Satellites::SMOGP:
        packets_priv.append(Packet(data, "smogp"));
        break;
    case SatelliteChanger::Satellites::ATL1:
        packets_priv.append(Packet(data, "atl1"));
        break;
    case SatelliteChanger::Satellites::UNSET:
        return;
    }
}

void PacketsJSONWrapper::requeuePacket(int index) {
    if ((index < 0) || (index >= packets_priv.length())) {
        qWarning() << "Tried to requeue packet with invalid index: " + QString::number(index);
    }
    else {
        packets_priv.append(packets_priv.at(index));
    }
}

void PacketsJSONWrapper::removePacket(int index) {
    if ((index < 0) || (index >= packets_priv.length())) {
        qWarning() << "Tried to remove packet with invalid index: " + QString::number(index);
    }
    else {
        packets_priv.removeAt(index);
    }
}

void PacketsJSONWrapper::clearPackets() {
    packets_priv.clear();
}

QByteArray PacketsJSONWrapper::writeToJSON(uint numberOfPackets, QJsonDocument::JsonFormat format) const {
    QJsonObject obj;
    QJsonArray packets;
    bool countLimit = numberOfPackets > 0;
    for (const Packet packet : packets_priv) {
        QJsonObject packetObject;
        packet.write(packetObject);
        packets.append(packetObject);
        if ((countLimit) && (--numberOfPackets == 0)) {
            break;
        }
    }
    obj[PACKETS_JSON] = packets;
    QJsonDocument doc(obj);
    return doc.toJson(format);
}

bool PacketsJSONWrapper::appendContentsFromFile(QString fileName) {
    QFile file(fileName);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream reader(&file);
        while (!reader.atEnd()) {
            QString line = reader.readLine();
            auto results = line.split("|", QString::SkipEmptyParts);
            if (results.length() != 2) {
                file.close();
                return false;
            }
            packets_priv.append(Packet(results[1], results[0]));
        }
        file.close();
        return true;
    }
    return false;
}

bool PacketsJSONWrapper::writeContentsToFile(QString fileName) const {
    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream writer(&file);
        for (const Packet packet : packets_priv) {
            writer << packet.satelliteName << "|" << packet.data << endl;
        }
        file.close();
        return true;
    }
    return false;
}

bool PacketsJSONWrapper::writePacketToFile(QString fileName, int index) const {
    if ((index < 0) || (index >= packets_priv.length())) {
        qWarning() << "Tried to write packet to file with invalid index: " + QString::number(index);
        return false;
    }
    QFile file(fileName);
    if (file.open(QIODevice::Append | QIODevice::Text)) {
        QTextStream writer(&file);
        auto packet = packets_priv.at(index);
        writer << packet.satelliteName << "|" << packet.data << endl;
        file.close();
        return true;
    }
    return false;
}

PacketsJSONWrapper::Packet::Packet(QString dataCTR, QString satelliteCTR) {
    Q_ASSERT(dataCTR != nullptr);
    Q_ASSERT(satelliteCTR != nullptr);
    this->data = dataCTR;
    this->satelliteName = satelliteCTR;
}

void PacketsJSONWrapper::Packet::write(QJsonObject &json) const {
    json[PacketsJSONWrapper::DATA_JSON] = data;
    json[PacketsJSONWrapper::SATELLITE_JSON] = satelliteName;
}
