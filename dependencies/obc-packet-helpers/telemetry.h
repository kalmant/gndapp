
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

#ifndef S1OBC_TELEMETRY_H
#define S1OBC_TELEMETRY_H

#include "bitfield.h"
#include "helper.h"
#include "int24.h"
#include "pack.h"
#include <QObject>
#include <QVariantList>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <ctime>

#define S1_PACKED __attribute__((__packed__))

static_assert(sizeof(uint32_t) == 4, "Size of uint32_t should be 32 bits.");

namespace s1obc {

    Q_NAMESPACE

    using Timestamp = uint32_t;
    using Temperature_C10 = int16_t;
    using Temperature_C = int16_t;
    using Rssi = int16_t;
    using SwrReading = uint8_t;
    using Voltage_mV = uint16_t;
    using Current_mA = uint16_t;
    using LightSensorReading_mV = uint16_t;
    using RadFetReading_uV = s1utils::s1_uint24_t;
    using BitField8 = uint8_t;
    using MeasurementId = uint16_t;
    using Rbw = uint8_t;
    using Frequency = uint32_t;
    using ADCValue = uint16_t;

    enum UplinkPacketType {
        UplinkPacketType_None = 0,
        UplinkPacketType_Ping,
        UplinkPacketType_Toggle,
        UplinkPacketType_Reset,
        UplinkPacketType_SetComParameters,
        UplinkPacketType_StartSpectrumAnalyzer,
        UplinkPacketType_OpenAntenna,
        UplinkPacketType_AntennaOpenSuccess,
        UplinkPacketType_SetBeacon,
        UplinkPacketType_FileDelete,
        UplinkPacketType_FileDownloadRequest,
        UplinkPacketType_MeasurementRequest,
        UplinkPacketType_MorseRequest,
        UplinkPacketType_HamRepeaterMessage,
        UplinkPacketType_SetHamRepeaterMode,
        UplinkPacketType_SilentMode,
    } S1_PACKED;
    Q_ENUM_NS(UplinkPacketType)
    static_assert(1 == sizeof(UplinkPacketType), "enum must be 1 byte long.");

    enum UplinkToggleType {
        UplinkToggleType_None = 0,
        UplinkToggleType_SDC,
        UplinkToggleType_OBC,
        UplinkToggleType_COM,
        UplinkToggleType_Battery,
    } S1_PACKED;
    Q_ENUM_NS(UplinkToggleType)
    static_assert(1 == sizeof(UplinkToggleType), "enum must be 1 byte long.");

    enum UplinkResetType {
        UplinkResetType_None = 0,
        UplinkResetType_PCU,
        UplinkResetType_OBC,
        UplinkResetType_COM,
        UplinkResetType_TID,
        UplinkResetType_OBC_UserData,
        UplinkResetType_ExitSilentMode,
        UplinkResetType_FileSystem,
    } S1_PACKED;
    Q_ENUM_NS(UplinkResetType)
    static_assert(1 == sizeof(UplinkResetType), "enum must be 1 byte long.");

    enum DownlinkPacketType {
        DownlinkPacketType_Telemetry1 = 1,
        DownlinkPacketType_Telemetry2 = 2,
        DownlinkPacketType_Telemetry3 = 3,
        DownlinkPacketType_Beacon = 4,
        DownlinkPacketType_SpectrumResult = 5,
        DownlinkPacketType_FileInfo = 6,
        DownlinkPacketType_FileDownload = 7,
        DownlinkPacketType_HamRepeater1 = 8,
        DownlinkPacketType_HamRepeater2 = 9,
        DownlinkPacketType_Telemetry1_B = 33,
        DownlinkPacketType_Telemetry2_B = 34,
        DownlinkPacketType_Telemetry1_A = 129,
        DownlinkPacketType_Telemetry2_A = 130,
        DownlinkPacketType_Telemetry3_A = 131,
    } S1_PACKED;
    Q_ENUM_NS(DownlinkPacketType)
    static_assert(1 == sizeof(DownlinkPacketType), "enum must be 1 byte long.");

    enum DownlinkEncoding {
        DownlinkEncoding_AO40Short_128_to_333 = 1,
        DownlinkEncoding_AO40_256_to_650,
        DownlinkEncoding_racoder_128_to_256,
        DownlinkEncoding_racoder_256_to_512,
        DownlinkEncoding_racoder_512_to_1024,
        DownlinkEncoding_racoder_1024_to_2048,
        DownlinkEncoding_racoder_2048_to_4096,
        DownlinkEncoding_sync_70,
    } S1_PACKED;
    Q_ENUM_NS(DownlinkEncoding)
    static_assert(1 == sizeof(DownlinkEncoding), "enum must be 1 byte long.");

    enum FileType {

        FileType_Deleted = 0,
        FileType_Text,
        FileType_Custom = 20,
        FileType_Spectrum,
        FileType_UniversalMeasurement,
        FileType_InfoTable = 255,

    } S1_PACKED;
    Q_ENUM_NS(FileType)
    static_assert(1 == sizeof(FileType), "enum must be 1 byte long.");

    struct FileName {
        static constexpr size_t length = 10;
        char characters[length];
    } S1_PACKED;

    using FileEntryBase = s1utils::Pack<uint8_t, uint16_t, s1utils::s1_uint24_t, uint32_t, FileName>;
    class FileEntry : public FileEntryBase {
    private:
        Q_GADGET
        Q_PROPERTY(int type READ type WRITE setType)
        Q_PROPERTY(quint16 pageAddr READ pageAddr WRITE setPageAddr)
        Q_PROPERTY(quint32 size READ size WRITE setSize)
        Q_PROPERTY(quint32 timestamp READ timestamp WRITE setTimestamp)
        Q_PROPERTY(QString name READ name_qt WRITE setName_qt)

    public:
        uint8_t type() const {
            return get<0>();
        }
        void setType(uint8_t val) {
            set<0>(val);
        }
        uint16_t pageAddr() const {
            return get<1>();
        }
        void setPageAddr(uint16_t val) {
            set<1>(val);
        }
        s1utils::s1_uint24_t size() const {
            return get<2>();
        }
        void setSize(s1utils::s1_uint24_t val) {
            set<2>(val);
        }
        uint32_t timestamp() const {
            return get<3>();
        }
        void setTimestamp(uint32_t val) {
            set<3>(val);
        }
        FileName name() const {
            return get<4>();
        }
        void setName(const FileName &val) {
            set<4>(val);
        }

        QString name_qt() const {
            FileName n = name();
            return QString::fromLatin1(n.characters, n.characters[FileName::length - 1] == 0 ? (-1) : FileName::length);
        }
        void setName_qt(const QString &val) {
            FileName n;
            strncpy(n.characters, val.toLatin1().data(), FileName::length);
            setName(n);
        }
    };

    // COM - communication subsystem telemetry ====================================================

    class SpectrumAnalyzerStatus : public s1utils::Bitfield<uint8_t, 7, 1> {
    private:
        Q_GADGET
        Q_PROPERTY(int voltage_30mV READ voltage_30mV WRITE setVoltage_30mV)
        Q_PROPERTY(int works READ works WRITE setWorks)

    public:
        uint8_t voltage_30mV() const {
            return get<0>();
        }
        void setVoltage_30mV(uint8_t val) {
            set<0>(val);
        }
        uint8_t works() const {
            return get<1>();
        }
        void setWorks(uint8_t val) {
            set<1>(val);
        }
    };
    static_assert(1 == sizeof(SpectrumAnalyzerStatus), "COM TX status must be 1 byte long.");

    using ComTelemetryBase = s1utils::
        Pack<Timestamp, SwrReading, uint8_t, SpectrumAnalyzerStatus, Voltage_mV, Temperature_C10, Temperature_C10>;
    class ComTelemetry : public ComTelemetryBase {
    private:
        Q_GADGET
        Q_PROPERTY(quint32 timestamp READ timestamp WRITE setTimestamp)
        Q_PROPERTY(int swrBridgeReading READ swrBridgeReading WRITE setSwrBridgeReading)
        Q_PROPERTY(qint16 lastReceivedRssi READ lastReceivedRssi WRITE setLastReceivedRssi)
        Q_PROPERTY(s1obc::SpectrumAnalyzerStatus spectrumAnalyzerStatus READ spectrumAnalyzerStatus WRITE
                setSpectrumAnalyzerStatus)
        Q_PROPERTY(quint16 activeComVoltage_mV READ activeComVoltage_mV WRITE setTimestamp)
        Q_PROPERTY(qint16 activeComTemperature_C10 READ activeComTemperature_C10 WRITE setActiveComTemperature_C10)
        Q_PROPERTY(qint16 activeComSpectrumAnalyzerTemperature_C10 READ activeComSpectrumAnalyzerTemperature_C10 WRITE
                setActiveComSpectrumAnalyzerTemperature_C10)

