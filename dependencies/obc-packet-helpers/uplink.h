
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

#ifndef S1OBC_UPLINK_H
#define S1OBC_UPLINK_H

#include "telemetry.h"

namespace s1obc {

    constexpr size_t MaxUplinkPayloadSize = 22;

    using UplinkPingPacketBase = s1utils::Pack<UplinkPacketType, uint8_t[21]>;
    class UplinkPingPacket : public UplinkPingPacketBase {
    private:
        Q_GADGET

    public:
        UplinkPingPacket() {
            set<0>(UplinkPacketType_Ping);
        }
    };
    static_assert(sizeof(UplinkPingPacket) == MaxUplinkPayloadSize,
        "UplinkPingPacket MUST be exactly MaxUplinkPayloadSize bytes.");

    using UplinkTogglePacketBase = s1utils::Pack<UplinkPacketType, UplinkToggleType, uint8_t[20]>;
    class UplinkTogglePacket : public UplinkTogglePacketBase {
    private:
        Q_GADGET
        Q_PROPERTY(s1obc::UplinkToggleType toggleType READ toggleType WRITE setToggleType)

    public:
        UplinkTogglePacket() {
            set<0>(UplinkPacketType_Toggle);
            setToggleType(UplinkToggleType_COM);
        }

        UplinkToggleType toggleType() const {
            return get<1>();
        }
        void setToggleType(UplinkToggleType val) {
            set<1>(val);
        }
    };
    static_assert(sizeof(UplinkTogglePacket) == MaxUplinkPayloadSize,
        "UplinkTogglePacket MUST be exactly MaxUplinkPayloadSize bytes.");

    using UplinkResetPacketBase = s1utils::Pack<UplinkPacketType, UplinkResetType, uint8_t[20]>;
    class UplinkResetPacket : public UplinkResetPacketBase {
    private:
        Q_GADGET
        Q_PROPERTY(s1obc::UplinkResetType resetType READ resetType WRITE setResetType)

    public:
        UplinkResetPacket() {
            set<0>(UplinkPacketType_Reset);
            setResetType(UplinkResetType_COM);
        }

        UplinkResetType resetType() const {
            return get<1>();
        }
        void setResetType(UplinkResetType val) {
            set<1>(val);
        }
    };
    static_assert(sizeof(UplinkResetPacket) == MaxUplinkPayloadSize,
        "UplinkResetPacket MUST be exactly MaxUplinkPayloadSize bytes.");

    using UplinkSetComParametersPacketBase =
        s1utils::Pack<UplinkPacketType, ComTxPowerLevel, ComTxDatarate, uint16_t, uint16_t, uint16_t, uint8_t[13]>;

    class UplinkSetComParametersPacket : public UplinkSetComParametersPacketBase {
    private:
        Q_GADGET
        Q_PROPERTY(s1obc::ComTxPowerLevel txPowerLevel READ txPowerLevel WRITE setTxPowerLevel)
        Q_PROPERTY(s1obc::ComTxDatarate txDataRate READ txDataRate WRITE setTxDataRate)
        Q_PROPERTY(quint32 idleTimeNormal READ idleTimeNormal WRITE setIdleTimeNormal)
        Q_PROPERTY(quint32 idleTimeSaving READ idleTimeSaving WRITE setIdleTimeSaving)
        Q_PROPERTY(quint32 idleTimeEmergency READ idleTimeEmergency WRITE setIdleTimeEmergency)

    public:
        UplinkSetComParametersPacket() {
            set<0>(UplinkPacketType_SetComParameters);
            setTxPowerLevel(ComTxPowerLevel_1);
            setTxDataRate(ComTxDatarate_5000bps);
        }

        ComTxPowerLevel txPowerLevel() const {
            return get<1>();
        }
        void setTxPowerLevel(ComTxPowerLevel val) {
            set<1>(val);
        }
        ComTxDatarate txDataRate() const {
            return get<2>();
        }
        void setTxDataRate(ComTxDatarate val) {
            set<2>(val);
        }
        uint16_t idleTimeNormal() const {
            return get<3>();
        }
        void setIdleTimeNormal(uint16_t val) {
            set<3>(val);
        }
        uint16_t idleTimeSaving() const {
            return get<4>();
        }
        void setIdleTimeSaving(uint16_t val) {
            set<4>(val);
        }
        uint16_t idleTimeEmergency() const {
            return get<5>();
        }
        void setIdleTimeEmergency(uint16_t val) {
            set<5>(val);
        }
    };
    static_assert(sizeof(UplinkSetComParametersPacket) == MaxUplinkPayloadSize,
        "UplinkSetComParametersPacket MUST be exactly MaxUplinkPayloadSize bytes.");

