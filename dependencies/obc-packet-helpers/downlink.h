
// This file is part of the SMOG-1 Utilities library.
// Copyright (c) 2016-2019 Timur Kristóf
// Licensed to you under the terms of the MIT license.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#ifndef S1OBC_DOWNLINK_H
#define S1OBC_DOWNLINK_H

#include "telemetry.h"

namespace s1obc {

    struct DownlinkSignature {
        uint8_t bytes[10];
    };

    using AcknowledgedCommandBase = s1utils::Pack<uint16_t, uint8_t>;
    class AcknowledgedCommand : public AcknowledgedCommandBase {
    private:
        Q_GADGET
        Q_PROPERTY(quint16 commandId READ commandId WRITE setCommandId)
        Q_PROPERTY(qint16 receivedRssi READ receivedRssi WRITE setReceivedRssi)

    public:
        uint16_t commandId() const {
            return get<0>();
        }
        void setCommandId(uint16_t val) {
            set<0>(val);
        }
        Rssi receivedRssi() const {
            return static_cast<Rssi>(get<1>() / 2.0 - 131);
        }
        void setReceivedRssi(Rssi val) {
            set<1>(static_cast<uint8_t>((val + 131) * 2));
        }

        AcknowledgedCommand() : AcknowledgedCommand(0, 0) {
        }

        AcknowledgedCommand(uint16_t id, int8_t rssi) {
            setCommandId(id);
            setReceivedRssi(rssi);
        }
    };

    class AcknowledgedCommands {
    private:
        Q_GADGET

    public:
        static constexpr size_t max = 3;
        AcknowledgedCommand commands[max];

        AcknowledgedCommands() {
        }
        AcknowledgedCommands(const AcknowledgedCommand *cmds, size_t count) {
            memcpy(commands, cmds, count * sizeof(AcknowledgedCommand));
        }
    };
    static_assert(3 == sizeof(AcknowledgedCommand), "AcknowledgedCommand must be 3 bytes long.");

    using SolarPanelTelemetryPacketBase = s1utils::Pack<DownlinkPacketType,
        Timestamp,
        SolarPanelTelemetry,
        SolarPanelTelemetry,
        SolarPanelTelemetry,
        SolarPanelTelemetry,
        SolarPanelTelemetry,
        SolarPanelTelemetry,
        AcknowledgedCommands,
        uint8_t[2],
        DownlinkSignature>;
    class SolarPanelTelemetryPacket : public SolarPanelTelemetryPacketBase {
    private:
        Q_GADGET
        Q_PROPERTY(s1obc::DownlinkPacketType packetType READ packetType CONSTANT)
        Q_PROPERTY(quint32 timestamp READ timestamp WRITE setTimestamp)
        Q_PROPERTY(QVariantList acknowledgedCommands READ acknowledgedCommands_qt WRITE setAcknowledgedCommands_qt)
        Q_PROPERTY(QByteArray signature READ signature_qt WRITE setSignature_qt)
        Q_PROPERTY(s1obc::SolarPanelTelemetry bottom READ bottom WRITE setBottom)
        Q_PROPERTY(s1obc::SolarPanelTelemetry front READ front WRITE setFront)
        Q_PROPERTY(s1obc::SolarPanelTelemetry right READ right WRITE setRight)
        Q_PROPERTY(s1obc::SolarPanelTelemetry left READ left WRITE setLeft)
        Q_PROPERTY(s1obc::SolarPanelTelemetry back READ back WRITE setBack)
        Q_PROPERTY(s1obc::SolarPanelTelemetry top READ top WRITE setTop)

    public:
        DownlinkPacketType packetType() const {
            return get<0>();
        }
        void setPacketType(DownlinkPacketType val) {
            set<0>(val);
        }
        Timestamp timestamp() const {
            return get<1>();
        }
        void setTimestamp(Timestamp val) {
            set<1>(val);
        }

        AcknowledgedCommands acknowledgedCommands() const {
            return get<8>();
        }
        void setAcknowledgedCommands(const AcknowledgedCommands &val) {
            set<8>(val);
        }
        DownlinkSignature signature() const {
            return get<10>();
        }
        void setSignature(const DownlinkSignature &val) {
            set<10>(val);
        }

        SolarPanelTelemetry bottom() const {
            return get<2>();
        }
        void setBottom(const SolarPanelTelemetry &val) {
            set<2>(val);
        }
        SolarPanelTelemetry front() const {
            return get<3>();
        }
        void setFront(const SolarPanelTelemetry &val) {
            set<3>(val);
        }
        SolarPanelTelemetry right() const {
            return get<4>();
        }
        void setRight(const SolarPanelTelemetry &val) {
            set<4>(val);
        }
        SolarPanelTelemetry left() const {
            return get<5>();
        }
        void setLeft(const SolarPanelTelemetry &val) {
            set<5>(val);
        }
        SolarPanelTelemetry back() const {
            return get<6>();
        }
        void setBack(const SolarPanelTelemetry &val) {
            set<6>(val);
        }
        SolarPanelTelemetry top() const {
            return get<7>();
        }
        void setTop(const SolarPanelTelemetry &val) {
            set<7>(val);
        }