    public:
        Timestamp timestamp() const {
            return get<0>();
        }
        void setTimestamp(Timestamp val) {
            set<0>(val);
        }
        SwrReading swrBridgeReading() const {
            return get<1>();
        }
        void setSwrBridgeReading(SwrReading val) {
            set<1>(val);
        }
        Rssi lastReceivedRssi() const {
            return static_cast<Rssi>(get<2>() / 2.0 - 131);
        }
        void setLastReceivedRssi(Rssi val) {
            set<2>(static_cast<uint8_t>((val + 131) * 2));
        }
        SpectrumAnalyzerStatus spectrumAnalyzerStatus() const {
            return get<3>();
        }
        void setSpectrumAnalyzerStatus(const SpectrumAnalyzerStatus &val) {
            set<3>(val);
        }
        Voltage_mV activeComVoltage_mV() const {
            return get<4>();
        }
        void setActiveComVoltage_mV(Voltage_mV val) {
            set<4>(val);
        }
        Temperature_C10 activeComTemperature_C10() const {
            return get<5>();
        }
        void setActiveComTemperature_C10(Temperature_C10 val) {
            set<5>(val);
        }
        Temperature_C10 activeComSpectrumAnalyzerTemperature_C10() const {
            return get<6>();
        }
        void setActiveComSpectrumAnalyzerTemperature_C10(Temperature_C10 val) {
            set<6>(val);
        }
    };
    static_assert(13 == sizeof(ComTelemetry), "COM telemetry must be 13 bytes long.");

    // OBC - On-board computer telemetry ==========================================================

    enum EnergyMode {
        EnergyMode_Normal = 0,
        EnergyMode_Saving,
        EnergyMode_Emergency,
    } S1_PACKED;
    Q_ENUM_NS(EnergyMode)
    static_assert(1 == sizeof(EnergyMode), "energy mode enum must be 1 byte long.");

    enum ComTxDatarate {
        ComTxDatarate_500bps = 0,
        ComTxDatarate_1250bps = 1,
        ComTxDatarate_2500bps = 2,
        ComTxDatarate_5000bps = 3,
        ComTxDatarate_12500bps = 4,
        ComTxDatarate_25000bps = 5,
        ComTxDatarate_50000bps = 6,
        ComTxDatarate_125000bps = 7,
        ComTxDatarate_SyncPacket = ComTxDatarate_1250bps,
    } S1_PACKED;
    Q_ENUM_NS(ComTxDatarate)
    static_assert(1 == sizeof(ComTxDatarate), "COM TX datarate enum must be 1 byte long.");

    enum ComTxPowerLevel {
        ComTxPowerLevel_10mW = 0,
        ComTxPowerLevel_min = ComTxPowerLevel_10mW,
        ComTxPowerLevel_1 = 0x1,
        ComTxPowerLevel_2 = 0x2,
        ComTxPowerLevel_3 = 0x3,
        ComTxPowerLevel_4 = 0x4,
        ComTxPowerLevel_5 = 0x5,
        ComTxPowerLevel_6 = 0x6,
        ComTxPowerLevel_25mW = 0x7,
        ComTxPowerLevel_8 = 0x8,
        ComTxPowerLevel_9 = 0x9,
        ComTxPowerLevel_A = 0xA,
        ComTxPowerLevel_B = 0xB,
        ComTxPowerLevel_C = 0xC,
        ComTxPowerLevel_50mW = 0xD,
        ComTxPowerLevel_E = 0xE,
        ComTxPowerLevel_100mW = 0xF,
        ComTxPowerLevel_max = ComTxPowerLevel_100mW,
    } S1_PACKED;
    Q_ENUM_NS(ComTxPowerLevel)
    static_assert(1 == sizeof(ComTxPowerLevel), "COM TX power level enum must be 1 byte long.");

    class ComTxStatus : public s1utils::Bitfield<uint8_t, 3, 4, 1> {
    private:
        Q_GADGET
        Q_PROPERTY(int dataRate READ dataRate WRITE setDataRate)
        Q_PROPERTY(int powerLevel READ powerLevel WRITE setPowerLevel)

    public:
        ComTxDatarate dataRate() const {
            return (ComTxDatarate)(get<0>());
        }
        void setDataRate(ComTxDatarate val) {
            set<0>((uint8_t) val);
        }
        void setDataRate(int val) {
            set<0>((uint8_t) val);
        }
        ComTxPowerLevel powerLevel() const {
            return (ComTxPowerLevel)(get<1>());
        }
        void setPowerLevel(ComTxPowerLevel val) {
            set<1>((uint8_t) val);
        }
        void setPowerLevel(int val) {
            set<1>((uint8_t) val);
        }
    };
    static_assert(1 == sizeof(ComTxStatus), "COM TX status must be 1 byte long.");

    class ComLimiterStatus : public s1utils::Bitfield<uint8_t, 1, 1, 1, 1, 1, 1, 2> {
    private:
        Q_GADGET
        Q_PROPERTY(int com1LimiterOvercurrent READ com1LimiterOvercurrent WRITE setCom1LimiterOvercurrent)
        Q_PROPERTY(int com2LimiterOvercurrent READ com2LimiterOvercurrent WRITE setCom2LimiterOvercurrent)
        Q_PROPERTY(int com1LimiterSwitch READ com1LimiterSwitch WRITE setCom1LimiterSwitch)
        Q_PROPERTY(int com2LimiterSwitch READ com2LimiterSwitch WRITE setCom2LimiterSwitch)
        Q_PROPERTY(int com1LimiterOverride READ com1LimiterOverride WRITE setCom1LimiterOverride)
        Q_PROPERTY(int com2LimiterOverride READ com2LimiterOverride WRITE setCom2LimiterOverride)

    public:
        uint8_t com1LimiterOvercurrent() const {
            return get<0>();
        }
        void setCom1LimiterOvercurrent(uint8_t val) {
            set<0>(val);
        }
        uint8_t com2LimiterOvercurrent() const {
            return get<1>();
        }
        void setCom2LimiterOvercurrent(uint8_t val) {
            set<1>(val);
        }
        uint8_t com1LimiterSwitch() const {
            return get<2>();
        }
        void setCom1LimiterSwitch(uint8_t val) {
            set<2>(val);
        }
        uint8_t com2LimiterSwitch() const {
            return get<3>();
        }
        void setCom2LimiterSwitch(uint8_t val) {
            set<3>(val);
        }
        uint8_t com1LimiterOverride() const {
            return get<4>();
        }
        void setCom1LimiterOverride(uint8_t val) {
            set<4>(val);
        }
        uint8_t com2LimiterOverride() const {
            return get<5>();
        }
        void setCom2LimiterOverride(uint8_t val) {
            set<5>(val);
        }
    };
    static_assert(1 == sizeof(ComLimiterStatus), "COM limiter status must be 1 byte long.");

    using ObcMotionSensorReadingBase = s1utils::Pack<int16_t, int16_t, int16_t>;
    class ObcMotionSensorReading : public ObcMotionSensorReadingBase {
    private:
        Q_GADGET
        Q_PROPERTY(qint16 x READ x WRITE setX)
        Q_PROPERTY(qint16 y READ y WRITE setY)
        Q_PROPERTY(qint16 z READ z WRITE setZ)

    public:
        int16_t x() const {
            return get<0>();
        }
        void setX(int16_t val) {
            set<0>(val);
        }
        int16_t y() const {
            return get<1>();
        }
        void setY(int16_t val) {
            set<1>(val);
        }
        int16_t z() const {
            return get<2>();
        }
        void setZ(int16_t val) {
            set<2>(val);
        }
    };
    static_assert(6 == sizeof(ObcMotionSensorReading), "OBC motion sensor reading must be 6 byte long.");

    class ObcMotionSensorTelemetry
    : public s1utils::Pack<ObcMotionSensorReading, ObcMotionSensorReading, ObcMotionSensorReading, Temperature_C10> {
    private:
        Q_GADGET
        Q_PROPERTY(s1obc::ObcMotionSensorReading gyroscope READ gyroscope WRITE setGyroscope)
        Q_PROPERTY(s1obc::ObcMotionSensorReading magnetometer READ magnetometer WRITE setMagnetometer)
        Q_PROPERTY(s1obc::ObcMotionSensorReading accelerometer READ accelerometer WRITE setAccelerometer)
        Q_PROPERTY(qint16 temperature_C10 READ temperature_C10 WRITE setTemperature_C10)

    public:
        ObcMotionSensorReading gyroscope() const {
            return get<0>();
        }
        void setGyroscope(const ObcMotionSensorReading &val) {
            set<0>(val);
        }
        ObcMotionSensorReading magnetometer() const {
            return get<1>();
        }
        void setMagnetometer(const ObcMotionSensorReading &val) {
            set<1>(val);
        }
        ObcMotionSensorReading accelerometer() const {
            return get<2>();
        }
        void setAccelerometer(const ObcMotionSensorReading &val) {
            set<2>(val);
        }
        Temperature_C10 temperature_C10() const {
            return get<3>();
        }
        void setTemperature_C10(Temperature_C10 val) {
            set<3>(val);
        }
    };
    static_assert(20 == sizeof(ObcMotionSensorTelemetry), "OBC motion sensor telemetry must be 20 byte long.");