    using UplinkStartSpectrumAnalyzerPacketBase =
        s1utils::Pack<UplinkPacketType, Frequency, Frequency, Frequency, Rbw, MeasurementId, Timestamp, uint8_t[2]>;
    class UplinkStartSpectrumAnalyzerPacket : public UplinkStartSpectrumAnalyzerPacketBase {
    private:
        Q_GADGET
        Q_PROPERTY(quint32 startFrequency READ startFrequency WRITE setStartFrequency)
        Q_PROPERTY(quint32 stopFrequency READ stopFrequency WRITE setStopFrequency)
        Q_PROPERTY(quint32 stepSize READ stepSize WRITE setStepSize)
        Q_PROPERTY(int rbw READ rbw WRITE setRbw)
        Q_PROPERTY(quint16 measurementIdentifier READ measurementIdentifier WRITE setMeasurementIdentifier)
        Q_PROPERTY(quint32 startTimestamp READ startTimestamp WRITE setStartTimestamp)

    public:
        UplinkStartSpectrumAnalyzerPacket() {
            set<0>(UplinkPacketType_StartSpectrumAnalyzer);
            setStartFrequency(460000000);
            setStopFrequency(860000000);
            setStepSize(9600);
            setRbw(8);
            setMeasurementIdentifier(1);
            setStartTimestamp(0);
        }

        Frequency startFrequency() const {
            return get<1>();
        }
        void setStartFrequency(Frequency val) {
            set<1>(val);
        }
        Frequency stopFrequency() const {
            return get<2>();
        }
        void setStopFrequency(Frequency val) {
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
        MeasurementId measurementIdentifier() const {
            return get<5>();
        }
        void setMeasurementIdentifier(MeasurementId val) {
            set<5>(val);
        }
        Timestamp startTimestamp() const {
            return get<6>();
        }
        void setStartTimestamp(Timestamp val) {
            set<6>(val);
        }
    };
    static_assert(sizeof(UplinkStartSpectrumAnalyzerPacket) == MaxUplinkPayloadSize,
        "UplinkStartSpectrumAnalyzerPacket MUST be exactly MaxUplinkPayloadSize bytes.");

    using UplinkOpenAntennaPacketBase = s1utils::Pack<UplinkPacketType, uint32_t, uint32_t, uint32_t, uint8_t[9]>;
    class UplinkOpenAntennaPacket : public UplinkOpenAntennaPacketBase {
    private:
        Q_GADGET
        Q_PROPERTY(quint32 batteryPwmTime READ batteryPwmTime WRITE setBatteryPwmTime)
        Q_PROPERTY(quint32 capacitorPwmChargeTime READ capacitorPwmChargeTime WRITE setCapacitorPwmChargeTime)
        Q_PROPERTY(quint32 capacitorPwmDischargeTime READ capacitorPwmDischargeTime WRITE setCapacitorPwmDischargeTime)

    public:
        UplinkOpenAntennaPacket() {
            set<0>(UplinkPacketType_OpenAntenna);
            setCapacitorPwmChargeTime(10 * 60 * 1000);
            setCapacitorPwmDischargeTime(5000);
        }

        uint32_t batteryPwmTime() const {
            return get<1>();
        }
        void setBatteryPwmTime(uint32_t val) {
            set<1>(val);
        }
        uint32_t capacitorPwmChargeTime() const {
            return get<2>();
        }
        void setCapacitorPwmChargeTime(uint32_t val) {
            set<2>(val);
        }
        uint32_t capacitorPwmDischargeTime() const {
            return get<3>();
        }
        void setCapacitorPwmDischargeTime(uint32_t val) {
            set<3>(val);
        }
    };
    static_assert(sizeof(UplinkOpenAntennaPacket) == MaxUplinkPayloadSize,
        "UplinkOpenAntennaPacket MUST be exactly MaxUplinkPayloadSize bytes.");

    using UplinkAntennaOpenSuccessPacketBase = s1utils::Pack<UplinkPacketType, uint8_t[21]>;
    class UplinkAntennaOpenSuccessPacket : public UplinkAntennaOpenSuccessPacketBase {
    private:
        Q_GADGET

    public:
        UplinkAntennaOpenSuccessPacket() {
            set<0>(UplinkPacketType_AntennaOpenSuccess);
        }
    };
    static_assert(sizeof(UplinkAntennaOpenSuccessPacket) == MaxUplinkPayloadSize,
        "UplinkAntennaOpenSuccessPacket MUST be exactly MaxUplinkPayloadSize bytes.");

    struct BeaconFragment {
        static constexpr size_t length = 20;
        char characters[length];
    } S1_PACKED;

    using UplinkSetBeaconPacketBase = s1utils::Pack<UplinkPacketType, uint8_t, BeaconFragment>;
    class UplinkSetBeaconPacket : public UplinkSetBeaconPacketBase {
    private:
        Q_GADGET
        Q_PROPERTY(int startIndex READ startIndex WRITE setStartIndex)
        Q_PROPERTY(QString beaconFragment READ beaconFragment_qt WRITE setBeaconFragment_qt)

    public:
        UplinkSetBeaconPacket() {
            set<0>(UplinkPacketType_SetBeacon);

            BeaconFragment defaultFragment;
            memset(defaultFragment.characters, 0, BeaconFragment::length);
            strcpy(defaultFragment.characters, "hello world");
            setBeaconFragment(defaultFragment);
        }

        uint8_t startIndex() const {
            return get<1>();
        }
        void setStartIndex(uint8_t val) {
            set<1>(val);
        }
        BeaconFragment beaconFragment() const {
            return get<2>();
        }
        void setBeaconFragment(const BeaconFragment &val) {
            set<2>(val);
        }

