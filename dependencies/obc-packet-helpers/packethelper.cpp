
#include "packethelper.h"

#include <QtQml>

void s1obc::registerObcPacketTypesQt() {
    qRegisterMetaType<s1obc::EnergyMode>();
    qRegisterMetaType<s1obc::ComTxPowerLevel>();
    qRegisterMetaType<s1obc::ComTxDatarate>();
    qRegisterMetaType<s1obc::FileType>();
    qRegisterMetaType<s1obc::SolarPanelAntennaStatus>();
    qRegisterMetaType<s1obc::FileEntry>();
    qRegisterMetaType<s1obc::SpectrumAnalyzerStatus>();
    qRegisterMetaType<s1obc::ComTelemetry>();
    qRegisterMetaType<s1obc::ComTxStatus>();
    qRegisterMetaType<s1obc::ComLimiterStatus>();
    qRegisterMetaType<s1obc::ObcMotionSensorReading>();
    qRegisterMetaType<s1obc::ObcMotionSensorTelemetry>();
    qRegisterMetaType<s1obc::ObcBoardStatus>();
    qRegisterMetaType<s1obc::ObcBoardStatusPA>();
    qRegisterMetaType<s1obc::ObcTelemetry>();
    qRegisterMetaType<s1obc::ObcTelemetryPA>();
    qRegisterMetaType<s1obc::SolarPanelStatus>();
    qRegisterMetaType<s1obc::SolarPanelTelemetry>();
    qRegisterMetaType<s1obc::TidTelemetry>();
    qRegisterMetaType<s1obc::PcuDeploymentStatus>();
    qRegisterMetaType<s1obc::PcuDeploymentTelemetry>();
    qRegisterMetaType<s1obc::PcuSdcChainStatus>();
    qRegisterMetaType<s1obc::PcuSdcTelemetry>();
    qRegisterMetaType<s1obc::PcuBatteryStatus>();
    qRegisterMetaType<s1obc::PcuBatteryTelemetry>();
    qRegisterMetaType<s1obc::PcuBusStatus>();
    qRegisterMetaType<s1obc::PcuBusTelemetry>();
    qRegisterMetaType<s1obc::DiagnosticStatus>();
    qRegisterMetaType<s1obc::DiagnosticInfo>();
    qRegisterMetaType<s1obc::DiagnosticInfoPA>();
    qRegisterMetaType<s1obc::BatteryTelemetryA>();

    qRegisterMetaType<s1obc::UplinkPacketType>();
    qRegisterMetaType<s1obc::UplinkToggleType>();
    qRegisterMetaType<s1obc::UplinkResetType>();

    qRegisterMetaType<s1obc::DownlinkPacketType>();
    qRegisterMetaType<s1obc::DownlinkEncoding>();

    qRegisterMetaType<s1obc::UplinkPingPacket>();
    qRegisterMetaType<s1obc::UplinkTogglePacket>();
    qRegisterMetaType<s1obc::UplinkResetPacket>();
    qRegisterMetaType<s1obc::UplinkSetComParametersPacket>();
    qRegisterMetaType<s1obc::UplinkStartSpectrumAnalyzerPacket>();
    qRegisterMetaType<s1obc::UplinkOpenAntennaPacket>();
    qRegisterMetaType<s1obc::UplinkAntennaOpenSuccessPacket>();
    qRegisterMetaType<s1obc::UplinkSetBeaconPacket>();
    qRegisterMetaType<s1obc::FileDownloadFlags>();
    qRegisterMetaType<s1obc::UplinkFileDownloadRequestPacket>();
    qRegisterMetaType<s1obc::MeasurementSelectionSolar>();
    qRegisterMetaType<s1obc::MeasurementSelectionPcu>();
    qRegisterMetaType<s1obc::MeasurementSelectionMpu>();
    qRegisterMetaType<s1obc::MeasurementSelectionObcCom>();
    qRegisterMetaType<s1obc::UplinkMeasurementRequestPacket>();
    qRegisterMetaType<s1obc::UplinkFileDeletePacket>();
    qRegisterMetaType<s1obc::UplinkMorseRequestPacket>();

    qRegisterMetaType<s1obc::AcknowledgedCommand>();
    qRegisterMetaType<s1obc::SolarPanelTelemetryPacket>();
    qRegisterMetaType<s1obc::PcuTelemetryPacket>();
    qRegisterMetaType<s1obc::OnboardTelemetryPacket>();
    qRegisterMetaType<s1obc::OnboardTelemetryPacketPA>();
    qRegisterMetaType<s1obc::BeaconPacket>();
    qRegisterMetaType<s1obc::BeaconPacketPA>();
    qRegisterMetaType<s1obc::BatteryPacketA>();
    qRegisterMetaType<s1obc::FileDownloadPacket>();
    qRegisterMetaType<s1obc::SpectrumPacket>();

    const char *import = "s1obc";
    const char *msg = "OBC: Can't create this type in QML.";

    qmlRegisterUncreatableMetaObject(s1obc::staticMetaObject, import, 1, 0, "OBC", msg);

    qmlRegisterUncreatableType<s1obc::FileEntry>(import, 1, 0, "FileEntry", msg);
    qmlRegisterUncreatableType<s1obc::SpectrumAnalyzerStatus>(import, 1, 0, "SpectrumAnalyzerStatus", msg);
    qmlRegisterUncreatableType<s1obc::ComTelemetry>(import, 1, 0, "ComTelemetry", msg);
    qmlRegisterUncreatableType<s1obc::ComTxStatus>(import, 1, 0, "ComTxStatus", msg);
    qmlRegisterUncreatableType<s1obc::ComLimiterStatus>(import, 1, 0, "ComLimiterStatus", msg);
    qmlRegisterUncreatableType<s1obc::ObcMotionSensorReading>(import, 1, 0, "ObcMotionSensorReading", msg);
    qmlRegisterUncreatableType<s1obc::ObcMotionSensorTelemetry>(import, 1, 0, "ObcMotionSensorTelemetry", msg);
    qmlRegisterUncreatableType<s1obc::ObcBoardStatus>(import, 1, 0, "ObcBoardStatus", msg);
    qmlRegisterUncreatableType<s1obc::ObcBoardStatusPA>(import, 1, 0, "ObcBoardStatusPA", msg);
    qmlRegisterUncreatableType<s1obc::ObcTelemetry>(import, 1, 0, "ObcTelemetry", msg);
    qmlRegisterUncreatableType<s1obc::ObcTelemetryPA>(import, 1, 0, "ObcTelemetryPA", msg);
    qmlRegisterUncreatableType<s1obc::SolarPanelStatus>(import, 1, 0, "SolarPanelStatus", msg);
    qmlRegisterUncreatableType<s1obc::SolarPanelTelemetry>(import, 1, 0, "SolarPanelTelemetry", msg);
    qmlRegisterUncreatableType<s1obc::TidTelemetry>(import, 1, 0, "TidTelemetry", msg);
    qmlRegisterUncreatableType<s1obc::PcuDeploymentStatus>(import, 1, 0, "PcuDeploymentStatus", msg);
    qmlRegisterUncreatableType<s1obc::PcuDeploymentTelemetry>(import, 1, 0, "PcuDeploymentTelemetry", msg);
    qmlRegisterUncreatableType<s1obc::PcuSdcChainStatus>(import, 1, 0, "PcuSdcChainStatus", msg);
    qmlRegisterUncreatableType<s1obc::PcuSdcTelemetry>(import, 1, 0, "PcuSdcTelemetry", msg);
    qmlRegisterUncreatableType<s1obc::PcuBatteryStatus>(import, 1, 0, "PcuBatteryStatus", msg);
    qmlRegisterUncreatableType<s1obc::PcuBatteryTelemetry>(import, 1, 0, "PcuBatteryTelemetry", msg);
    qmlRegisterUncreatableType<s1obc::PcuBusStatus>(import, 1, 0, "PcuBusStatus", msg);
    qmlRegisterUncreatableType<s1obc::PcuBusTelemetry>(import, 1, 0, "PcuBusTelemetry", msg);
    qmlRegisterUncreatableType<s1obc::DiagnosticStatus>(import, 1, 0, "DiagnosticStatus", msg);
    qmlRegisterUncreatableType<s1obc::DiagnosticInfo>(import, 1, 0, "DiagnosticInfo", msg);
    qmlRegisterUncreatableType<s1obc::DiagnosticInfoPA>(import, 1, 0, "DiagnosticInfoPA", msg);
    qmlRegisterUncreatableType<s1obc::BatteryTelemetryA>(import, 1, 0, "BatteryTelemetryA", msg);
    // Some registered types from above are missing here. Is that on purpose? (i.e. EnergyMode, FileType,...)

    qmlRegisterUncreatableType<s1obc::UplinkPingPacket>(import, 1, 0, "UplinkPingPacket", msg);
    qmlRegisterUncreatableType<s1obc::UplinkTogglePacket>(import, 1, 0, "UplinkTogglePacket", msg);
    qmlRegisterUncreatableType<s1obc::UplinkResetPacket>(import, 1, 0, "UplinkResetPacket", msg);
    qmlRegisterUncreatableType<s1obc::UplinkSetComParametersPacket>(import, 1, 0, "UplinkSetComParametersPacket", msg);
    qmlRegisterUncreatableType<s1obc::UplinkStartSpectrumAnalyzerPacket>(
        import, 1, 0, "UplinkStartSpectrumAnalyzerPacket", msg);
    qmlRegisterUncreatableType<s1obc::UplinkOpenAntennaPacket>(import, 1, 0, "UplinkOpenAntennaPacket", msg);
    qmlRegisterUncreatableType<s1obc::UplinkAntennaOpenSuccessPacket>(
        import, 1, 0, "UplinkAntennaOpenSuccessPacket", msg);
    qmlRegisterUncreatableType<s1obc::UplinkSetBeaconPacket>(import, 1, 0, "UplinkSetBeaconPacket", msg);
    qmlRegisterUncreatableType<s1obc::FileDownloadFlags>(import, 1, 0, "FileDownloadFlags", msg);
    qmlRegisterUncreatableType<s1obc::UplinkFileDownloadRequestPacket>(
        import, 1, 0, "UplinkFileDownloadRequestPacket", msg);
    qmlRegisterUncreatableType<s1obc::MeasurementSelectionSolar>(import, 1, 0, "MeasurementSelectionSolar", msg);
    qmlRegisterUncreatableType<s1obc::MeasurementSelectionPcu>(import, 1, 0, "MeasurementSelectionPcu", msg);
    qmlRegisterUncreatableType<s1obc::MeasurementSelectionMpu>(import, 1, 0, "MeasurementSelectionMpu", msg);
    qmlRegisterUncreatableType<s1obc::MeasurementSelectionObcCom>(import, 1, 0, "MeasurementSelectionObcCom", msg);
    qmlRegisterUncreatableType<s1obc::UplinkMeasurementRequestPacket>(
        import, 1, 0, "UplinkMeasurementRequestPacket", msg);
    qmlRegisterUncreatableType<s1obc::UplinkMorseRequestPacket>(import, 1, 0, "UplinkMorseRequestPacket", msg);

    qmlRegisterUncreatableType<s1obc::AcknowledgedCommand>(import, 1, 0, "AcknowledgedCommand", msg);
    qmlRegisterUncreatableType<s1obc::SolarPanelTelemetryPacket>(import, 1, 0, "SolarPanelTelemetryPacket", msg);
    qmlRegisterUncreatableType<s1obc::PcuTelemetryPacket>(import, 1, 0, "PcuTelemetryPacket", msg);
    qmlRegisterUncreatableType<s1obc::OnboardTelemetryPacket>(import, 1, 0, "OnboardTelemetryPacket", msg);
    qmlRegisterUncreatableType<s1obc::OnboardTelemetryPacketPA>(import, 1, 0, "OnboardTelemetryPacketPA", msg);
    qmlRegisterUncreatableType<s1obc::BeaconPacket>(import, 1, 0, "BeaconPacket", msg);
    qmlRegisterUncreatableType<s1obc::BeaconPacketPA>(import, 1, 0, "BeaconPacketPA", msg);
    qmlRegisterUncreatableType<s1obc::BatteryPacketA>(import, 1, 0, "BatteryPacketA", msg);
    qmlRegisterUncreatableType<s1obc::FileDownloadPacket>(import, 1, 0, "FileDownloadPacket", msg);
    qmlRegisterUncreatableType<s1obc::SpectrumPacket>(import, 1, 0, "SpectrumPacket", msg);

    qmlRegisterUncreatableType<s1obc::PacketHelper>(import, 1, 0, "UplinkHelper", msg);
}