    class ObcBoardStatus : public s1utils::Bitfield<uint8_t, 1, 1, 1, 1, 1, 1, 1, 1> {
    private:
        Q_GADGET
        Q_PROPERTY(int motionSensor1Status READ motionSensor1Status WRITE setMotionSensor1Status)
        Q_PROPERTY(int motionSensor2Status READ motionSensor2Status WRITE setMotionSensor2Status)
        Q_PROPERTY(int flash1Status READ flash1Status WRITE setFlash1Status)
        Q_PROPERTY(int flash2Status READ flash2Status WRITE setFlash2Status)
        Q_PROPERTY(int rtcc1Status READ rtcc1Status WRITE setRtcc1Status)
        Q_PROPERTY(int rtcc2Status READ rtcc2Status WRITE setRtcc2Status)
        Q_PROPERTY(int activeObc READ activeObc WRITE setActiveObc)
        Q_PROPERTY(int activeCom READ activeCom WRITE setActiveCom)

    public:
        uint8_t motionSensor1Status() const {
            return get<0>();
        }
        void setMotionSensor1Status(uint8_t val) {
            set<0>(val);
        }
        uint8_t motionSensor2Status() const {
            return get<1>();
        }
        void setMotionSensor2Status(uint8_t val) {
            set<1>(val);
        }
        uint8_t flash1Status() const {
            return get<2>();
        }
        void setFlash1Status(uint8_t val) {
            set<2>(val);
        }
        uint8_t flash2Status() const {
            return get<3>();
        }
        void setFlash2Status(uint8_t val) {
            set<3>(val);
        }
        uint8_t rtcc1Status() const {
            return get<4>();
        }
        void setRtcc1Status(uint8_t val) {
            set<4>(val);
        }
        uint8_t rtcc2Status() const {
            return get<5>();
        }
        void setRtcc2Status(uint8_t val) {
            set<5>(val);
        }
        uint8_t activeObc() const {
            return get<6>();
        }
        void setActiveObc(uint8_t val) {
            set<6>(val);
        }
        uint8_t activeCom() const {
            return get<7>();
        }
        void setActiveCom(uint8_t val) {
            set<7>(val);
        }
    };
    static_assert(1 == sizeof(ObcBoardStatus), "OBC board status must be 1 byte long.");

    using ObcTelemetryBase = s1utils::Pack<Voltage_mV,
        Temperature_C10,
        Temperature_C10,
        Temperature_C10,
        Temperature_C10,
        Temperature_C10,
        ComTxStatus,
        Current_mA,
        Current_mA,
        ComLimiterStatus,
        ObcMotionSensorTelemetry,
        ObcMotionSensorTelemetry,
        ObcBoardStatus>;

    class ObcTelemetry : public ObcTelemetryBase {
    private:
        Q_GADGET
        Q_PROPERTY(quint16 supplyVoltage_mV READ supplyVoltage_mV WRITE setSupplyVoltage_mV)
        Q_PROPERTY(qint16 rtcc1Temperature_C10 READ rtcc1Temperature_C10 WRITE setRtcc1Temperature_C10)
        Q_PROPERTY(qint16 rtcc2Temperature_C10 READ rtcc2Temperature_C10 WRITE setRtcc2Temperature_C10)
        Q_PROPERTY(qint16 activeObcTemperature_C10 READ activeObcTemperature_C10 WRITE setActiveObcTemperature_C10)
        Q_PROPERTY(
            qint16 eps2PanelATemperature1_C10 READ eps2PanelATemperature1_C10 WRITE setEps2PanelATemperature1_C10)
        Q_PROPERTY(
            qint16 eps2PanelATemperature2_C10 READ eps2PanelATemperature2_C10 WRITE setEps2PanelATemperature2_C10)
        Q_PROPERTY(s1obc::ComTxStatus comTxStatus READ comTxStatus WRITE setComTxStatus)
        Q_PROPERTY(quint16 comTxCurrent_mA READ comTxCurrent_mA WRITE setComTxCurrent_mA)
        Q_PROPERTY(quint16 comRxCurrent_mA READ comRxCurrent_mA WRITE setComRxCurrent_mA)
        Q_PROPERTY(s1obc::ComLimiterStatus comLimiterStatus READ comLimiterStatus WRITE setComLimiterStatus)
        Q_PROPERTY(s1obc::ObcMotionSensorTelemetry motionSensor1 READ motionSensor1 WRITE setMotionSensor1)
        Q_PROPERTY(s1obc::ObcMotionSensorTelemetry motionSensor2 READ motionSensor2 WRITE setMotionSensor2)
        Q_PROPERTY(s1obc::ObcBoardStatus obcBoardStatus READ obcBoardStatus WRITE setObcBoardStatus)

    public:
        Voltage_mV supplyVoltage_mV() const {
            return get<0>();
        }
        void setSupplyVoltage_mV(Voltage_mV val) {
            set<0>(val);
        }
        Temperature_C10 rtcc1Temperature_C10() const {
            return get<1>();
        }
        void setRtcc1Temperature_C10(Temperature_C10 val) {
            set<1>(val);
        }
        Temperature_C10 rtcc2Temperature_C10() const {
            return get<2>();
        }
        void setRtcc2Temperature_C10(Temperature_C10 val) {
            set<2>(val);
        }
        Temperature_C10 activeObcTemperature_C10() const {
            return get<3>();
        }
        void setActiveObcTemperature_C10(Temperature_C10 val) {
            set<3>(val);
        }
        Temperature_C10 eps2PanelATemperature1_C10() const {
            return get<4>();
        }
        void setEps2PanelATemperature1_C10(Temperature_C10 val) {
            set<4>(val);
        }
        Temperature_C10 eps2PanelATemperature2_C10() const {
            return get<5>();
        }
        void setEps2PanelATemperature2_C10(Temperature_C10 val) {
            set<5>(val);
        }
        ComTxStatus comTxStatus() const {
            return get<6>();
        }
        void setComTxStatus(const ComTxStatus &val) {
            set<6>(val);
        }
        Current_mA comTxCurrent_mA() const {
            return get<7>();
        }
        void setComTxCurrent_mA(Current_mA val) {
            set<7>(val);
        }
        Current_mA comRxCurrent_mA() const {
            return get<8>();
        }
        void setComRxCurrent_mA(Current_mA val) {
            set<8>(val);
        }
        ComLimiterStatus comLimiterStatus() const {
            return get<9>();
        }
        void setComLimiterStatus(const ComLimiterStatus &val) {
            set<9>(val);
        }
        ObcMotionSensorTelemetry motionSensor1() const {
            return get<10>();
        }
        void setMotionSensor1(const ObcMotionSensorTelemetry &val) {
            set<10>(val);
        }
        ObcMotionSensorTelemetry motionSensor2() const {
            return get<11>();
        }
        void setMotionSensor2(const ObcMotionSensorTelemetry &val) {
            set<11>(val);
        }
        ObcBoardStatus obcBoardStatus() const {
            return get<12>();
        }
        void setObcBoardStatus(const ObcBoardStatus &val) {
            set<12>(val);
        }
    };
    static_assert(59 == sizeof(ObcTelemetry), "OBC telemetry must be 59 byte long.");

    class ObcBoardStatusPA : public s1utils::Bitfield<uint8_t, 1, 1, 1, 1, 1, 1, 1, 1> {
    private:
        Q_GADGET
        Q_PROPERTY(int motionSensor1Status READ motionSensor1Status WRITE setMotionSensor1Status)
        Q_PROPERTY(int motionSensor2Status READ motionSensor2Status WRITE setMotionSensor2Status)
        Q_PROPERTY(int motionSensorStatus READ motionSensorStatus)
        Q_PROPERTY(int flash1Status READ flash1Status WRITE setFlash1Status)
        Q_PROPERTY(int flash2Status READ flash2Status WRITE setFlash2Status)
        Q_PROPERTY(int flashStatus READ flashStatus)
        Q_PROPERTY(int rtcc1Status READ rtcc1Status WRITE setRtcc1Status)
        Q_PROPERTY(int rtcc2Status READ rtcc2Status WRITE setRtcc2Status)
        Q_PROPERTY(int rtccStatus READ rtccStatus)
        Q_PROPERTY(int activeObc READ activeObc WRITE setActiveObc)
        Q_PROPERTY(int activeCom READ activeCom WRITE setActiveCom)