        QString beaconFragment_qt() const {
            BeaconFragment f = beaconFragment();
            return QString::fromLatin1(
                f.characters, f.characters[BeaconFragment::length - 1] == 0 ? (-1) : BeaconFragment::length);
        }
        void setBeaconFragment_qt(const QString &val) {
            BeaconFragment fragment;
            strncpy(fragment.characters, val.toLatin1().data(), BeaconFragment::length);
            setBeaconFragment(fragment);
        }
    };
    static_assert(sizeof(UplinkSetBeaconPacket) == MaxUplinkPayloadSize,
        "UplinkSetBeaconPacket MUST be exactly MaxUplinkPayloadSize bytes.");

    struct MorseMessage {
        static constexpr size_t length = 12;
        char characters[length];
    } S1_PACKED;

    using UplinkMorseRequestPacketBase = s1utils::Pack<UplinkPacketType, Timestamp, MorseMessage, uint8_t[5]>;
    class UplinkMorseRequestPacket : public UplinkMorseRequestPacketBase {
    private:
        Q_GADGET
        Q_PROPERTY(quint32 startTimestamp READ startTimestamp WRITE setStartTimestamp)
        Q_PROPERTY(QString morseMessage READ morseMessage_qt WRITE setMorseMessage_qt)

    public:
        UplinkMorseRequestPacket() {
            set<0>(UplinkPacketType_MorseRequest);

            MorseMessage defaultMessage;
            memset(defaultMessage.characters, 0, MorseMessage::length);
            strcpy(defaultMessage.characters, "hello world");
            setMorseMessage(defaultMessage);
        }

        Timestamp startTimestamp() const {
            return get<1>();
        }
        void setStartTimestamp(Timestamp val) {
            set<1>(val);
        }
        MorseMessage morseMessage() const {
            return get<2>();
        }
        void setMorseMessage(const MorseMessage &val) {
            set<2>(val);
        }

        QString morseMessage_qt() const {
            auto m = morseMessage();
            return QString::fromLatin1(
                m.characters, m.characters[MorseMessage::length - 1] == 0 ? (-1) : MorseMessage::length);
        }
        void setMorseMessage_qt(const QString &val) {
            MorseMessage message;
            strncpy(message.characters, val.toLatin1().data(), MorseMessage::length);
            setMorseMessage(message);
        }
    };
    static_assert(sizeof(UplinkMorseRequestPacket) == MaxUplinkPayloadSize,
        "UplinkMorseRequestPacket MUST be exactly MaxUplinkPayloadSize bytes.");

    using UplinkSilentModePacketBase = s1utils::Pack<UplinkPacketType, Timestamp, uint32_t, uint8_t[13]>;
    class UplinkSilentModePacket : public UplinkSilentModePacketBase {
    private:
        Q_GADGET
        Q_PROPERTY(quint32 startTimestamp READ startTimestamp WRITE setStartTimestamp)
        Q_PROPERTY(quint32 silentDuration READ silentDuration WRITE setSilentDuration)

    public:
        UplinkSilentModePacket() {
            set<0>(UplinkPacketType_SilentMode);
        }

        Timestamp startTimestamp() const {
            return get<1>();
        }
        void setStartTimestamp(Timestamp val) {
            set<1>(val);
        }
        uint32_t silentDuration() const {
            return get<2>();
        }
        void setSilentDuration(const uint32_t val) {
            set<2>(val);
        }
    };
    static_assert(sizeof(UplinkSilentModePacket) == MaxUplinkPayloadSize,
        "UplinkSilentModePacket MUST be exactly MaxUplinkPayloadSize bytes.");

    class FileDownloadFlags : public s1utils::Bitfield<uint8_t, 1, 3, 4> {
    private:
        Q_GADGET
        Q_PROPERTY(int shouldDelete READ shouldDelete WRITE setShouldDelete)
        Q_PROPERTY(s1obc::ComTxDatarate txDataRate READ txDataRate WRITE setTxDataRate)

    public:
        uint8_t shouldDelete() const {
            return get<0>();
        }
        void setShouldDelete(uint8_t val) {
            set<0>(val);
        }
        ComTxDatarate txDataRate() const {
            return (ComTxDatarate) get<1>();
        }
        void setTxDataRate(ComTxDatarate val) {
            set<1>(val);
        }
    };

    using UplinkFileDownloadRequestPacketBase = s1utils::Pack<UplinkPacketType,
        uint8_t,
        FileName,
        FileDownloadFlags,
        s1utils::s1_uint24_t,
        s1utils::s1_uint24_t,
        s1utils::s1_uint24_t>;
    class UplinkFileDownloadRequestPacket : public UplinkFileDownloadRequestPacketBase {
    private:
        Q_GADGET
        Q_PROPERTY(int fileId READ fileId WRITE setFileId)
        Q_PROPERTY(QString name READ fileName_qt WRITE setFileName_qt)
        Q_PROPERTY(int shouldDelete READ shouldDelete WRITE setShouldDelete)
        Q_PROPERTY(s1obc::ComTxDatarate txDataRate READ txDataRate WRITE setTxDataRate)
        Q_PROPERTY(quint32 offset READ offset WRITE setOffset)
        Q_PROPERTY(quint32 length READ length WRITE setLength)
        Q_PROPERTY(quint32 relativeTime READ relativeTime WRITE setRelativeTime)