        QByteArray signature_qt() const {
            return QByteArray((const char *) signature().bytes, 10);
        }
        void setSignature_qt(const QByteArray &val) {
            DownlinkSignature sig;
            memcpy(sig.bytes, val.data(), 10);
            setSignature(sig);
        }
        QVariantList acknowledgedCommands_qt() const {
            auto ack = acknowledgedCommands();
            QVariantList ls;
            ls.append(QVariant::fromValue(ack.commands[0]));
            ls.append(QVariant::fromValue(ack.commands[1]));
            ls.append(QVariant::fromValue(ack.commands[2]));
            return ls;
        }
        void setAcknowledgedCommands_qt(const QVariantList &val) {
            AcknowledgedCommands ack;
            ack.commands[0] = val[0].value<AcknowledgedCommand>();
            ack.commands[1] = val[1].value<AcknowledgedCommand>();
            ack.commands[2] = val[2].value<AcknowledgedCommand>();
            setAcknowledgedCommands(ack);
        }

        SolarPanelTelemetryPacket() {
            this->setPacketType(DownlinkPacketType_Telemetry1);
        }
    };
    static_assert(128 == sizeof(SolarPanelTelemetryPacket), "SolarPanelTelemetryPacket size MUST be 128 bytes.");

    using PcuTelemetryPacketBase = s1utils::Pack<DownlinkPacketType,
        Timestamp,
        PcuDeploymentTelemetry,
        PcuDeploymentTelemetry,
        PcuSdcTelemetry,
        PcuSdcTelemetry,
        PcuBatteryTelemetry,
        PcuBatteryTelemetry,
        PcuBusTelemetry,
        PcuBusTelemetry,
        AcknowledgedCommands,
        uint8_t[4],
        DownlinkSignature>;
    class PcuTelemetryPacket : public PcuTelemetryPacketBase {
    private:
        Q_GADGET
        Q_PROPERTY(s1obc::DownlinkPacketType packetType READ packetType CONSTANT)
        Q_PROPERTY(quint32 timestamp READ timestamp WRITE setTimestamp)
        Q_PROPERTY(QVariantList acknowledgedCommands READ acknowledgedCommands_qt WRITE setAcknowledgedCommands_qt)
        Q_PROPERTY(QByteArray signature READ signature_qt WRITE setSignature_qt)
        Q_PROPERTY(s1obc::PcuDeploymentTelemetry deployment1 READ deployment1 WRITE setDeployment1)
        Q_PROPERTY(s1obc::PcuDeploymentTelemetry deployment2 READ deployment2 WRITE setDeployment2)
        Q_PROPERTY(s1obc::PcuSdcTelemetry sdc1 READ sdc1 WRITE setSdc1)
        Q_PROPERTY(s1obc::PcuSdcTelemetry sdc2 READ sdc2 WRITE setSdc2)
        Q_PROPERTY(s1obc::PcuBatteryTelemetry battery1 READ battery1 WRITE setBattery1)
        Q_PROPERTY(s1obc::PcuBatteryTelemetry battery2 READ battery2 WRITE setBattery2)
        Q_PROPERTY(s1obc::PcuBusTelemetry bus1 READ bus1 WRITE setBus1)
        Q_PROPERTY(s1obc::PcuBusTelemetry bus2 READ bus2 WRITE setBus2)

    public:
        DownlinkPacketType packetType() const {
            return get<0>();
        }
        void setPacketType(DownlinkPacketType val) {
            set<0>(val);
        }
        Timestamp timestamp() const {
            return get<1>();
        }
        void setTimestamp(Timestamp val) {
            set<1>(val);
        }

        AcknowledgedCommands acknowledgedCommands() const {
            return get<10>();
        }
        void setAcknowledgedCommands(const AcknowledgedCommands &val) {
            set<10>(val);
        }
        DownlinkSignature signature() const {
            return get<12>();
        }
        void setSignature(const DownlinkSignature &val) {
            set<12>(val);
        }

        PcuDeploymentTelemetry deployment1() const {
            return get<2>();
        }
        void setDeployment1(const PcuDeploymentTelemetry &val) {
            set<2>(val);
        }
        PcuDeploymentTelemetry deployment2() const {
            return get<3>();
        }
        void setDeployment2(const PcuDeploymentTelemetry &val) {
            set<3>(val);
        }
        PcuSdcTelemetry sdc1() const {
            return get<4>();
        }
        void setSdc1(const PcuSdcTelemetry &val) {
            set<4>(val);
        }
        PcuSdcTelemetry sdc2() const {
            return get<5>();
        }
        void setSdc2(const PcuSdcTelemetry &val) {
            set<5>(val);
        }
        PcuBatteryTelemetry battery1() const {
            return get<6>();
        }
        void setBattery1(const PcuBatteryTelemetry &val) {
            set<6>(val);
        }
        PcuBatteryTelemetry battery2() const {
            return get<7>();
        }
        void setBattery2(const PcuBatteryTelemetry &val) {
            set<7>(val);
        }
        PcuBusTelemetry bus1() const {
            return get<8>();
        }
        void setBus1(const PcuBusTelemetry &val) {
            set<8>(val);
        }
        PcuBusTelemetry bus2() const {
            return get<9>();
        }
        void setBus2(const PcuBusTelemetry &val) {
            set<9>(val);
        }