    public:
        uint8_t motionSensor1Status() const {
            return get<0>();
        }
        void setMotionSensor1Status(uint8_t val) {
            set<0>(val);
        }
        uint8_t motionSensor2Status() const {
            return get<1>();
        }
        void setMotionSensor2Status(uint8_t val) {
            set<1>(val);
        }
        uint8_t motionSensorStatus() const {
            return activeObc() == 0 ? motionSensor1Status() : motionSensor2Status();
        }
        uint8_t flash1Status() const {
            return get<2>();
        }
        void setFlash1Status(uint8_t val) {
            set<2>(val);
        }
        uint8_t flash2Status() const {
            return get<3>();
        }
        void setFlash2Status(uint8_t val) {
            set<3>(val);
        }
        uint8_t flashStatus() const {
            return activeObc() == 0 ? flash1Status() : flash2Status();
        }
        uint8_t rtcc1Status() const {
            return get<4>();
        }
        void setRtcc1Status(uint8_t val) {
            set<4>(val);
        }
        uint8_t rtcc2Status() const {
            return get<5>();
        }
        void setRtcc2Status(uint8_t val) {
            set<5>(val);
        }
        uint8_t rtccStatus() const {
            return activeObc() == 0 ? rtcc1Status() : rtcc2Status();
        }
        uint8_t activeObc() const {
            return get<6>();
        }
        void setActiveObc(uint8_t val) {
            set<6>(val);
        }
        uint8_t activeCom() const {
            return get<7>();
        }
        void setActiveCom(uint8_t val) {
            set<7>(val);
        }
    };
    static_assert(1 == sizeof(ObcBoardStatusPA), "ObcBoardStatusPA must be 1 byte long.");
    using ObcTelemetryBasePA = s1utils::Pack<Voltage_mV,
        Temperature_C,
        Temperature_C,
        Temperature_C10,
        Temperature_C10,
        Temperature_C10,
        ComTxStatus,
        Current_mA,
        Current_mA,
        ComLimiterStatus,
        ObcMotionSensorTelemetry,
        ObcMotionSensorTelemetry,
        ObcBoardStatusPA>;
    class ObcTelemetryPA : public ObcTelemetryBasePA {
    private:
        Q_GADGET
        Q_PROPERTY(quint16 supplyVoltage_mV READ supplyVoltage_mV WRITE setSupplyVoltage_mV)
        Q_PROPERTY(qint16 rtcc1Temperature_C READ rtcc1Temperature_C WRITE setRtcc1Temperature_C)
        Q_PROPERTY(qint16 rtcc2Temperature_C READ rtcc2Temperature_C WRITE setRtcc2Temperature_C)
        Q_PROPERTY(qint16 rtccTemperature_C READ rtccTemperature_C)
        Q_PROPERTY(qint16 activeObcTemperature_C10 READ activeObcTemperature_C10 WRITE setActiveObcTemperature_C10)
        Q_PROPERTY(
            qint16 eps2PanelATemperature1_C10 READ eps2PanelATemperature1_C10 WRITE setEps2PanelATemperature1_C10)
        Q_PROPERTY(
            qint16 eps2PanelATemperature2_C10 READ eps2PanelATemperature2_C10 WRITE setEps2PanelATemperature2_C10)
        Q_PROPERTY(s1obc::ComTxStatus comTxStatus READ comTxStatus WRITE setComTxStatus)
        Q_PROPERTY(quint16 comTxCurrent_mA READ comTxCurrent_mA WRITE setComTxCurrent_mA)
        Q_PROPERTY(quint16 comRxCurrent_mA READ comRxCurrent_mA WRITE setComRxCurrent_mA)
        Q_PROPERTY(s1obc::ComLimiterStatus comLimiterStatus READ comLimiterStatus WRITE setComLimiterStatus)
        Q_PROPERTY(s1obc::ObcMotionSensorTelemetry motionSensor1 READ motionSensor1 WRITE setMotionSensor1)
        Q_PROPERTY(s1obc::ObcMotionSensorTelemetry motionSensor2 READ motionSensor2 WRITE setMotionSensor2)
        Q_PROPERTY(s1obc::ObcMotionSensorTelemetry motionSensor READ motionSensor)
        Q_PROPERTY(s1obc::ObcBoardStatusPA obcBoardStatus READ obcBoardStatus WRITE setObcBoardStatus)

    public:
        Voltage_mV supplyVoltage_mV() const {
            return get<0>();
        }
        void setSupplyVoltage_mV(Voltage_mV val) {
            set<0>(val);
        }
        Temperature_C rtcc1Temperature_C() const {
            return get<1>();
        }
        void setRtcc1Temperature_C(Temperature_C val) {
            set<1>(val);
        }
        Temperature_C rtcc2Temperature_C() const {
            return get<2>();
        }
        void setRtcc2Temperature_C(Temperature_C val) {
            set<2>(val);
        }
        Temperature_C rtccTemperature_C() const {
            return obcBoardStatus().activeObc() == 0 ? rtcc1Temperature_C() : rtcc2Temperature_C();
        }
        Temperature_C10 activeObcTemperature_C10() const {
            return get<3>();
        }
        void setActiveObcTemperature_C10(Temperature_C10 val) {
            set<3>(val);
        }
        Temperature_C10 eps2PanelATemperature1_C10() const {
            return get<4>();
        }
        void setEps2PanelATemperature1_C10(Temperature_C10 val) {
            set<4>(val);
        }
        Temperature_C10 eps2PanelATemperature2_C10() const {
            return get<5>();
        }
        void setEps2PanelATemperature2_C10(Temperature_C10 val) {
            set<5>(val);
        }
        ComTxStatus comTxStatus() const {
            return get<6>();
        }
        void setComTxStatus(const ComTxStatus &val) {
            set<6>(val);
        }
        Current_mA comTxCurrent_mA() const {
            return get<7>();
        }
        void setComTxCurrent_mA(Current_mA val) {
            set<7>(val);
        }
        Current_mA comRxCurrent_mA() const {
            return get<8>();
        }
        void setComRxCurrent_mA(Current_mA val) {
            set<8>(val);
        }
        ComLimiterStatus comLimiterStatus() const {
            return get<9>();
        }
        void setComLimiterStatus(const ComLimiterStatus &val) {
            set<9>(val);
        }
        ObcMotionSensorTelemetry motionSensor1() const {
            return get<10>();
        }
        void setMotionSensor1(const ObcMotionSensorTelemetry &val) {
            set<10>(val);
        }
        ObcMotionSensorTelemetry motionSensor2() const {
            return get<11>();
        }
        void setMotionSensor2(const ObcMotionSensorTelemetry &val) {
            set<11>(val);
        }
        ObcMotionSensorTelemetry motionSensor() const {
            return obcBoardStatus().activeObc() == 0 ? motionSensor1() : motionSensor2();
        }
        ObcBoardStatusPA obcBoardStatus() const {
            return get<12>();
        }
        void setObcBoardStatus(const ObcBoardStatusPA &val) {
            set<12>(val);
        }
    };
    static_assert(59 == sizeof(ObcTelemetryPA), "ObcTelemetryPA must be 59 byte long.");

    // Solar panel ================================================================================

    enum SolarPanelAntennaStatus {
        SolarPanelAntennaStatus_NotDeployed = '0',
        SolarPanelAntennaStatus_Deployed = '1',
        SolarPanelAntennaStatus_NotMonitored = 'X',
    } S1_PACKED;
    Q_ENUM_NS(SolarPanelAntennaStatus)
    static_assert(1 == sizeof(SolarPanelAntennaStatus), "Size of SolarPanelAntennaStatus MUST be 1 byte.");

    class SolarPanelStatus : public s1utils::Bitfield<uint8_t, 3, 2, 3> {
    private:
        Q_GADGET
        Q_PROPERTY(int solarPanelId READ solarPanelId WRITE setSolarPanelId)
        Q_PROPERTY(QString antennaStatus READ antennaStatus_qt WRITE setAntennaStatus_qt)

    public:
        uint8_t solarPanelId() const {
            return get<0>();
        }
        void setSolarPanelId(uint8_t val) {
            set<0>(val);
        }

        SolarPanelAntennaStatus antennaStatus() const {
            switch (get<1>()) {
            case 0:
                return SolarPanelAntennaStatus_NotDeployed;
            case 1:
                return SolarPanelAntennaStatus_Deployed;
            default:
                return SolarPanelAntennaStatus_NotMonitored;
            }
        }