s1obc::PacketHelper::PacketHelper(QObject *parent) : QObject(parent) {
}

QVariant s1obc::PacketHelper::createUplink(s1obc::UplinkPacketType packetType) {
    switch (packetType) {
    case UplinkPacketType_Ping:
        return QVariant::fromValue(UplinkPingPacket());
    case UplinkPacketType_Toggle:
        return QVariant::fromValue(UplinkTogglePacket());
    case UplinkPacketType_Reset:
        return QVariant::fromValue(UplinkResetPacket());
    case UplinkPacketType_SetComParameters:
        return QVariant::fromValue(UplinkSetComParametersPacket());
    case UplinkPacketType_StartSpectrumAnalyzer:
        return QVariant::fromValue(UplinkStartSpectrumAnalyzerPacket());
    case UplinkPacketType_OpenAntenna:
        return QVariant::fromValue(UplinkOpenAntennaPacket());
    case UplinkPacketType_AntennaOpenSuccess:
        return QVariant::fromValue(UplinkAntennaOpenSuccessPacket());
    case UplinkPacketType_SetBeacon:
        return QVariant::fromValue(UplinkSetBeaconPacket());
    case UplinkPacketType_FileDownloadRequest:
        return QVariant::fromValue(UplinkFileDownloadRequestPacket());
    case UplinkPacketType_MeasurementRequest:
        return QVariant::fromValue(UplinkMeasurementRequestPacket());
    case UplinkPacketType_FileDelete:
        return QVariant::fromValue(UplinkFileDeletePacket());
    case UplinkPacketType_MorseRequest:
        return QVariant::fromValue(UplinkMorseRequestPacket());
    default:
        return QVariant();
    }
}