        QByteArray signature_qt() const {
            return QByteArray((const char *) signature().bytes, 10);
        }
        void setSignature_qt(const QByteArray &val) {
            DownlinkSignature sig;
            memcpy(sig.bytes, val.data(), 10);
            setSignature(sig);
        }
        QVariantList acknowledgedCommands_qt() const {
            auto ack = acknowledgedCommands();
            QVariantList ls;
            ls.append(QVariant::fromValue(ack.commands[0]));
            ls.append(QVariant::fromValue(ack.commands[1]));
            ls.append(QVariant::fromValue(ack.commands[2]));
            return ls;
        }
        void setAcknowledgedCommands_qt(const QVariantList &val) {
            AcknowledgedCommands ack;
            ack.commands[0] = val[0].value<AcknowledgedCommand>();
            ack.commands[1] = val[1].value<AcknowledgedCommand>();
            ack.commands[2] = val[2].value<AcknowledgedCommand>();
            setAcknowledgedCommands(ack);
        }

        PcuTelemetryPacket() {
            this->setPacketType(DownlinkPacketType_Telemetry2);
        }
    };
    static_assert(128 == sizeof(PcuTelemetryPacket), "PcuTelemetryPacket size MUST be 128 bytes.");

    using OnboardTelemetryPacketBase = s1utils::Pack<DownlinkPacketType,
        Timestamp,
        ObcTelemetry,
        ComTelemetry,
        TidTelemetry,
        TidTelemetry,
        AcknowledgedCommands,
        DownlinkSignature>;
    class OnboardTelemetryPacket : public OnboardTelemetryPacketBase {
    private:
        Q_GADGET
        Q_PROPERTY(s1obc::DownlinkPacketType packetType READ packetType CONSTANT)
        Q_PROPERTY(quint32 timestamp READ timestamp WRITE setTimestamp)
        Q_PROPERTY(QVariantList acknowledgedCommands READ acknowledgedCommands_qt WRITE setAcknowledgedCommands_qt)
        Q_PROPERTY(QByteArray signature READ signature_qt WRITE setSignature_qt)
        Q_PROPERTY(s1obc::ObcTelemetry obc READ obc WRITE setObc)
        Q_PROPERTY(s1obc::ComTelemetry com READ com WRITE setCom)
        Q_PROPERTY(s1obc::TidTelemetry tid1 READ tid1 WRITE setTid1)
        Q_PROPERTY(s1obc::TidTelemetry tid2 READ tid2 WRITE setTid2)

    public:
        DownlinkPacketType packetType() const {
            return get<0>();
        }
        void setPacketType(DownlinkPacketType val) {
            set<0>(val);
        }
        Timestamp timestamp() const {
            return get<1>();
        }
        void setTimestamp(Timestamp val) {
            set<1>(val);
        }

        AcknowledgedCommands acknowledgedCommands() const {
            return get<6>();
        }
        void setAcknowledgedCommands(const AcknowledgedCommands &val) {
            set<6>(val);
        }
        DownlinkSignature signature() const {
            return get<7>();
        }
        void setSignature(const DownlinkSignature &val) {
            set<7>(val);
        }

        ObcTelemetry obc() const {
            return get<2>();
        }
        void setObc(const ObcTelemetry &val) {
            set<2>(val);
        }
        ComTelemetry com() const {
            return get<3>();
        }
        void setCom(const ComTelemetry &val) {
            set<3>(val);
        }
        TidTelemetry tid1() const {
            return get<4>();
        }
        void setTid1(const TidTelemetry &val) {
            set<4>(val);
        }
        TidTelemetry tid2() const {
            return get<5>();
        }
        void setTid2(const TidTelemetry &val) {
            set<5>(val);
        }

        QByteArray signature_qt() const {
            return QByteArray((const char *) signature().bytes, 10);
        }
        void setSignature_qt(const QByteArray &val) {
            DownlinkSignature sig;
            memcpy(sig.bytes, val.data(), 10);
            setSignature(sig);
        }
        QVariantList acknowledgedCommands_qt() const {
            auto ack = acknowledgedCommands();
            QVariantList ls;
            ls.append(QVariant::fromValue(ack.commands[0]));
            ls.append(QVariant::fromValue(ack.commands[1]));
            ls.append(QVariant::fromValue(ack.commands[2]));
            return ls;
        }
        void setAcknowledgedCommands_qt(const QVariantList &val) {
            AcknowledgedCommands ack;
            ack.commands[0] = val[0].value<AcknowledgedCommand>();
            ack.commands[1] = val[1].value<AcknowledgedCommand>();
            ack.commands[2] = val[2].value<AcknowledgedCommand>();
            setAcknowledgedCommands(ack);
        }

        OnboardTelemetryPacket() {
            setPacketType(DownlinkPacketType_Telemetry3);
        }
    };
    static_assert(128 == sizeof(OnboardTelemetryPacket), "S1 Telemetry3 packet size MUST be 128 bytes.");