        void setAntennaStatus(SolarPanelAntennaStatus s) {
            switch (s) {
            case SolarPanelAntennaStatus_NotDeployed:
                return set<1>(0);
            case SolarPanelAntennaStatus_Deployed:
                return set<1>(1);
            case SolarPanelAntennaStatus_NotMonitored:
                return set<1>(3);
            }
        }

        QString antennaStatus_qt() const {
            return QString((char) antennaStatus());
        }

        void setAntennaStatus_qt(const QString &val) {
            setAntennaStatus((SolarPanelAntennaStatus) val[0].unicode());
        }
    };

    static_assert(1 == sizeof(SolarPanelStatus), "Solar panel status must be 1 byte long.");

    using SolarPanelTelemetryBase = s1utils::Pack<Timestamp,
        Temperature_C10,
        LightSensorReading_mV,
        Current_mA,
        Voltage_mV,
        Current_mA,
        Voltage_mV,
        SolarPanelStatus>;

    class SolarPanelTelemetry : public SolarPanelTelemetryBase {
    private:
        Q_GADGET
        Q_PROPERTY(quint32 timestamp READ timestamp WRITE setTimestamp)
        Q_PROPERTY(qint16 temperature_C10 READ temperature_C10 WRITE setTemperature_C10)
        Q_PROPERTY(quint16 lightSensorReading_mV READ lightSensorReading_mV WRITE setLightSensorReading_mV)
        Q_PROPERTY(quint16 inputCurrent_mA READ inputCurrent_mA WRITE setInputCurrent_mA)
        Q_PROPERTY(quint16 inputVoltage_mV READ inputVoltage_mV WRITE setInputVoltage_mV)
        Q_PROPERTY(quint16 outputCurrent_mA READ outputCurrent_mA WRITE setOutputCurrent_mA)
        Q_PROPERTY(quint16 outputVoltage_mV READ outputVoltage_mV WRITE setOutputVoltage_mV)
        Q_PROPERTY(s1obc::SolarPanelStatus status READ status WRITE setStatus)

    public:
        Timestamp timestamp() const {
            return get<0>();
        }
        void setTimestamp(Timestamp val) {
            set<0>(val);
        }
        Temperature_C10 temperature_C10() const {
            return get<1>();
        }
        void setTemperature_C10(Temperature_C10 val) {
            set<1>(val);
        }
        LightSensorReading_mV lightSensorReading_mV() const {
            return get<2>();
        }
        void setLightSensorReading_mV(LightSensorReading_mV val) {
            set<2>(val);
        }
        Current_mA inputCurrent_mA() const {
            return get<3>();
        }
        void setInputCurrent_mA(Current_mA val) {
            set<3>(val);
        }
        Voltage_mV inputVoltage_mV() const {
            return get<4>();
        }
        void setInputVoltage_mV(Voltage_mV val) {
            set<4>(val);
        }
        Current_mA outputCurrent_mA() const {
            return get<5>();
        }
        void setOutputCurrent_mA(Current_mA val) {
            set<5>(val);
        }
        Voltage_mV outputVoltage_mV() const {
            return get<6>();
        }
        void setOutputVoltage_mV(Voltage_mV val) {
            set<6>(val);
        }
        SolarPanelStatus status() const {
            return get<7>();
        }
        void setStatus(const SolarPanelStatus &val) {
            set<7>(val);
        }
    };
    static_assert(17 == sizeof(SolarPanelTelemetry), "Solar panel telemetry must be 17 byte long.");

    // TID - total ionizing dose sensor ===========================================================

    using TidTelemetryBase =
        s1utils::Pack<Timestamp, Temperature_C10, Voltage_mV, RadFetReading_uV, RadFetReading_uV, uint16_t>;
    class TidTelemetry : public TidTelemetryBase {
    private:
        Q_GADGET
        Q_PROPERTY(quint32 timestamp READ timestamp WRITE setTimestamp)
        Q_PROPERTY(qint16 temperature_C10 READ temperature_C10 WRITE setTemperature_C10)
        Q_PROPERTY(quint16 supplyVoltage_mV READ supplyVoltage_mV WRITE setSupplyVoltage_mV)
        Q_PROPERTY(quint32 radfet1_uV READ radfet1_uV WRITE setRadfet1_uV)
        Q_PROPERTY(quint32 radfet2_uV READ radfet2_uV WRITE setRadfet2_uV)
        Q_PROPERTY(quint16 measurementSerial READ measurementSerial WRITE setMeasurementSerial)

    public:
        Timestamp timestamp() const {
            return get<0>();
        }
        void setTimestamp(Timestamp val) {
            set<0>(val);
        }
        Temperature_C10 temperature_C10() const {
            return get<1>();
        }
        void setTemperature_C10(Temperature_C10 val) {
            set<1>(val);
        }
        Voltage_mV supplyVoltage_mV() const {
            return get<2>();
        }
        void setSupplyVoltage_mV(Voltage_mV val) {
            set<2>(val);
        }
        RadFetReading_uV radfet1_uV() const {
            return get<3>();
        }
        void setRadfet1_uV(RadFetReading_uV val) {
            set<3>(val);
        }
        RadFetReading_uV radfet2_uV() const {
            return get<4>();
        }
        void setRadfet2_uV(RadFetReading_uV val) {
            set<4>(val);
        }
        uint16_t measurementSerial() const {
            return get<5>();
        }
        void setMeasurementSerial(uint16_t val) {
            set<5>(val);
        }
    };
    static_assert(16 == sizeof(TidTelemetry), "TID panel telemetry must be 16 byte long.");

    // PCU - power control unit ===================================================================

    class PcuDeploymentStatus : public s1utils::Bitfield<uint8_t, 1, 1, 1, 1, 1, 1, 2> {
    private:
        Q_GADGET
        Q_PROPERTY(int switch1 READ switch1 WRITE setSwitch1)
        Q_PROPERTY(int switch2 READ switch2 WRITE setSwitch2)
        Q_PROPERTY(int removeBeforeFlight READ removeBeforeFlight WRITE setRemoveBeforeFlight)
        Q_PROPERTY(int pcuDeploymentFlag READ pcuDeploymentFlag WRITE setPcuDeploymentFlag)
        Q_PROPERTY(int pcuAntennaFlag READ pcuAntennaFlag WRITE setPcuAntennaFlag)

    public:
        uint8_t switch1() const {
            return get<0>();
        }
        void setSwitch1(uint8_t val) {
            set<0>(val);
        }
        uint8_t switch2() const {
            return get<1>();
        }
        void setSwitch2(uint8_t val) {
            set<1>(val);
        }
        uint8_t removeBeforeFlight() const {
            return get<2>();
        }
        void setRemoveBeforeFlight(uint8_t val) {
            set<2>(val);
        }
        uint8_t pcuDeploymentFlag() const {
            return get<4>();
        }
        void setPcuDeploymentFlag(uint8_t val) {
            set<4>(val);
        }
        uint8_t pcuAntennaFlag() const {
            return get<5>();
        }
        void setPcuAntennaFlag(uint8_t val) {
            set<5>(val);
        }
    };
    static_assert(1 == sizeof(PcuDeploymentStatus), "PCU deployment status must be 1 byte long.");

    using PcuDeploymentTelemetryBase = s1utils::Pack<Timestamp, PcuDeploymentStatus, uint16_t, uint16_t>;
    class PcuDeploymentTelemetry : public PcuDeploymentTelemetryBase {
    private:
        Q_GADGET
        Q_PROPERTY(quint32 timestamp READ timestamp WRITE setTimestamp)
        Q_PROPERTY(s1obc::PcuDeploymentStatus status READ status WRITE setStatus)
        Q_PROPERTY(quint16 pcuBootCounter READ pcuBootCounter WRITE setPcuBootCounter)
        Q_PROPERTY(quint16 pcuUptimeMinutes READ pcuUptimeMinutes WRITE setPcuUptimeMinutes)

    public:
        Timestamp timestamp() const {
            return get<0>();
        }
        void setTimestamp(Timestamp val) {
            set<0>(val);
        }
        PcuDeploymentStatus status() const {
            return get<1>();
        }
        void setStatus(PcuDeploymentStatus val) {
            set<1>(val);
        }
        uint16_t pcuBootCounter() const {
            return get<2>();
        }
        void setPcuBootCounter(uint16_t val) {
            set<2>(val);
        }
        uint16_t pcuUptimeMinutes() const {
            return get<3>();
        }
        void setPcuUptimeMinutes(uint16_t val) {
            set<3>(val);
        }
    };
    static_assert(9 == sizeof(PcuDeploymentTelemetry), "PCU deployment telemetry must be 9 bytes long.");