    public:
        UplinkFileDownloadRequestPacket() {
            this->set<0>(UplinkPacketType_FileDownloadRequest);
            setFileName_qt(QStringLiteral("spa_1"));
            setTxDataRate(ComTxDatarate_5000bps);
        }

        uint8_t fileId() const {
            return get<1>();
        }
        void setFileId(uint8_t val) {
            set<1>(val);
        }

        FileName fileName() const {
            return get<2>();
        }
        void setFileName(const FileName &val) {
            set<2>(val);
        }

        FileDownloadFlags fileDownloadFlags() const {
            return get<3>();
        }
        void setFileDownloadFlags(FileDownloadFlags val) {
            set<3>(val);
        }

        bool shouldDelete() const {
            return fileDownloadFlags().shouldDelete();
        }
        void setShouldDelete(bool val) {
            setBitfield8<3, 0>(val);
        }

        ComTxDatarate txDataRate() const {
            return fileDownloadFlags().txDataRate();
        }
        void setTxDataRate(ComTxDatarate val) {
            setBitfield8<3, 1>(val);
        }

        s1utils::s1_uint24_t offset() const {
            return get<4>();
        }
        void setOffset(s1utils::s1_uint24_t val) {
            set<4>(val);
        }

        s1utils::s1_uint24_t length() const {
            return get<5>();
        }
        void setLength(s1utils::s1_uint24_t val) {
            set<5>(val);
        }

        s1utils::s1_uint24_t relativeTime() const {
            return get<6>();
        }
        void setRelativeTime(s1utils::s1_uint24_t val) {
            set<6>(val);
        }

        QString fileName_qt() const {
            FileName n = fileName();
            return QString::fromLatin1(n.characters, n.characters[FileName::length - 1] == 0 ? (-1) : FileName::length);
        }
        void setFileName_qt(const QString &val) {
            FileName n;
            strncpy(n.characters, val.toLatin1().data(), FileName::length);
            setFileName(n);
        }
    };
    static_assert(sizeof(UplinkFileDownloadRequestPacket) == MaxUplinkPayloadSize,
        "UplinkFileDownloadRequestPacket MUST be exactly MaxUplinkPayloadSize bytes.");

    class MeasurementSelectionSolar : public s1utils::Bitfield<uint8_t, 1, 1, 1, 1, 1, 1, 1, 1> {
    private:
        Q_GADGET
        Q_PROPERTY(int selectTemperature READ selectTemperature WRITE setSelectTemperature)
        Q_PROPERTY(int selectLightSensor READ selectLightSensor WRITE setSelectLightSensor)
        Q_PROPERTY(int selectInputCurrent READ selectInputCurrent WRITE setSelectInputCurrent)
        Q_PROPERTY(int selectInputVoltage READ selectInputVoltage WRITE setSelectInputVoltage)
        Q_PROPERTY(int selectOutputCurrent READ selectOutputCurrent WRITE setSelectOutputCurrent)
        Q_PROPERTY(int selectOutputVoltage READ selectOutputVoltage WRITE setSelectOutputVoltage)
        Q_PROPERTY(int selectTimestamp READ selectTimestamp WRITE setSelectTimestamp)

    public:
        uint8_t selectTemperature() const {
            return get<0>();
        }
        void setSelectTemperature(uint8_t val) {
            set<0>(val);
        }
        uint8_t selectLightSensor() const {
            return get<1>();
        }
        void setSelectLightSensor(uint8_t val) {
            set<1>(val);
        }
        uint8_t selectInputCurrent() const {
            return get<2>();
        }
        void setSelectInputCurrent(uint8_t val) {
            set<2>(val);
        }
        uint8_t selectInputVoltage() const {
            return get<3>();
        }
        void setSelectInputVoltage(uint8_t val) {
            set<3>(val);
        }
        uint8_t selectOutputCurrent() const {
            return get<4>();
        }
        void setSelectOutputCurrent(uint8_t val) {
            set<4>(val);
        }
        uint8_t selectOutputVoltage() const {
            return get<5>();
        }
        void setSelectOutputVoltage(uint8_t val) {
            set<5>(val);
        }
        uint8_t selectTimestamp() const {
            return get<6>();
        }
        void setSelectTimestamp(uint8_t val) {
            set<6>(val);
        }
    };

    class MeasurementSelectionPcu : public s1utils::Bitfield<uint16_t, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1> {
    private:
        Q_GADGET

        Q_PROPERTY(int selectSdc1InputCurrent READ selectSdc1InputCurrent WRITE setSelectSdc1InputCurrent)
        Q_PROPERTY(int selectSdc1OutputCurrent READ selectSdc1OutputCurrent WRITE setSelectSdc1OutputCurrent)
        Q_PROPERTY(int selectSdc1OutputVoltage READ selectSdc1OutputVoltage WRITE setSelectSdc1OutputVoltage)