    using OnboardTelemetryPacketBaseP = s1utils::Pack<DownlinkPacketType,
        Timestamp,
        ObcTelemetryPA,
        ComTelemetry,
        TidTelemetry,
        TidTelemetry,
        AcknowledgedCommands,
        DownlinkSignature>;
    class OnboardTelemetryPacketPA : public OnboardTelemetryPacketBaseP {
    private:
        Q_GADGET
        Q_PROPERTY(s1obc::DownlinkPacketType packetType READ packetType CONSTANT)
        Q_PROPERTY(quint32 timestamp READ timestamp WRITE setTimestamp)
        Q_PROPERTY(QVariantList acknowledgedCommands READ acknowledgedCommands_qt WRITE setAcknowledgedCommands_qt)
        Q_PROPERTY(QByteArray signature READ signature_qt WRITE setSignature_qt)
        Q_PROPERTY(s1obc::ObcTelemetryPA obc READ obc WRITE setObc)
        Q_PROPERTY(s1obc::ComTelemetry com READ com WRITE setCom)
        Q_PROPERTY(s1obc::TidTelemetry tid1 READ tid1 WRITE setTid1)
        Q_PROPERTY(s1obc::TidTelemetry tid2 READ tid2 WRITE setTid2)

    public:
        DownlinkPacketType packetType() const {
            return get<0>();
        }
        void setPacketType(DownlinkPacketType val) {
            set<0>(val);
        }
        Timestamp timestamp() const {
            return get<1>();
        }
        void setTimestamp(Timestamp val) {
            set<1>(val);
        }

        AcknowledgedCommands acknowledgedCommands() const {
            return get<6>();
        }
        void setAcknowledgedCommands(const AcknowledgedCommands &val) {
            set<6>(val);
        }
        DownlinkSignature signature() const {
            return get<7>();
        }
        void setSignature(const DownlinkSignature &val) {
            set<7>(val);
        }

        ObcTelemetryPA obc() const {
            return get<2>();
        }
        void setObc(const ObcTelemetryPA &val) {
            set<2>(val);
        }
        ComTelemetry com() const {
            return get<3>();
        }
        void setCom(const ComTelemetry &val) {
            set<3>(val);
        }
        TidTelemetry tid1() const {
            return get<4>();
        }
        void setTid1(const TidTelemetry &val) {
            set<4>(val);
        }
        TidTelemetry tid2() const {
            return get<5>();
        }
        void setTid2(const TidTelemetry &val) {
            set<5>(val);
        }

        QByteArray signature_qt() const {
            return QByteArray((const char *) signature().bytes, 10);
        }
        void setSignature_qt(const QByteArray &val) {
            DownlinkSignature sig;
            memcpy(sig.bytes, val.data(), 10);
            setSignature(sig);
        }
        QVariantList acknowledgedCommands_qt() const {
            auto ack = acknowledgedCommands();
            QVariantList ls;
            ls.append(QVariant::fromValue(ack.commands[0]));
            ls.append(QVariant::fromValue(ack.commands[1]));
            ls.append(QVariant::fromValue(ack.commands[2]));
            return ls;
        }
        void setAcknowledgedCommands_qt(const QVariantList &val) {
            AcknowledgedCommands ack;
            ack.commands[0] = val[0].value<AcknowledgedCommand>();
            ack.commands[1] = val[1].value<AcknowledgedCommand>();
            ack.commands[2] = val[2].value<AcknowledgedCommand>();
            setAcknowledgedCommands(ack);
        }

        OnboardTelemetryPacketPA() {
            setPacketType(DownlinkPacketType_Telemetry3);
        }
    };
    static_assert(128 == sizeof(OnboardTelemetryPacketPA), "OnboardTelemetryPacketP size MUST be 128 bytes.");

    struct BeaconMessage {
        static constexpr size_t length = 80;
        char characters[length];

        BeaconMessage() = default;
        BeaconMessage(const char *message) {
            strncpy(characters, message, length);
        }
    };

    struct VersionInfo {
        static constexpr size_t length = 7;
        char characters[length];
    };

    using BeaconPacketBase = s1utils::Pack<DownlinkPacketType,
        Timestamp,
        BeaconMessage,
        DiagnosticInfo,
        VersionInfo,
        AcknowledgedCommands,
        DownlinkSignature>;
    class BeaconPacket : public BeaconPacketBase {
    private:
        Q_GADGET
        Q_PROPERTY(s1obc::DownlinkPacketType packetType READ packetType CONSTANT)
        Q_PROPERTY(quint32 timestamp READ timestamp WRITE setTimestamp)
        Q_PROPERTY(QVariantList acknowledgedCommands READ acknowledgedCommands_qt WRITE setAcknowledgedCommands_qt)
        Q_PROPERTY(QByteArray signature READ signature_qt WRITE setSignature_qt)
        Q_PROPERTY(QString message READ message_qt WRITE setMessage_qt)
        Q_PROPERTY(s1obc::DiagnosticInfo diag READ diagnosticInfo WRITE setDiagnosticInfo)
        Q_PROPERTY(QString versionInfo READ versionInfo_qt WRITE setVersionInfo_qt)

    public:
        DownlinkPacketType packetType() const {
            return get<0>();
        }
        void setPacketType(DownlinkPacketType val) {
            set<0>(val);
        }
        Timestamp timestamp() const {
            return get<1>();
        }
        void setTimestamp(Timestamp val) {
            set<1>(val);
        }