    class PcuSdcChainStatus : public s1utils::Bitfield<uint8_t, 1, 1, 1, 1, 4> {
    private:
        Q_GADGET
        Q_PROPERTY(int chain1Overcurrent READ chain1Overcurrent WRITE setChain1Overcurrent)
        Q_PROPERTY(int chain1Overvoltage READ chain1Overvoltage WRITE setChain1Overvoltage)
        Q_PROPERTY(int chain2Overcurrent READ chain2Overcurrent WRITE setChain2Overcurrent)
        Q_PROPERTY(int chain2Overvoltage READ chain2Overvoltage WRITE setChain2Overvoltage)

    public:
        uint8_t chain1Overcurrent() const {
            return get<0>();
        }
        void setChain1Overcurrent(uint8_t val) {
            set<0>(val);
        }
        uint8_t chain1Overvoltage() const {
            return get<1>();
        }
        void setChain1Overvoltage(uint8_t val) {
            set<1>(val);
        }
        uint8_t chain2Overcurrent() const {
            return get<2>();
        }
        void setChain2Overcurrent(uint8_t val) {
            set<2>(val);
        }
        uint8_t chain2Overvoltage() const {
            return get<3>();
        }
        void setChain2Overvoltage(uint8_t val) {
            set<3>(val);
        }
    };
    static_assert(1 == sizeof(PcuSdcChainStatus), "PCU SDC chain status must be 1 byte long.");

    using PcuSdcTelemetryBase = s1utils::
        Pack<Timestamp, Current_mA, Current_mA, Voltage_mV, Current_mA, Current_mA, Voltage_mV, PcuSdcChainStatus>;
    class PcuSdcTelemetry : public PcuSdcTelemetryBase {
    private:
        Q_GADGET
        Q_PROPERTY(quint32 timestamp READ timestamp WRITE setTimestamp)
        Q_PROPERTY(quint16 chain1InputCurrent_mA READ chain1InputCurrent_mA WRITE setChain1InputCurrent_mA)
        Q_PROPERTY(quint16 chain1OutputCurrent_mA READ chain1OutputCurrent_mA WRITE setChain1OutputCurrent_mA)
        Q_PROPERTY(quint16 chain1OutputVoltage_mV READ chain1OutputVoltage_mV WRITE setChain1OutputVoltage_mV)
        Q_PROPERTY(quint16 chain2InputCurrent_mA READ chain2InputCurrent_mA WRITE setChain2InputCurrent_mA)
        Q_PROPERTY(quint16 chain2OutputCurrent_mA READ chain2OutputCurrent_mA WRITE setChain2OutputCurrent_mA)
        Q_PROPERTY(quint16 chain2OutputVoltage_mV READ chain2OutputVoltage_mV WRITE setChain2OutputVoltage_mV)
        Q_PROPERTY(s1obc::PcuSdcChainStatus status READ status WRITE setStatus)

    public:
        Timestamp timestamp() const {
            return get<0>();
        }
        void setTimestamp(Timestamp val) {
            set<0>(val);
        }
        Current_mA chain1InputCurrent_mA() const {
            return get<1>();
        }
        void setChain1InputCurrent_mA(Current_mA val) {
            set<1>(val);
        }
        Current_mA chain1OutputCurrent_mA() const {
            return get<2>();
        }
        void setChain1OutputCurrent_mA(Current_mA val) {
            set<2>(val);
        }
        Voltage_mV chain1OutputVoltage_mV() const {
            return get<3>();
        }
        void setChain1OutputVoltage_mV(Voltage_mV val) {
            set<3>(val);
        }
        Current_mA chain2InputCurrent_mA() const {
            return get<4>();
        }
        void setChain2InputCurrent_mA(Current_mA val) {
            set<4>(val);
        }
        Current_mA chain2OutputCurrent_mA() const {
            return get<5>();
        }
        void setChain2OutputCurrent_mA(Current_mA val) {
            set<5>(val);
        }
        Voltage_mV chain2OutputVoltage_mV() const {
            return get<6>();
        }
        void setChain2OutputVoltage_mV(Voltage_mV val) {
            set<6>(val);
        }
        PcuSdcChainStatus status() const {
            return get<7>();
        }
        void setStatus(PcuSdcChainStatus val) {
            set<7>(val);
        }
    };
    static_assert(17 == sizeof(PcuSdcTelemetry), "PCU SDC telemetry must be 17 byte long.");

    class PcuBatteryStatus : public s1utils::Bitfield<uint8_t, 1, 1, 1, 1, 1, 1, 2> {
    private:
        Q_GADGET
        Q_PROPERTY(int chargeOvercurrent READ chargeOvercurrent WRITE setChargeOvercurrent)
        Q_PROPERTY(int chargeOvervoltage READ chargeOvervoltage WRITE setChargeOvervoltage)
        Q_PROPERTY(int dischargeOvercurrent READ dischargeOvercurrent WRITE setDischargeOvercurrent)
        Q_PROPERTY(int dischargeOvervoltage READ dischargeOvervoltage WRITE setDischargeOvervoltage)
        Q_PROPERTY(int isChargeEnabled READ isChargeEnabled WRITE setIsChargeEnabled)
        Q_PROPERTY(int isDischargeEnabled READ isDischargeEnabled WRITE setIsDischargeEnabled)

    public:
        uint8_t chargeOvercurrent() const {
            return get<0>();
        }
        void setChargeOvercurrent(uint8_t val) {
            set<0>(val);
        }
        uint8_t chargeOvervoltage() const {
            return get<1>();
        }
        void setChargeOvervoltage(uint8_t val) {
            set<1>(val);
        }
        uint8_t dischargeOvercurrent() const {
            return get<2>();
        }
        void setDischargeOvercurrent(uint8_t val) {
            set<2>(val);
        }
        uint8_t dischargeOvervoltage() const {
            return get<3>();
        }
        void setDischargeOvervoltage(uint8_t val) {
            set<3>(val);
        }
        uint8_t isChargeEnabled() const {
            return get<4>();
        }
        void setIsChargeEnabled(uint8_t val) {
            set<4>(val);
        }
        uint8_t isDischargeEnabled() const {
            return get<5>();
        }
        void setIsDischargeEnabled(uint8_t val) {
            set<5>(val);
        }
    };
    static_assert(1 == sizeof(PcuBatteryStatus), "PCU battery status must be 1 byte long.");

    using PcuBatteryTelemetryBase = s1utils::Pack<Timestamp, Voltage_mV, Current_mA, Current_mA, PcuBatteryStatus>;
    class PcuBatteryTelemetry : public PcuBatteryTelemetryBase {
    private:
        Q_GADGET
        Q_PROPERTY(quint32 timestamp READ timestamp WRITE setTimestamp)
        Q_PROPERTY(quint16 voltage_mV READ voltage_mV WRITE setVoltage_mV)
        Q_PROPERTY(quint16 chargeCurrent_mA READ chargeCurrent_mA WRITE setChargeCurrent_mA)
        Q_PROPERTY(quint16 dischargeCurrent_mA READ dischargeCurrent_mA WRITE setDischargeCurrent_mA)
        Q_PROPERTY(s1obc::PcuBatteryStatus status READ status WRITE setStatus)

    public:
        Timestamp timestamp() const {
            return get<0>();
        }
        void setTimestamp(Timestamp val) {
            set<0>(val);
        }
        Voltage_mV voltage_mV() const {
            return get<1>();
        }
        void setVoltage_mV(Voltage_mV val) {
            set<1>(val);
        }
        Current_mA chargeCurrent_mA() const {
            return get<2>();
        }
        void setChargeCurrent_mA(Current_mA val) {
            set<2>(val);
        }
        Current_mA dischargeCurrent_mA() const {
            return get<3>();
        }
        void setDischargeCurrent_mA(Current_mA val) {
            set<3>(val);
        }
        PcuBatteryStatus status() const {
            return get<4>();
        }
        void setStatus(PcuBatteryStatus val) {
            set<4>(val);
        }
    };
    static_assert(11 == sizeof(PcuBatteryTelemetry), "PCU battery telemetry must be 11 byte long.");

    class PcuBusStatus : public s1utils::Bitfield<uint8_t, 1, 1, 6> {
    private:
        Q_GADGET
        Q_PROPERTY(int obc1Overcurrent READ obc1Overcurrent WRITE setObc1Overcurrent)
        Q_PROPERTY(int obc2Overcurrent READ obc2Overcurrent WRITE setObc2Overcurrent)

    public:
        uint8_t obc1Overcurrent() const {
            return get<0>();
        }
        void setObc1Overcurrent(uint8_t val) {
            set<0>(val);
        }
        uint8_t obc2Overcurrent() const {
            return get<1>();
        }
        void setObc2Overcurrent(uint8_t val) {
            set<1>(val);
        }
    };
    static_assert(1 == sizeof(PcuBusStatus), "PCU bus status must be 1 byte long.");