        Q_PROPERTY(int selectSdc2InputCurrent READ selectSdc2InputCurrent WRITE setSelectSdc2InputCurrent)
        Q_PROPERTY(int selectSdc2OutputCurrent READ selectSdc2OutputCurrent WRITE setSelectSdc2OutputCurrent)
        Q_PROPERTY(int selectSdc2OutputVoltage READ selectSdc2OutputVoltage WRITE setSelectSdc2OutputVoltage)

        Q_PROPERTY(int selectRegulatedBusVoltage READ selectRegulatedBusVoltage WRITE setSelectRegulatedBusVoltage)
        Q_PROPERTY(
            int selectUnregulatedBusVoltage READ selectUnregulatedBusVoltage WRITE setSelectUnregulatedBusVoltage)
        Q_PROPERTY(int selectObc1Current READ selectObc1Current WRITE setSelectObc1Current)
        Q_PROPERTY(int selectObc2Current READ selectObc2Current WRITE setSelectObc2Current)

        Q_PROPERTY(int selectBatteryVoltage READ selectBatteryVoltage WRITE setSelectBatteryVoltage)
        Q_PROPERTY(int selectBatteryChargeCurrent READ selectBatteryChargeCurrent WRITE setSelectBatteryChargeCurrent)
        Q_PROPERTY(
            int selectBatteryDischargeCurrent READ selectBatteryDischargeCurrent WRITE setSelectBatteryDischargeCurrent)
        Q_PROPERTY(int selectBatteryChargeEnabled READ selectBatteryChargeEnabled WRITE setSelectBatteryChargeEnabled)
        Q_PROPERTY(
            int selectBatteryDischargeEnabled READ selectBatteryDischargeEnabled WRITE setSelectBatteryChargeEnabled)

        Q_PROPERTY(int usePcu2 READ usePcu2 WRITE setUsePcu2)

    public:
        uint16_t selectSdc1InputCurrent() const {
            return get<0>();
        }
        void setSelectSdc1InputCurrent(uint16_t val) {
            set<0>(val);
        }
        uint16_t selectSdc1OutputCurrent() const {
            return get<1>();
        }
        void setSelectSdc1OutputCurrent(uint16_t val) {
            set<1>(val);
        }
        uint16_t selectSdc1OutputVoltage() const {
            return get<2>();
        }
        void setSelectSdc1OutputVoltage(uint16_t val) {
            set<2>(val);
        }

        uint16_t selectSdc2InputCurrent() const {
            return get<3>();
        }
        void setSelectSdc2InputCurrent(uint16_t val) {
            set<3>(val);
        }
        uint16_t selectSdc2OutputCurrent() const {
            return get<4>();
        }
        void setSelectSdc2OutputCurrent(uint16_t val) {
            set<4>(val);
        }
        uint16_t selectSdc2OutputVoltage() const {
            return get<5>();
        }
        void setSelectSdc2OutputVoltage(uint16_t val) {
            set<5>(val);
        }

        uint16_t selectRegulatedBusVoltage() const {
            return get<6>();
        }
        void setSelectRegulatedBusVoltage(uint16_t val) {
            set<6>(val);
        }
        uint16_t selectUnregulatedBusVoltage() const {
            return get<7>();
        }
        void setSelectUnregulatedBusVoltage(uint16_t val) {
            set<7>(val);
        }
        uint16_t selectObc1Current() const {
            return get<8>();
        }
        void setSelectObc1Current(uint16_t val) {
            set<8>(val);
        }
        uint16_t selectObc2Current() const {
            return get<9>();
        }
        void setSelectObc2Current(uint16_t val) {
            set<9>(val);
        }

        uint16_t selectBatteryVoltage() const {
            return get<10>();
        }
        void setSelectBatteryVoltage(uint16_t val) {
            set<10>(val);
        }
        uint16_t selectBatteryChargeCurrent() const {
            return get<11>();
        }
        void setSelectBatteryChargeCurrent(uint16_t val) {
            set<11>(val);
        }
        uint16_t selectBatteryDischargeCurrent() const {
            return get<12>();
        }
        void setSelectBatteryDischargeCurrent(uint16_t val) {
            set<12>(val);
        }
        uint16_t selectBatteryChargeEnabled() const {
            return get<13>();
        }
        void setSelectBatteryChargeEnabled(uint16_t val) {
            set<13>(val);
        }
        uint16_t selectBatteryDischargeEnabled() const {
            return get<14>();
        }
        void setSelectBatteryDischargeEnabled(uint16_t val) {
            set<14>(val);
        }

        uint16_t usePcu2() const {
            return get<15>();
        }
        void setUsePcu2(uint16_t val) {
            set<15>(val);
        }
    };

    class MeasurementSelectionMpu : public s1utils::Bitfield<uint8_t, 1, 1, 1, 1, 1, 1, 2> {
    private:
        Q_GADGET