        AcknowledgedCommands acknowledgedCommands() const {
            return get<5>();
        }
        void setAcknowledgedCommands(const AcknowledgedCommands &val) {
            set<5>(val);
        }
        DownlinkSignature signature() const {
            return get<6>();
        }
        void setSignature(const DownlinkSignature &val) {
            set<6>(val);
        }

        BeaconMessage message() const {
            return get<2>();
        }
        void setMessage(const BeaconMessage &val) {
            set<2>(val);
        }

        DiagnosticInfo diagnosticInfo() const {
            return get<3>();
        }
        void setDiagnosticInfo(const DiagnosticInfo &val) {
            set<3>(val);
        }

        VersionInfo versionInfo() const {
            return get<4>();
        }
        void setVersionInfo(const VersionInfo &val) {
            set<4>(val);
        }

        QByteArray signature_qt() const {
            return QByteArray((const char *) signature().bytes, 10);
        }
        void setSignature_qt(const QByteArray &val) {
            DownlinkSignature sig;
            memcpy(sig.bytes, val.data(), 10);
            setSignature(sig);
        }
        QString message_qt() const {
            auto msg = message();
            return QString::fromLatin1(
                msg.characters, msg.characters[BeaconMessage::length - 1] == 0 ? (-1) : BeaconMessage::length);
        }
        void setMessage_qt(const QString &val) {
            BeaconMessage msg;
            strncpy(msg.characters, val.toLatin1().data(), BeaconMessage::length);
            setMessage(msg);
        }
        QString versionInfo_qt() const {
            auto ver = versionInfo();
            return QString::fromLatin1(
                ver.characters, ver.characters[VersionInfo::length - 1] == 0 ? (-1) : VersionInfo::length);
        }
        void setVersionInfo_qt(const QString &val) {
            VersionInfo ver;
            strncpy(ver.characters, val.toLatin1().data(), VersionInfo::length);
            setVersionInfo(ver);
        }
        QVariantList acknowledgedCommands_qt() const {
            auto ack = acknowledgedCommands();
            QVariantList ls;
            ls.append(QVariant::fromValue(ack.commands[0]));
            ls.append(QVariant::fromValue(ack.commands[1]));
            ls.append(QVariant::fromValue(ack.commands[2]));
            return ls;
        }
        void setAcknowledgedCommands_qt(const QVariantList &val) {
            AcknowledgedCommands ack;
            ack.commands[0] = val[0].value<AcknowledgedCommand>();
            ack.commands[1] = val[1].value<AcknowledgedCommand>();
            ack.commands[2] = val[2].value<AcknowledgedCommand>();
            setAcknowledgedCommands(ack);
        }

        BeaconPacket() {
            this->setPacketType(DownlinkPacketType_Beacon);
        }
    };
    static_assert(128 == sizeof(BeaconPacket), "BeaconPacket size MUST be 128 bytes.");

    using BeaconPacketBasePA = s1utils::Pack<DownlinkPacketType,
        Timestamp,
        BeaconMessage,
        DiagnosticInfoPA,
        AcknowledgedCommands,
        uint8_t[8],
        DownlinkSignature>;
    class BeaconPacketPA : public BeaconPacketBasePA {
    private:
        Q_GADGET
        Q_PROPERTY(s1obc::DownlinkPacketType packetType READ packetType CONSTANT)
        Q_PROPERTY(quint32 timestamp READ timestamp WRITE setTimestamp)
        Q_PROPERTY(QVariantList acknowledgedCommands READ acknowledgedCommands_qt WRITE setAcknowledgedCommands_qt)
        Q_PROPERTY(QByteArray signature READ signature_qt WRITE setSignature_qt)
        Q_PROPERTY(QString message READ message_qt WRITE setMessage_qt)
        Q_PROPERTY(s1obc::DiagnosticInfoPA diag READ diagnosticInfo WRITE setDiagnosticInfo)

    public:
        DownlinkPacketType packetType() const {
            return get<0>();
        }
        void setPacketType(DownlinkPacketType val) {
            set<0>(val);
        }
        Timestamp timestamp() const {
            return get<1>();
        }
        void setTimestamp(Timestamp val) {
            set<1>(val);
        }

        AcknowledgedCommands acknowledgedCommands() const {
            return get<4>();
        }
        void setAcknowledgedCommands(const AcknowledgedCommands &val) {
            set<4>(val);
        }
        DownlinkSignature signature() const {
            return get<6>();
        }
        void setSignature(const DownlinkSignature &val) {
            set<6>(val);
        }

        BeaconMessage message() const {
            return get<2>();
        }
        void setMessage(const BeaconMessage &val) {
            set<2>(val);
        }
        DiagnosticInfoPA diagnosticInfo() const {
            return get<3>();
        }
        void setDiagnosticInfo(const DiagnosticInfoPA &val) {
            set<3>(val);
        }