    using PcuBusTelemetryBase = s1utils::Pack<Timestamp, Voltage_mV, Voltage_mV, Current_mA, Current_mA, PcuBusStatus>;
    class PcuBusTelemetry : public PcuBusTelemetryBase {
    private:
        Q_GADGET
        Q_PROPERTY(quint32 timestamp READ timestamp WRITE setTimestamp)
        Q_PROPERTY(quint16 unregulatedBusVoltage_mV READ unregulatedBusVoltage_mV WRITE setUnregulatedBusVoltage_mV)
        Q_PROPERTY(quint16 regulatedBusVoltage_mV READ regulatedBusVoltage_mV WRITE setRegulatedBusVoltage_mV)
        Q_PROPERTY(quint16 obc1CurrentConsumption_mA READ obc1CurrentConsumption_mA WRITE setObc1CurrentConsumption_mA)
        Q_PROPERTY(quint16 obc2CurrentConsumption_mA READ obc2CurrentConsumption_mA WRITE setObc2CurrentConsumption_mA)
        Q_PROPERTY(s1obc::PcuBusStatus status READ status WRITE setStatus)

    public:
        Timestamp timestamp() const {
            return get<0>();
        }
        void setTimestamp(Timestamp val) {
            set<0>(val);
        }
        Voltage_mV unregulatedBusVoltage_mV() const {
            return get<1>();
        }
        void setUnregulatedBusVoltage_mV(Voltage_mV val) {
            set<1>(val);
        }
        Voltage_mV regulatedBusVoltage_mV() const {
            return get<2>();
        }
        void setRegulatedBusVoltage_mV(Voltage_mV val) {
            set<2>(val);
        }
        Current_mA obc1CurrentConsumption_mA() const {
            return get<3>();
        }
        void setObc1CurrentConsumption_mA(Current_mA val) {
            set<3>(val);
        }
        Current_mA obc2CurrentConsumption_mA() const {
            return get<4>();
        }
        void setObc2CurrentConsumption_mA(Current_mA val) {
            set<4>(val);
        }
        PcuBusStatus status() const {
            return get<5>();
        }
        void setStatus(PcuBusStatus val) {
            set<5>(val);
        }
    };
    static_assert(13 == sizeof(PcuBusTelemetry), "PCU bus telemetry must be 13 byte long.");

    class DiagnosticStatus : public s1utils::Bitfield<uint8_t, 3, 1, 4> {
    private:
        Q_GADGET
        Q_PROPERTY(int energyMode READ energyMode WRITE setEnergyMode)
        Q_PROPERTY(bool tcxoWorks READ tcxoWorks WRITE setTcxoWorks)

    public:
        EnergyMode energyMode() const {
            return (EnergyMode) get<0>();
        }
        void setEnergyMode(EnergyMode val) {
            set<0>(val);
        }
        void setEnergyMode(int val) {
            set<0>((uint8_t) val);
        }
        bool tcxoWorks() const {
            return get<1>();
        }
        void setTcxoWorks(bool val) {
            set<1>(val);
        }
    };
    static_assert(1 == sizeof(DiagnosticStatus), "DiagnosticStatus must be 1 byte long.");

    using DiagnosticInfoBase = s1utils::
        Pack<uint32_t, uint32_t, s1utils::s1_uint24_t, s1utils::s1_uint24_t, uint8_t, uint8_t, DiagnosticStatus>;
    class DiagnosticInfo : public DiagnosticInfoBase {
    private:
        Q_GADGET
        Q_PROPERTY(quint32 flashChecksum READ flashChecksum WRITE setFlashChecksum)
        Q_PROPERTY(quint32 lastUplinkTimestamp READ lastUplinkTimestamp WRITE setLastUplinkTimestamp)
        Q_PROPERTY(quint32 obcUptimeMin READ obcUptimeMin WRITE setObcUptimeMin)
        Q_PROPERTY(quint32 comUptimeMin READ comUptimeMin WRITE setComUptimeMin)
        Q_PROPERTY(int txVoltageDrop_10mV READ txVoltageDrop_10mV WRITE setTxVoltageDrop_10mV)
        Q_PROPERTY(int taskCount READ taskCount WRITE setTaskCount)
        Q_PROPERTY(s1obc::DiagnosticStatus status READ diagnosticStatus WRITE setDiagnosticStatus)

    public:
        uint32_t flashChecksum() const {
            return get<0>();
        }
        void setFlashChecksum(uint32_t val) {
            set<0>(val);
        }
        uint32_t lastUplinkTimestamp() const {
            return get<1>();
        }
        void setLastUplinkTimestamp(uint32_t val) {
            set<1>(val);
        }
        s1utils::s1_uint24_t obcUptimeMin() const {
            return get<2>();
        }
        void setObcUptimeMin(s1utils::s1_uint24_t val) {
            set<2>(val);
        }
        s1utils::s1_uint24_t comUptimeMin() const {
            return get<3>();
        }
        void setComUptimeMin(s1utils::s1_uint24_t val) {
            set<3>(val);
        }
        uint8_t txVoltageDrop_10mV() const {
            return get<4>();
        }
        void setTxVoltageDrop_10mV(uint8_t val) {
            set<4>(val);
        }
        uint8_t taskCount() const {
            return get<5>();
        }
        void setTaskCount(uint8_t val) {
            set<5>(val);
        }
        DiagnosticStatus diagnosticStatus() const {
            return get<6>();
        }
        void setDiagnosticStatus(DiagnosticStatus val) {
            set<6>(val);
        }
    };
    static_assert(17 == sizeof(DiagnosticInfo), "DiagnosticInfo must be 17 byte long.");

    using DiagnosticInfoBasePA = s1utils::Pack<uint32_t, uint16_t, uint16_t, uint16_t, uint16_t, uint32_t>;
    class DiagnosticInfoPA : public DiagnosticInfoBasePA {
    private:
        Q_GADGET
        Q_PROPERTY(quint32 validPackets READ validPackets WRITE setValidPackets)
        Q_PROPERTY(quint16 wrongSizedPackets READ wrongSizedPackets WRITE setWrongSizedPackets)
        Q_PROPERTY(quint16 packetsWithFailedGolayDecoding READ packetsWithFailedGolayDecoding WRITE
                setPacketsWithFailedGolayDecoding)
        Q_PROPERTY(quint16 packetsWithWrongSignature READ packetsWithWrongSignature WRITE setPacketsWithWrongSignature)
        Q_PROPERTY(quint16 packetsWithInvalidSerialNumber READ packetsWithInvalidSerialNumber WRITE
                setPacketsWithInvalidSerialNumber)
        Q_PROPERTY(quint32 uartErrorCounter READ uartErrorCounter WRITE setUartErrorCounter)
    public:
        uint32_t validPackets() const {
            return get<0>();
        }
        void setValidPackets(uint32_t val) {
            set<0>(val);
        }
        uint16_t wrongSizedPackets() const {
            return get<1>();
        }
        void setWrongSizedPackets(uint16_t val) {
            set<1>(val);
        }
        uint16_t packetsWithFailedGolayDecoding() const {
            return get<2>();
        }
        void setPacketsWithFailedGolayDecoding(uint16_t val) {
            set<2>(val);
        }
        uint16_t packetsWithWrongSignature() const {
            return get<3>();
        }
        void setPacketsWithWrongSignature(uint16_t val) {
            set<3>(val);
        }
        uint16_t packetsWithInvalidSerialNumber() const {
            return get<4>();
        }
        void setPacketsWithInvalidSerialNumber(uint16_t val) {
            set<4>(val);
        }
        uint32_t uartErrorCounter() const {
            return get<5>();
        }
        void setUartErrorCounter(uint32_t val) {
            set<5>(val);
        }
    };
    static_assert(16 == sizeof(DiagnosticInfoPA), "DiagnosticInfoPA must be 16 byte long.");