        Q_PROPERTY(int selectMpu1Temperature READ selectMpu1Temperature WRITE setSelectMpu1Temperature)
        Q_PROPERTY(int selectMpu1Gyroscope READ selectMpu1Gyroscope WRITE setSelectMpu1Gyroscope)
        Q_PROPERTY(int selectMpu1Magnetometer READ selectMpu1Magnetometer WRITE setSelectMpu1Magnetometer)

        Q_PROPERTY(int selectMpu2Temperature READ selectMpu2Temperature WRITE setSelectMpu2Temperature)
        Q_PROPERTY(int selectMpu2Gyroscope READ selectMpu2Gyroscope WRITE setSelectMpu2Gyroscope)
        Q_PROPERTY(int selectMpu2Magnetometer READ selectMpu2Magnetometer WRITE setSelectMpu2Magnetometer)

    public:
        uint8_t selectMpu1Temperature() const {
            return get<0>();
        }
        void setSelectMpu1Temperature(uint8_t val) {
            set<0>(val);
        }
        uint8_t selectMpu1Gyroscope() const {
            return get<1>();
        }
        void setSelectMpu1Gyroscope(uint8_t val) {
            set<1>(val);
        }
        uint8_t selectMpu1Magnetometer() const {
            return get<2>();
        }
        void setSelectMpu1Magnetometer(uint8_t val) {
            set<2>(val);
        }

        uint8_t selectMpu2Temperature() const {
            return get<3>();
        }
        void setSelectMpu2Temperature(uint8_t val) {
            set<3>(val);
        }
        uint8_t selectMpu2Gyroscope() const {
            return get<4>();
        }
        void setSelectMpu2Gyroscope(uint8_t val) {
            set<4>(val);
        }
        uint8_t selectMpu2Magnetometer() const {
            return get<5>();
        }
        void setSelectMpu2Magnetometer(uint8_t val) {
            set<5>(val);
        }
    };

    class MeasurementSelectionObcCom : public s1utils::Bitfield<uint16_t, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 6> {
    private:
        Q_GADGET
        Q_PROPERTY(int selectRtcc1Temperature READ selectRtcc1Temperature WRITE setSelectRtcc1Temperature)
        Q_PROPERTY(int selectRtcc2Temperature READ selectRtcc2Temperature WRITE setSelectRtcc2Temperature)
        Q_PROPERTY(int selectObcTemperature READ selectObcTemperature WRITE setSelectObcTemperature)
        Q_PROPERTY(int selectComCurrent READ selectComCurrent WRITE setSelectComCurrent)
        Q_PROPERTY(int selectEps2aTemperature1 READ selectEps2aTemperature1 WRITE setSelectEps2aTemperature1)
        Q_PROPERTY(int selectEps2aTemperature2 READ selectEps2aTemperature2 WRITE setSelectEps2aTemperature2)
        Q_PROPERTY(int selectComVoltage READ selectComVoltage WRITE setSelectComVoltage)
        Q_PROPERTY(int selectComTemperature READ selectComTemperature WRITE setSelectComTemperature)
        Q_PROPERTY(int selectSpectrumTemperature READ selectSpectrumTemperature WRITE setSelectSpectrumTemperature)
        Q_PROPERTY(int silentMode READ silentMode WRITE setSilentMode)

    public:
        uint16_t selectRtcc1Temperature() const {
            return get<0>();
        }
        void setSelectRtcc1Temperature(uint16_t val) {
            set<0>(val);
        }
        uint16_t selectRtcc2Temperature() const {
            return get<1>();
        }
        void setSelectRtcc2Temperature(uint16_t val) {
            set<1>(val);
        }
        uint16_t selectObcTemperature() const {
            return get<2>();
        }
        void setSelectObcTemperature(uint16_t val) {
            set<2>(val);
        }
        uint16_t selectComCurrent() const {
            return get<3>();
        }
        void setSelectComCurrent(uint16_t val) {
            set<3>(val);
        }
        uint16_t selectEps2aTemperature1() const {
            return get<4>();
        }
        void setSelectEps2aTemperature1(uint16_t val) {
            set<4>(val);
        }
        uint16_t selectEps2aTemperature2() const {
            return get<5>();
        }
        void setSelectEps2aTemperature2(uint16_t val) {
            set<5>(val);
        }
        uint16_t selectComVoltage() const {
            return get<6>();
        }
        void setSelectComVoltage(uint16_t val) {
            set<6>(val);
        }
        uint16_t selectComTemperature() const {
            return get<7>();
        }
        void setSelectComTemperature(uint16_t val) {
            set<7>(val);
        }
        uint16_t selectSpectrumTemperature() const {
            return get<8>();
        }
        void setSelectSpectrumTemperature(uint16_t val) {
            set<8>(val);
        }
        uint16_t silentMode() const {
            return get<9>();
        }
        void setSilentMode(uint16_t val) {
            set<9>(val);
        }
    };

    class MeasurementSelectionTid : public s1utils::Bitfield<uint16_t, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 6> {
    private:
        Q_GADGET
        Q_PROPERTY(int selectTid1Temperature READ selectTid1Temperature WRITE setSelectTid1Temperature)
        Q_PROPERTY(int selectTid1Serial READ selectTid1Serial WRITE setSelectTid1Serial)
        Q_PROPERTY(int selectTid1Radfet1 READ selectTid1Radfet1 WRITE setSelectTid1Radfet1)
        Q_PROPERTY(int selectTid1Radfet2 READ selectTid1Radfet2 WRITE setSelectTid1Radfet2)
        Q_PROPERTY(int selectTid1Voltage READ selectTid1Voltage WRITE setSelectTid1Voltage)