        QByteArray signature_qt() const {
            return QByteArray((const char *) signature().bytes, 10);
        }
        void setSignature_qt(const QByteArray &val) {
            DownlinkSignature sig;
            memcpy(sig.bytes, val.data(), 10);
            setSignature(sig);
        }
        QString message_qt() const {
            auto msg = message();
            return QString::fromLatin1(
                msg.characters, msg.characters[BeaconMessage::length - 1] == 0 ? (-1) : BeaconMessage::length);
        }
        void setMessage_qt(const QString &val) {
            BeaconMessage msg;
            strncpy(msg.characters, val.toLatin1().data(), BeaconMessage::length);
            setMessage(msg);
        }
        QVariantList acknowledgedCommands_qt() const {
            auto ack = acknowledgedCommands();
            QVariantList ls;
            ls.append(QVariant::fromValue(ack.commands[0]));
            ls.append(QVariant::fromValue(ack.commands[1]));
            ls.append(QVariant::fromValue(ack.commands[2]));
            return ls;
        }
        void setAcknowledgedCommands_qt(const QVariantList &val) {
            AcknowledgedCommands ack;
            ack.commands[0] = val[0].value<AcknowledgedCommand>();
            ack.commands[1] = val[1].value<AcknowledgedCommand>();
            ack.commands[2] = val[2].value<AcknowledgedCommand>();
            setAcknowledgedCommands(ack);
        }

        BeaconPacketPA() {
            this->setPacketType(DownlinkPacketType_Beacon);
        }
    };
    static_assert(128 == sizeof(BeaconPacketPA), "BeaconPacketPA size MUST be 128 bytes.");

    struct FileDownloadData final {
        constexpr static size_t maxLengthPerPacket = 217;
        uint8_t bytes[maxLengthPerPacket];
    };
    static_assert(FileDownloadData::maxLengthPerPacket == sizeof(FileDownloadData),
        "size of FileDownloadData MUST match its maxLengthPerPacket");

    using FileDownloadPacketBase = s1utils::
        Pack<DownlinkPacketType, Timestamp, uint16_t, uint16_t, FileEntry, FileDownloadData, DownlinkSignature>;
    class FileDownloadPacket : public FileDownloadPacketBase {
    private:
        Q_GADGET
        Q_PROPERTY(s1obc::DownlinkPacketType packetType READ packetType CONSTANT)
        Q_PROPERTY(quint32 timestamp READ timestamp WRITE setTimestamp)
        Q_PROPERTY(QByteArray signature READ signature_qt WRITE setSignature_qt)
        Q_PROPERTY(quint16 index READ index WRITE setIndex)
        Q_PROPERTY(quint16 count READ count WRITE setCount)
        Q_PROPERTY(s1obc::FileEntry entry READ entry WRITE setEntry)
        Q_PROPERTY(QByteArray data READ data_qt WRITE setData_qt)

    public:
        DownlinkPacketType packetType() const {
            return get<0>();
        }
        void setPacketType(DownlinkPacketType val) {
            set<0>(val);
        }
        Timestamp timestamp() const {
            return get<1>();
        }
        void setTimestamp(Timestamp val) {
            set<1>(val);
        }
        DownlinkSignature signature() const {
            return get<6>();
        }
        void setSignature(const DownlinkSignature &val) {
            set<6>(val);
        }

        uint16_t index() const {
            return get<2>();
        }
        void setIndex(uint16_t val) {
            set<2>(val);
        }
        uint16_t count() const {
            return get<3>();
        }
        void setCount(uint16_t val) {
            set<3>(val);
        }
        FileEntry entry() const {
            return get<4>();
        }
        void setEntry(FileEntry val) {
            set<4>(val);
        }
        FileDownloadData data() const {
            return get<5>();
        }
        void setData(FileDownloadData val) {
            set<5>(val);
        }

        QByteArray signature_qt() const {
            return QByteArray((const char *) signature().bytes, 10);
        }
        void setSignature_qt(const QByteArray &val) {
            DownlinkSignature sig;
            memcpy(sig.bytes, val.data(), 10);
            setSignature(sig);
        }
        QByteArray data_qt() const {
            return QByteArray((const char *) data().bytes, FileDownloadData::maxLengthPerPacket);
        }
        void setData_qt(const QByteArray &val) {
            FileDownloadData sp;
            memcpy(sp.bytes, val.data(), FileDownloadData::maxLengthPerPacket);
            setData(sp);
        }

        FileDownloadPacket() {
            this->setPacketType(DownlinkPacketType_FileDownload);
        }
    };
    static_assert(256 == sizeof(FileDownloadPacket), "FileDownloadPacket size MUST be 256 bytes.");

    class HamRepeaterMessage {
    private:
        Q_GADGET
        Q_PROPERTY(QString content READ content WRITE setContent)
        Q_PROPERTY(int rssi READ rssi WRITE setRssi)

    public:
        constexpr static size_t length = 29;

        QString content() const {
            return QString::fromLatin1(contentChars, contentChars[length - 1] == 0 ? (-1) : length);
        }

        void setContent(const QString &content) {
            strncpy(contentChars, content.toLatin1().data(), length);
        }

        int rssi() const {
            return rssiValue;
        }

        void setRssi(int val) {
            rssiValue = static_cast<int8_t>(val);
        }

    private:
        char contentChars[length] = {0};
        int8_t rssiValue;
    };