QVariant s1obc::PacketHelper::createDownlink(
    s1obc::DownlinkPacketType packetType, SatelliteChanger::Satellites satellite) {
    switch (packetType) {
    case DownlinkPacketType_Telemetry1:
        switch (satellite) {
        case SatelliteChanger::Satellites::SMOG1:
        case SatelliteChanger::Satellites::SMOGP:
        case SatelliteChanger::Satellites::ATL1:
            return QVariant::fromValue(SolarPanelTelemetryPacket());
        default:
            return QVariant();
        }
    case DownlinkPacketType_Telemetry2:
        switch (satellite) {
        case SatelliteChanger::Satellites::SMOG1:
        case SatelliteChanger::Satellites::SMOGP:
        case SatelliteChanger::Satellites::ATL1:
            return QVariant::fromValue(PcuTelemetryPacket());
        default:
            return QVariant();
        }
    case DownlinkPacketType_Telemetry3:
        switch (satellite) {
        case SatelliteChanger::Satellites::SMOG1:
            return QVariant::fromValue(OnboardTelemetryPacket());
        case SatelliteChanger::Satellites::SMOGP:
        case SatelliteChanger::Satellites::ATL1:
            return QVariant::fromValue(OnboardTelemetryPacketPA());
        default:
            return QVariant();
        }
    case DownlinkPacketType_Beacon:
        switch (satellite) {
        case SatelliteChanger::Satellites::SMOG1:
            return QVariant::fromValue(BeaconPacket());
        case SatelliteChanger::Satellites::SMOGP:
        case SatelliteChanger::Satellites::ATL1:
            return QVariant::fromValue(BeaconPacketPA());
        default:
            return QVariant();
        }
    case DownlinkPacketType_Telemetry3_A: {
        switch (satellite) {
        case SatelliteChanger::Satellites::ATL1:
            return QVariant::fromValue(BatteryPacketA());
        default:
            return QVariant();
        }
    }
    default:
        return QVariant();
    }
}