        Q_PROPERTY(int selectTid2Temperature READ selectTid2Temperature WRITE setSelectTid2Temperature)
        Q_PROPERTY(int selectTid2Serial READ selectTid2Serial WRITE setSelectTid2Serial)
        Q_PROPERTY(int selectTid2Radfet1 READ selectTid2Radfet1 WRITE setSelectTid2Radfet1)
        Q_PROPERTY(int selectTid2Radfet2 READ selectTid2Radfet2 WRITE setSelectTid2Radfet2)
        Q_PROPERTY(int selectTid2Voltage READ selectTid2Voltage WRITE setSelectTid2Voltage)

    public:
        uint16_t selectTid1Temperature() const {
            return get<0>();
        }
        void setSelectTid1Temperature(uint16_t val) {
            set<0>(val);
        }
        uint16_t selectTid1Serial() const {
            return get<1>();
        }
        void setSelectTid1Serial(uint16_t val) {
            set<1>(val);
        }
        uint16_t selectTid1Radfet1() const {
            return get<2>();
        }
        void setSelectTid1Radfet1(uint16_t val) {
            set<2>(val);
        }
        uint16_t selectTid1Radfet2() const {
            return get<3>();
        }
        void setSelectTid1Radfet2(uint16_t val) {
            set<3>(val);
        }
        uint16_t selectTid1Voltage() const {
            return get<4>();
        }
        void setSelectTid1Voltage(uint16_t val) {
            set<4>(val);
        }

        uint16_t selectTid2Temperature() const {
            return get<5>();
        }
        void setSelectTid2Temperature(uint16_t val) {
            set<5>(val);
        }
        uint16_t selectTid2Serial() const {
            return get<6>();
        }
        void setSelectTid2Serial(uint16_t val) {
            set<6>(val);
        }
        uint16_t selectTid2Radfet1() const {
            return get<7>();
        }
        void setSelectTid2Radfet1(uint16_t val) {
            set<7>(val);
        }
        uint16_t selectTid2Radfet2() const {
            return get<8>();
        }
        void setSelectTid2Radfet2(uint16_t val) {
            set<8>(val);
        }
        uint16_t selectTid2Voltage() const {
            return get<9>();
        }
        void setSelectTid2Voltage(uint16_t val) {
            set<9>(val);
        }
    };

    using UplinkMeasurementRequestPacketBase = s1utils::Pack<UplinkPacketType,
        uint32_t,
        uint16_t,
        uint16_t,
        MeasurementSelectionSolar,
        MeasurementSelectionPcu,
        MeasurementSelectionMpu,
        MeasurementSelectionObcCom,
        MeasurementSelectionTid,
        uint8_t,
        uint8_t,
        uint8_t,
        uint8_t,
        uint8_t>;
    class UplinkMeasurementRequestPacket : public UplinkMeasurementRequestPacketBase {
    private:
        Q_GADGET

        Q_PROPERTY(quint32 startTimestamp READ startTimestamp WRITE setStartTimestamp)
        Q_PROPERTY(int measurementIdentifier READ measurementIdentifier WRITE setMeasurementIdentifier)
        Q_PROPERTY(int durationMinutes READ durationMinutes WRITE setDurationMinutes)

        Q_PROPERTY(s1obc::MeasurementSelectionSolar solar READ solar WRITE setSolar)
        Q_PROPERTY(s1obc::MeasurementSelectionPcu pcu READ pcu WRITE setPcu)
        Q_PROPERTY(s1obc::MeasurementSelectionMpu mpu READ mpu WRITE setMpu)
        Q_PROPERTY(s1obc::MeasurementSelectionObcCom obcCom READ obcCom WRITE setObcCom)
        Q_PROPERTY(s1obc::MeasurementSelectionTid tid READ tid WRITE setTid)

        Q_PROPERTY(int intervalSolarSec READ intervalSolarSec WRITE setIntervalSolarSec)
        Q_PROPERTY(int intervalPcuSec READ intervalPcuSec WRITE setIntervalPcuSec)
        Q_PROPERTY(int intervalMpuDecisec READ intervalMpuDecisec WRITE setIntervalMpuDecisec)
        Q_PROPERTY(int intervalObcComSec READ intervalObcComSec WRITE setIntervalObcComSec)
        Q_PROPERTY(int intervalTid5min READ intervalTid5min WRITE setIntervalTid5min)

    public:
        UplinkMeasurementRequestPacket() {
            this->set<0>(UplinkPacketType_MeasurementRequest);
        }

        uint32_t startTimestamp() const {
            return get<1>();
        }
        void setStartTimestamp(uint32_t val) {
            set<1>(val);
        }
        uint16_t measurementIdentifier() const {
            return get<2>();
        }
        void setMeasurementIdentifier(uint16_t val) {
            set<2>(val);
        }
        uint16_t durationMinutes() const {
            return get<3>();
        }
        void setDurationMinutes(uint16_t val) {
            set<3>(val);
        }