    using BatteryTelemetryBaseA = s1utils::
        Pack<uint8_t, uint8_t, Timestamp, ADCValue, ADCValue, ADCValue, ADCValue, ADCValue, ADCValue, ADCValue>;
    class BatteryTelemetryA : public BatteryTelemetryBaseA {
    private:
        Q_GADGET
        Q_PROPERTY(bool valid READ valid)
        Q_PROPERTY(quint16 oneWireBus READ oneWireBus)
        Q_PROPERTY(quint32 timestamp READ timestamp)
        Q_PROPERTY(QVariantList current_mA READ current_mA)
        Q_PROPERTY(QVariantList temperature1_mC READ temperature1_mC)
        Q_PROPERTY(QVariantList temperature2_mC READ temperature2_mC)
        Q_PROPERTY(QVariantList temperature3_mC READ temperature3_mC)
        Q_PROPERTY(QVariantList temperature4_mC READ temperature4_mC)
        Q_PROPERTY(QVariantList temperature5_mC READ temperature5_mC)
    public:
        bool valid() const {
            return get<0>() == 1;
        }
        uint16_t oneWireBus() const {
            return get<1>() + 1;
        }
        uint32_t timestamp() const {
            return get<2>();
        }
        QVariantList current_mA() const {
            QVariantList ls;
            ls.append(
                QVariant::fromValue(helper_DOWNLINK_PCKT_TYPE_ATL_TELEMETRY_3_calc_battery_current_mA(1, get<3>())));
            ls.append(
                QVariant::fromValue(helper_DOWNLINK_PCKT_TYPE_ATL_TELEMETRY_3_calc_battery_current_mA(2, get<3>())));
            ls.append(
                QVariant::fromValue(helper_DOWNLINK_PCKT_TYPE_ATL_TELEMETRY_3_calc_battery_current_mA(3, get<3>())));
            ls.append(
                QVariant::fromValue(helper_DOWNLINK_PCKT_TYPE_ATL_TELEMETRY_3_calc_battery_current_mA(4, get<3>())));
            return ls;
        }
        QVariantList temperature1_mC() const {
            QVariantList ls;
            ls.append(QVariant::fromValue(
                helper_DOWNLINK_PCKT_TYPE_ATL_TELEMETRY_3_calc_temperature_mC(1, get<4>(), get<5>(), 1)));
            ls.append(QVariant::fromValue(
                helper_DOWNLINK_PCKT_TYPE_ATL_TELEMETRY_3_calc_temperature_mC(2, get<4>(), get<5>(), 1)));
            ls.append(QVariant::fromValue(
                helper_DOWNLINK_PCKT_TYPE_ATL_TELEMETRY_3_calc_temperature_mC(3, get<4>(), get<5>(), 1)));
            ls.append(QVariant::fromValue(
                helper_DOWNLINK_PCKT_TYPE_ATL_TELEMETRY_3_calc_temperature_mC(4, get<4>(), get<5>(), 1)));
            return ls;
        }
        QVariantList temperature2_mC() const {
            QVariantList ls;
            ls.append(QVariant::fromValue(
                helper_DOWNLINK_PCKT_TYPE_ATL_TELEMETRY_3_calc_temperature_mC(1, get<4>(), get<6>(), 2)));
            ls.append(QVariant::fromValue(
                helper_DOWNLINK_PCKT_TYPE_ATL_TELEMETRY_3_calc_temperature_mC(2, get<4>(), get<6>(), 2)));
            ls.append(QVariant::fromValue(
                helper_DOWNLINK_PCKT_TYPE_ATL_TELEMETRY_3_calc_temperature_mC(3, get<4>(), get<6>(), 2)));
            ls.append(QVariant::fromValue(
                helper_DOWNLINK_PCKT_TYPE_ATL_TELEMETRY_3_calc_temperature_mC(4, get<4>(), get<6>(), 2)));
            return ls;
        }
        QVariantList temperature3_mC() const {
            QVariantList ls;
            ls.append(QVariant::fromValue(
                helper_DOWNLINK_PCKT_TYPE_ATL_TELEMETRY_3_calc_temperature_mC(1, get<4>(), get<7>(), 3)));
            ls.append(QVariant::fromValue(
                helper_DOWNLINK_PCKT_TYPE_ATL_TELEMETRY_3_calc_temperature_mC(2, get<4>(), get<7>(), 3)));
            ls.append(QVariant::fromValue(
                helper_DOWNLINK_PCKT_TYPE_ATL_TELEMETRY_3_calc_temperature_mC(3, get<4>(), get<7>(), 3)));
            ls.append(QVariant::fromValue(
                helper_DOWNLINK_PCKT_TYPE_ATL_TELEMETRY_3_calc_temperature_mC(4, get<4>(), get<7>(), 3)));
            return ls;
        }
        QVariantList temperature4_mC() const {
            QVariantList ls;
            ls.append(QVariant::fromValue(
                helper_DOWNLINK_PCKT_TYPE_ATL_TELEMETRY_3_calc_temperature_mC(1, get<4>(), get<8>(), 4)));
            ls.append(QVariant::fromValue(
                helper_DOWNLINK_PCKT_TYPE_ATL_TELEMETRY_3_calc_temperature_mC(2, get<4>(), get<8>(), 4)));
            ls.append(QVariant::fromValue(
                helper_DOWNLINK_PCKT_TYPE_ATL_TELEMETRY_3_calc_temperature_mC(3, get<4>(), get<8>(), 4)));
            ls.append(QVariant::fromValue(
                helper_DOWNLINK_PCKT_TYPE_ATL_TELEMETRY_3_calc_temperature_mC(4, get<4>(), get<8>(), 4)));
            return ls;
        }
        QVariantList temperature5_mC() const {
            QVariantList ls;
            ls.append(QVariant::fromValue(
                helper_DOWNLINK_PCKT_TYPE_ATL_TELEMETRY_3_calc_temperature_mC(1, get<4>(), get<9>(), 5)));
            ls.append(QVariant::fromValue(
                helper_DOWNLINK_PCKT_TYPE_ATL_TELEMETRY_3_calc_temperature_mC(2, get<4>(), get<9>(), 5)));
            ls.append(QVariant::fromValue(
                helper_DOWNLINK_PCKT_TYPE_ATL_TELEMETRY_3_calc_temperature_mC(3, get<4>(), get<9>(), 5)));
            ls.append(QVariant::fromValue(
                helper_DOWNLINK_PCKT_TYPE_ATL_TELEMETRY_3_calc_temperature_mC(4, get<4>(), get<9>(), 5)));
            return ls;
        }
    };
    static_assert(20 == sizeof(BatteryTelemetryA), "BatteryTelemetryA must be 20 byte long.");

} // namespace s1obc

Q_DECLARE_METATYPE(s1obc::EnergyMode)
Q_DECLARE_METATYPE(s1obc::ComTxPowerLevel)
Q_DECLARE_METATYPE(s1obc::ComTxDatarate)
Q_DECLARE_METATYPE(s1obc::FileType)
Q_DECLARE_METATYPE(s1obc::SolarPanelAntennaStatus)
Q_DECLARE_METATYPE(s1obc::FileEntry)
Q_DECLARE_METATYPE(s1obc::SpectrumAnalyzerStatus)
Q_DECLARE_METATYPE(s1obc::ComTelemetry)
Q_DECLARE_METATYPE(s1obc::ComTxStatus)
Q_DECLARE_METATYPE(s1obc::ComLimiterStatus)
Q_DECLARE_METATYPE(s1obc::ObcMotionSensorReading)
Q_DECLARE_METATYPE(s1obc::ObcMotionSensorTelemetry)
Q_DECLARE_METATYPE(s1obc::ObcBoardStatus)
Q_DECLARE_METATYPE(s1obc::ObcBoardStatusPA)
Q_DECLARE_METATYPE(s1obc::ObcTelemetry)
Q_DECLARE_METATYPE(s1obc::ObcTelemetryPA)
Q_DECLARE_METATYPE(s1obc::SolarPanelStatus)
Q_DECLARE_METATYPE(s1obc::SolarPanelTelemetry)
Q_DECLARE_METATYPE(s1obc::TidTelemetry)
Q_DECLARE_METATYPE(s1obc::PcuDeploymentStatus)
Q_DECLARE_METATYPE(s1obc::PcuDeploymentTelemetry)
Q_DECLARE_METATYPE(s1obc::PcuSdcChainStatus)
Q_DECLARE_METATYPE(s1obc::PcuSdcTelemetry)
Q_DECLARE_METATYPE(s1obc::PcuBatteryStatus)
Q_DECLARE_METATYPE(s1obc::PcuBatteryTelemetry)
Q_DECLARE_METATYPE(s1obc::PcuBusStatus)
Q_DECLARE_METATYPE(s1obc::PcuBusTelemetry)
Q_DECLARE_METATYPE(s1obc::DiagnosticStatus)
Q_DECLARE_METATYPE(s1obc::DiagnosticInfo)
Q_DECLARE_METATYPE(s1obc::DiagnosticInfoPA)
Q_DECLARE_METATYPE(s1obc::BatteryTelemetryA)

Q_DECLARE_METATYPE(s1obc::UplinkPacketType)
Q_DECLARE_METATYPE(s1obc::UplinkToggleType)
Q_DECLARE_METATYPE(s1obc::UplinkResetType)

Q_DECLARE_METATYPE(s1obc::DownlinkPacketType)
Q_DECLARE_METATYPE(s1obc::DownlinkEncoding)

#endif // S1OBC_TELEMETRY_H
