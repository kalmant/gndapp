#ifndef SYNCPACKET_H
#define SYNCPACKET_H

#include "../utilities/common.h"
#include "../utilities/satellitechanger.h"
#include <QByteArray>
#include <utility>

namespace s1sync {

    // clang-format off
    static const uint8_t syncBytes[64] = {
        0x97,
        0xfd, 0xd3, 0x7b, 0x0f, 0x1f, 0x6d, 0x08,
        0xf7, 0x83, 0x5d, 0x9e, 0x59, 0x82, 0xc0, 0xfd,
        0x1d, 0xca, 0xad, 0x3b, 0x5b, 0xeb, 0xd4, 0x93,
        0xe1, 0x4a, 0x04, 0xd2, 0x28, 0xdd, 0xf9, 0x01,
        0x53, 0xd2, 0xe6, 0x6c, 0x5b, 0x25, 0x65, 0x31,
        0xc5, 0x7c, 0xe7, 0xf1, 0x38, 0x61, 0x2d, 0x5c,
        0x03, 0x3a, 0xc6, 0x88, 0x90, 0xdb, 0x8c, 0x8c,
        0x42, 0xf3, 0x51, 0x75, 0x43, 0xa0, 0x83, 0x93,
    };
    static const uint8_t syncBytesSmog1Tx[64] = {
        0xA3,
        0x9E, 0x1A, 0x55, 0x6B, 0xCB, 0x5C, 0x2F,
        0x2A, 0x5C, 0xAD, 0xD5, 0x32, 0xFE, 0x85, 0x1D,
        0xDC, 0xE8, 0xBC, 0xE5, 0x13, 0x7E, 0xBA, 0xBD,
        0x9D, 0x44, 0x31, 0x51, 0x3C, 0x92, 0x26, 0x6C,
        0xF3, 0x68, 0x98, 0xDA, 0xA3, 0xBA, 0x7F, 0x84,
        0x86, 0x32, 0x95, 0xAC, 0x8D, 0x4E, 0x66, 0x8B,
        0x7F, 0x7B, 0xE0, 0x14, 0xE2, 0x3C, 0x49, 0x45,
        0x32, 0xE4, 0x5C, 0x44, 0xF5, 0x6D, 0x2D, 0x0A,
    };
    static const uint8_t syncBytesSmog1HamRepeaterRx[64] = {
        0xE0,
        0x7F, 0x06, 0x9A, 0x24, 0xE2, 0x57, 0x5B,
        0x37, 0x7E, 0x39, 0xC9, 0x4B, 0xC0, 0x7A, 0xEE,
        0x13, 0xFB, 0xFA, 0x9E, 0xF8, 0x37, 0x13, 0x44,
        0xE2, 0x2B, 0x04, 0x13, 0x20, 0xC0, 0x24, 0xF9,
        0xB5, 0x92, 0x0F, 0x1E, 0xB7, 0x4F, 0x54, 0xC5,
        0x9D, 0x16, 0x73, 0xD0, 0x22, 0x45, 0x07, 0xD2,
        0x4B, 0xC2, 0x47, 0xFA, 0x85, 0x21, 0xA7, 0x0C,
        0xB6, 0xBC, 0x0B, 0x74, 0x61, 0xCD, 0xEB, 0xF6,
    };
    // clang-format on

    static constexpr size_t syncBytesLength = sizeof(syncBytes);
    static constexpr size_t syncPacketLength = syncBytesLength + 6;

    static const QByteArray syncDefaultArray(reinterpret_cast<const char *>(syncBytes), syncBytesLength);
    static const QByteArray syncSmog1TxArray(reinterpret_cast<const char *>(syncBytesSmog1Tx), syncBytesLength);
    static const QByteArray syncSmog1HamRxArray(
        reinterpret_cast<const char *>(syncBytesSmog1HamRepeaterRx), syncBytesLength);

    enum class OperatingMode {
        Receive,
        AO40Short,
        AO40,
        RA_128_to_260,
        RA_256_to_514,
        RA_512_to_1028,
        RA_1024_to_2050,
        RA_2048_to_4100,
        Invalid,
        HamReceive
    };

    /**
     * @brief Decodes \p packet as a sync packet
     * @param packet The packet to decode as a sync packet
     * @param satellite The satellite currently in use
     * @return A pair describing the result of the decoding process
     */
    const std::pair<unsigned int, OperatingMode> getSyncContents(
        QByteArray packet, SatelliteChanger::Satellites satellite);

    /**
     * @brief Returns the QString representation of the particular OperatingMode in \p opmode
     * @param opmode The operating mode
     * @return The QString representing \p opmode
     */
    const QString operatingModeToQString(OperatingMode opmode);

    /**
     * @brief Returns whether \p byte 's bits are mostly 1
     * @param byte the char (byte) to check for 1 majority
     * @return True if there are more than four 1's in \p byte
     */
    inline bool oneMajority(char byte) {
        return count_ones(static_cast<uint8_t>(byte)) > 4;
    }

    /**
     * @brief shouldAccept Returns true if \p packet starts with syncBytes (at least one third matches)
     * @param packet The packet to check
     * @return True if at least one third of the start of the packet matches syncBytes
     */
    inline bool shouldAccept(const QByteArray &packet, const QByteArray &syncBytes) {
        if (static_cast<unsigned int>(packet.length()) < syncBytesLength) {
            return false;
        }
        auto data = packet.left(static_cast<int>(syncBytesLength));
        unsigned int matches = 0;
        for (unsigned int i = 0; i < syncBytesLength; i++) {
            auto xored = data[i] ^ syncBytes[i];
            matches += (8 - count_ones(static_cast<uint8_t>(xored)));
        }
        // Fix sync packet detection so it doesn't confuse the different sync packet types
        return matches >= ((syncBytesLength * 8) / 3 * 2);
    }

} // namespace s1sync

#endif // SYNCPACKET_H