        MeasurementSelectionSolar solar() const {
            return get<4>();
        }
        void setSolar(const MeasurementSelectionSolar &val) {
            set<4>(val);
        }
        MeasurementSelectionPcu pcu() const {
            return get<5>();
        }
        void setPcu(const MeasurementSelectionPcu &val) {
            set<5>(val);
        }
        MeasurementSelectionMpu mpu() const {
            return get<6>();
        }
        void setMpu(const MeasurementSelectionMpu &val) {
            set<6>(val);
        }
        MeasurementSelectionObcCom obcCom() const {
            return get<7>();
        }
        void setObcCom(const MeasurementSelectionObcCom &val) {
            set<7>(val);
        }
        MeasurementSelectionTid tid() const {
            return get<8>();
        }
        void setTid(const MeasurementSelectionTid &val) {
            set<8>(val);
        }

        uint8_t intervalSolarSec() const {
            return get<9>();
        }
        void setIntervalSolarSec(uint8_t val) {
            set<9>(val);
        }
        uint8_t intervalPcuSec() const {
            return get<10>();
        }
        void setIntervalPcuSec(uint8_t val) {
            set<10>(val);
        }
        uint8_t intervalMpuDecisec() const {
            return get<11>();
        }
        void setIntervalMpuDecisec(uint8_t val) {
            set<11>(val);
        }
        uint8_t intervalObcComSec() const {
            return get<12>();
        }
        void setIntervalObcComSec(uint8_t val) {
            set<12>(val);
        }
        uint8_t intervalTid5min() const {
            return get<13>();
        }
        void setIntervalTid5min(uint8_t val) {
            set<13>(val);
        }
    };
    static_assert(sizeof(UplinkMeasurementRequestPacket) == MaxUplinkPayloadSize,
        "UplinkMeasurementRequestPacket MUST be exactly MaxUplinkPayloadSize bytes.");

    using UplinkFileDeletePacketBase = s1utils::Pack<UplinkPacketType, uint8_t, FileName, uint8_t[10]>;
    class UplinkFileDeletePacket : public UplinkFileDeletePacketBase {
    private:
        Q_GADGET
        Q_PROPERTY(int fileNameIsFilter READ fileNameIsFilter WRITE setFileNameIsFilter)
        Q_PROPERTY(QString fileName READ fileName_qt WRITE setFileName_qt)

    public:
        UplinkFileDeletePacket() {
            set<0>(UplinkPacketType_FileDelete);
        }

        uint8_t fileNameIsFilter() const {
            return get<1>();
        }
        void setFileNameIsFilter(uint8_t val) {
            set<1>(val);
        }

        FileName fileName() const {
            return get<2>();
        }
        void setFileName(const FileName &val) {
            set<2>(val);
        }

        QString fileName_qt() const {
            FileName n = fileName();
            return QString::fromLatin1(n.characters, n.characters[FileName::length - 1] == 0 ? (-1) : FileName::length);
        }
        void setFileName_qt(const QString &val) {
            FileName n;
            strncpy(n.characters, val.toLatin1().data(), FileName::length);
            setFileName(n);
        }
    };
    static_assert(sizeof(UplinkFileDeletePacket) == MaxUplinkPayloadSize,
        "UplinkFileDeletePacket MUST be exactly MaxUplinkPayloadSize bytes.");

} // namespace s1obc

Q_DECLARE_METATYPE(s1obc::UplinkPingPacket)
Q_DECLARE_METATYPE(s1obc::UplinkTogglePacket)
Q_DECLARE_METATYPE(s1obc::UplinkResetPacket)
Q_DECLARE_METATYPE(s1obc::UplinkSetComParametersPacket)
Q_DECLARE_METATYPE(s1obc::UplinkStartSpectrumAnalyzerPacket)
Q_DECLARE_METATYPE(s1obc::UplinkOpenAntennaPacket)
Q_DECLARE_METATYPE(s1obc::UplinkAntennaOpenSuccessPacket)
Q_DECLARE_METATYPE(s1obc::UplinkSetBeaconPacket)
Q_DECLARE_METATYPE(s1obc::FileDownloadFlags)
Q_DECLARE_METATYPE(s1obc::MeasurementSelectionSolar)
Q_DECLARE_METATYPE(s1obc::MeasurementSelectionPcu)
Q_DECLARE_METATYPE(s1obc::MeasurementSelectionMpu)
Q_DECLARE_METATYPE(s1obc::MeasurementSelectionObcCom)
Q_DECLARE_METATYPE(s1obc::MeasurementSelectionTid)
Q_DECLARE_METATYPE(s1obc::UplinkMeasurementRequestPacket)
Q_DECLARE_METATYPE(s1obc::UplinkFileDeletePacket)
Q_DECLARE_METATYPE(s1obc::UplinkMorseRequestPacket)
Q_DECLARE_METATYPE(s1obc::UplinkSilentModePacket)

#endif // S1OBC_UPLINK_H