    struct HamRepeaterMessageList {
        constexpr static size_t length = 8;
        HamRepeaterMessage messages[length];
    };
    static_assert(240 == sizeof(HamRepeaterMessageList), "HamRepeaterMessageList size MUST be 240 bytes.");

    using HamRepeaterPacketBase =
        s1utils::Pack<DownlinkPacketType, uint32_t, HamRepeaterMessageList, uint8_t, DownlinkSignature>;

    class HamRepeaterPacket : public HamRepeaterPacketBase {
    private:
        Q_GADGET
        Q_PROPERTY(s1obc::DownlinkPacketType packetType READ packetType CONSTANT)
        Q_PROPERTY(quint32 timestamp READ timestamp WRITE setTimestamp)
        Q_PROPERTY(int invalidMsgCount READ invalidMsgCount WRITE setInvalidMsgCount)
        Q_PROPERTY(QVariantList messageList READ messageList_qt WRITE setMessageList_qt)
        Q_PROPERTY(QByteArray signature READ signature_qt WRITE setSignature_qt)

    public:
        DownlinkPacketType packetType() const {
            return get<0>();
        }
        void setPacketType(DownlinkPacketType val) {
            set<0>(val);
        }
        Timestamp timestamp() const {
            return get<1>();
        }
        void setTimestamp(Timestamp val) {
            set<1>(val);
        }
        HamRepeaterMessageList messageList() const {
            return get<2>();
        }
        void setMessageList(const HamRepeaterMessageList &lst) {
            set<2>(lst);
        }
        uint8_t invalidMsgCount() const {
            return get<3>();
        }
        void setInvalidMsgCount(uint8_t val) {
            set<3>(val);
        }
        DownlinkSignature signature() const {
            return get<4>();
        }
        void setSignature(const DownlinkSignature &val) {
            set<4>(val);
        }

        QVariantList messageList_qt() const {
            HamRepeaterMessageList lst = messageList();
            QVariantList result;

            for (auto &item : lst.messages) {
                result.append(QVariant::fromValue<HamRepeaterMessage>(item));
            }

            return result;
        }
        void setMessageList_qt(const QVariantList &list) {
            HamRepeaterMessageList lst;

            for (size_t i = 0; i < HamRepeaterMessageList::length && i < (size_t) list.size(); i++) {
                HamRepeaterMessage item = list[(int) i].value<HamRepeaterMessage>();
                lst.messages[i] = item;
            }

            setMessageList(lst);
        }

        QByteArray signature_qt() const {
            return QByteArray((const char *) signature().bytes, 10);
        }
        void setSignature_qt(const QByteArray &val) {
            DownlinkSignature sig;
            memcpy(sig.bytes, val.data(), 10);
            setSignature(sig);
        }
    };
    static_assert(256 == sizeof(HamRepeaterPacket), "HamRepeaterPacket size MUST be 256 bytes.");

    struct SpectrumData {
        static constexpr size_t maxLengthPerPacket = 224;
        uint8_t bytes[maxLengthPerPacket];
    };

    using SpectrumPacketBase = s1utils::Pack<DownlinkPacketType,
        Timestamp,
        Frequency,
        Frequency,
        Rbw,
        uint8_t,
        uint8_t,
        uint16_t,
        uint16_t,
        MeasurementId,
        SpectrumData,
        DownlinkSignature>;
    class SpectrumPacket : public SpectrumPacketBase {
    private:
        Q_GADGET
        Q_PROPERTY(s1obc::DownlinkPacketType packetType READ packetType CONSTANT)
        Q_PROPERTY(quint32 timestamp READ timestamp WRITE setTimestamp)
        Q_PROPERTY(QByteArray signature READ signature_qt WRITE setSignature_qt)
        Q_PROPERTY(quint32 startFreq READ startFreq WRITE setStartFreq)
        Q_PROPERTY(quint32 stepSize READ stepSize WRITE setStepSize)
        Q_PROPERTY(int rbw READ rbw WRITE setRbw)
        Q_PROPERTY(int index READ index WRITE setIndex)
        Q_PROPERTY(int count READ count WRITE setCount)
        Q_PROPERTY(quint16 dataLength READ dataLength WRITE setDataLength)
        Q_PROPERTY(quint16 reqUplinkSerial READ reqUplinkSerial WRITE setReqUplinkSerial)
        Q_PROPERTY(quint16 measurementId READ measurementId WRITE setMeasurementId)
        Q_PROPERTY(QByteArray data READ data_qt WRITE setData_qt)

    public:
        DownlinkPacketType packetType() const {
            return get<0>();
        }
        void setPacketType(DownlinkPacketType val) {
            set<0>(val);
        }
        Timestamp timestamp() const {
            return get<1>();
        }
        void setTimestamp(Timestamp val) {
            set<1>(val);
        }
        DownlinkSignature signature() const {
            return get<11>();
        }
        void setSignature(const DownlinkSignature &val) {
            set<11>(val);
        }

