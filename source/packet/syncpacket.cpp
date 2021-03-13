#include "syncpacket.h"
#include <QDebug>

namespace s1sync {

    const std::pair<unsigned int, OperatingMode> getSyncContents(
        QByteArray packet, SatelliteChanger::Satellites satellite) {
        if (packet.length() != syncPacketLength) {
            qWarning() << "Wrong sync packet length";
            return std::make_pair(0, OperatingMode::Invalid);
        }

        bool smog1 = satellite == SatelliteChanger::Satellites::SMOG1;
        bool maybeTxSync = smog1 ? shouldAccept(packet, syncSmog1TxArray) : shouldAccept(packet, syncDefaultArray);

        if (maybeTxSync) {
            auto params = packet.right(6);
            auto datarate = params.left(3);
            auto opmode = params.right(3);

            unsigned int darray[] = {500, 1250, 2500, 5000, 12500, 25000, 50000, 125000};
            unsigned int dindex =
                oneMajority(datarate[0]) * 4 + oneMajority(datarate[1]) * 2 + oneMajority(datarate[2]);
            unsigned int dr = darray[dindex];

            OperatingMode oparray[] = {OperatingMode::Receive,
                OperatingMode::AO40Short,
                OperatingMode::AO40,
                OperatingMode::RA_128_to_260,
                OperatingMode::RA_256_to_514,
                OperatingMode::RA_512_to_1028,
                OperatingMode::RA_1024_to_2050,
                OperatingMode::RA_2048_to_4100};
            unsigned int opindex = oneMajority(opmode[0]) * 4 + oneMajority(opmode[1]) * 2 + oneMajority(opmode[2]);
            OperatingMode om = oparray[opindex];

            return std::make_pair(dr, om);
        }

        if (shouldAccept(packet, syncDefaultArray)) {
            return std::make_pair(5000, OperatingMode::Receive);
        }
        else if (smog1 && shouldAccept(packet, syncSmog1HamRxArray)) {
            return std::make_pair(5000, OperatingMode::HamReceive);
        }

        qWarning() << "Sync packet is invalid, perhaps does not start with the specified prefix";
        return std::make_pair(0, OperatingMode::Invalid);
    }

    const QString operatingModeToQString(OperatingMode opmode) {
        switch (opmode) {
        case OperatingMode::AO40Short:
            return "AO40Short";
        case OperatingMode::AO40:
            return "AO40";
        case OperatingMode::RA_128_to_260:
            return "RA128";
        case OperatingMode::RA_256_to_514:
            return "RA256";
        case OperatingMode::RA_512_to_1028:
            return "RA512";
        case OperatingMode::RA_1024_to_2050:
            return "RA1024";
        case OperatingMode::RA_2048_to_4100:
            return "RA2048";
        case OperatingMode::Receive:
            return "Receive";
        case OperatingMode::HamReceive:
            return "HAM receive";
        default:
            return "Invalid";
        }
    }

} // namespace s1sync