        Frequency startFreq() const {
            return get<2>();
        }
        void setStartFreq(Frequency val) {
            set<2>(val);
        }
        Frequency stepSize() const {
            return get<3>();
        }
        void setStepSize(Frequency val) {
            set<3>(val);
        }
        Rbw rbw() const {
            return get<4>();
        }
        void setRbw(Rbw val) {
            set<4>(val);
        }
        uint8_t index() const {
            return get<5>();
        }
        void setIndex(uint8_t val) {
            set<5>(val);
        }
        uint8_t count() const {
            return get<6>();
        }
        void setCount(uint8_t val) {
            set<6>(val);
        }

        uint16_t dataLength() const {
            return get<7>();
        }
        void setDataLength(uint16_t val) {
            set<7>(val);
        }
        uint16_t reqUplinkSerial() const {
            return get<8>();
        }
        void setReqUplinkSerial(uint16_t val) {
            set<8>(val);
        }
        MeasurementId measurementId() const {
            return get<9>();
        }
        void setMeasurementId(MeasurementId val) {
            set<9>(val);
        }
        SpectrumData data() const {
            return get<10>();
        }
        void setData(SpectrumData val) {
            set<10>(val);
        }

        QByteArray signature_qt() const {
            return QByteArray((const char *) signature().bytes, 10);
        }
        void setSignature_qt(const QByteArray &val) {
            DownlinkSignature sig;
            memcpy(sig.bytes, val.data(), 10);
            setSignature(sig);
        }
        QByteArray data_qt() const {
            return QByteArray((const char *) data().bytes, SpectrumData::maxLengthPerPacket);
        }
        void setData_qt(const QByteArray &val) {
            SpectrumData sp;
            memcpy(sp.bytes, val.data(), SpectrumData::maxLengthPerPacket);
            setData(sp);
        }

        SpectrumPacket() {
            this->setPacketType(DownlinkPacketType_SpectrumResult);
        }
    };
    static_assert(256 == sizeof(SpectrumPacket), "SpectrumPacket size MUST be 256 bytes (max downlink packet size).");

    using BatteryPacketBaseA = s1utils::Pack<DownlinkPacketType,
        Timestamp,
        BatteryTelemetryA,
        BatteryTelemetryA,
        BatteryTelemetryA,
        BatteryTelemetryA,
        uint8_t[33],
        DownlinkSignature>;
    class BatteryPacketA : public BatteryPacketBaseA {
    private:
        Q_GADGET
        Q_PROPERTY(uint8_t packetType READ packetType CONSTANT)
        Q_PROPERTY(quint32 timestamp READ timestamp WRITE setTimestamp)
        Q_PROPERTY(QByteArray signature READ signature_qt WRITE setSignature_qt)
        Q_PROPERTY(s1obc::BatteryTelemetryA panel1 READ panel1)
        Q_PROPERTY(s1obc::BatteryTelemetryA panel2 READ panel2)
        Q_PROPERTY(s1obc::BatteryTelemetryA panel3 READ panel3)
        Q_PROPERTY(s1obc::BatteryTelemetryA panel4 READ panel4)

    public:
        uint8_t packetType() const {
            return static_cast<uint8_t>(get<0>());
        }
        void setPacketType(DownlinkPacketType val) {
            set<0>(val);
        }
        Timestamp timestamp() const {
            return get<1>();
        }
        void setTimestamp(Timestamp val) {
            set<1>(val);
        }
        DownlinkSignature signature() const {
            return get<7>();
        }
        void setSignature(const DownlinkSignature &val) {
            set<7>(val);
        }
        QByteArray signature_qt() const {
            return QByteArray((const char *) signature().bytes, 10);
        }
        void setSignature_qt(const QByteArray &val) {
            DownlinkSignature sig;
            memcpy(sig.bytes, val.data(), 10);
            setSignature(sig);
        }
        s1obc::BatteryTelemetryA panel1() const {
            return get<2>();
        }
        s1obc::BatteryTelemetryA panel2() const {
            return get<3>();
        }
        s1obc::BatteryTelemetryA panel3() const {
            return get<4>();
        }
        s1obc::BatteryTelemetryA panel4() const {
            return get<5>();
        }

        BatteryPacketA() {
            this->setPacketType(DownlinkPacketType_SpectrumResult);
        }
    };
    static_assert(128 == sizeof(BatteryPacketA), "BatteryPacketA size MUST be 128 bytes.");

} // namespace s1obc

Q_DECLARE_METATYPE(s1obc::AcknowledgedCommand)
Q_DECLARE_METATYPE(s1obc::SolarPanelTelemetryPacket)
Q_DECLARE_METATYPE(s1obc::PcuTelemetryPacket)
Q_DECLARE_METATYPE(s1obc::OnboardTelemetryPacket)
Q_DECLARE_METATYPE(s1obc::OnboardTelemetryPacketPA)
Q_DECLARE_METATYPE(s1obc::BeaconPacket)
Q_DECLARE_METATYPE(s1obc::BeaconPacketPA)
Q_DECLARE_METATYPE(s1obc::BatteryPacketA)
Q_DECLARE_METATYPE(s1obc::FileDownloadPacket)
Q_DECLARE_METATYPE(s1obc::HamRepeaterMessage)
Q_DECLARE_METATYPE(s1obc::HamRepeaterPacket)
Q_DECLARE_METATYPE(s1obc::SpectrumPacket)

#endif // S1OBC_DOWNLINK_H
