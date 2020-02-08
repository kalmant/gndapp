#include "packetdecoder.h"

#ifdef CHECK_SIGNATURE

// Not included in this version

#endif // CHECK_SIGNATURE

/**
 * @brief Consturctor for the class.
 *
 * @param file The pointer to the file that packet log will be written to
 * @param downloadDirString the folder that downloaded files will be written to
 * @param spectrumReceiver pointer to the Spectrum Receiver object
 * @param parent Parent QObject pointer, should be left empty
 */
PacketDecoder::PacketDecoder(QString logDirString,
    QString fileName,
    QString downloadDirString,
    SpectrumReceiver *spectrumReceiver,
    QObject *parent)
: QObject(parent) {
    this->logDirString = logDirString;
    this->fileName = fileName;
    this->prefix = "startup";
    this->downloadDirString = downloadDirString;
    this->spectrumReceiver_priv = spectrumReceiver;
    this->currentSatellite = SatelliteChanger::Satellites::UNSET;

    // The "initial file" is both empty and finished. This ensures that even if we miss the first fragment of the first
    // real file, a new file will be created, since isFinished is true
    fileDownload_priv.reset(new FileDownload("INITIAL_INVALID", 1, s1obc::FileType_Deleted));
    fileDownload_priv.data()->isFinished = true;

    QObject::connect(&syncTimeoutTimer_priv, &QTimer::timeout, [this]() {
        qInfo() << "No packet received in time, waiting for sync packet";
        if (fileDownload_priv.data() && !fileDownload_priv.data()->isFinished) {
            bool result = fileDownload_priv.data()->finishUpFile();
            if (result) {
                // successful file download
            }
            else {
                // a partial file may have been created
            }
        }
        emit timeOutSpectrumReception();
        this->waitForSyncPacket();
    });
    QObject::connect(this, &PacketDecoder::stopSyncTimeoutTimer, &syncTimeoutTimer_priv, &QTimer::stop);
    QObject::connect(this, &PacketDecoder::startSyncTimeoutTimer, &syncTimeoutTimer_priv, [this](int ms) {
        syncTimeoutTimer_priv.start(ms);
    });
    QTimer::singleShot(1000, [this]() { this->waitForSyncPacket(); });
    syncTimeoutTimer_priv.setSingleShot(true);
}

/**
 * @brief Performs manual packet input from QML to the decoding
 * @param source Source of the packet
 * @param packetUpperHexString The packet as an upper hex string
 */
void PacketDecoder::manualPacketInput(QString source, QString packetUpperHexString) {
    unsigned int realLength = static_cast<unsigned int>(packetUpperHexString.length() / 2);
    using namespace s1sync;
    OperatingMode opMode = OperatingMode::Invalid;
    bool needsDecoding = true;
    switch (realLength) {
    case 333:
        opMode = OperatingMode::AO40Short;
        break;
    case 650:
        opMode = OperatingMode::AO40;
        break;
    case 260:
        opMode = OperatingMode::RA_128_to_260;
        break;
    case 514:
        opMode = OperatingMode::RA_256_to_514;
        break;
    case 1028:
        opMode = OperatingMode::RA_512_to_1028;
        break;
    case 2050:
        opMode = OperatingMode::RA_1024_to_2050;
        break;
    case 4100:
        opMode = OperatingMode::RA_2048_to_4100;
        break;
    default:
        needsDecoding = false;
        break;
    }
    if (needsDecoding) {
        processSyncContents(5000, opMode);
        decodablePacketReceived(QDateTime::currentDateTimeUtc(), source, packetUpperHexString);
    }
    else {
        if (packetUpperHexString.length() == 0) {
            qWarning() << "manualPacketInput received packetUpperHexString with a length of 0 from " << source;
            return;
        }

        // Checking if packetUpperHexString really is an upper hex string
        if (!isUpperHexString(packetUpperHexString)) {
            qWarning() << "Received a non upperHexString as packetUpperHexString in manualPacketInput: "
                       << packetUpperHexString;
            return;
        }
        QByteArray received = QByteArray::fromHex(packetUpperHexString.toLocal8Bit());
        processDecodedPacket(QDateTime::currentDateTimeUtc(), source, "PRE-DECODED", received, 0);
    }
}

/**
 * @brief Takes an s1obc::SolarPanelTelemetryPacket and processes it, returning the resulting QString
 * @param packet The packet
 * @return The QString that represents the packet's contents in an easily readable format.
 */
QString PacketDecoder::processTelemetry1(const s1obc::SolarPanelTelemetryPacket &packet) {
    using namespace s1obc;
    Q_ASSERT(static_cast<char>(s1obc::DownlinkPacketType_Telemetry1) == packet.packetType());
    QString zeroth = getDTSFromUint32UTC(packet.timestamp()) + " === Telemetry1 ===";
    SolarPanelTelemetry spfront = packet.front();
    SolarPanelTelemetry spback = packet.back();
    SolarPanelTelemetry spright = packet.right();
    SolarPanelTelemetry spleft = packet.left();
    SolarPanelTelemetry sptop = packet.top();
    SolarPanelTelemetry spbottom = packet.bottom();

    QString first = QString(
        "--- Front " + getDTSFromUint32UTC(spfront.timestamp()) + " ---\n" + extractSolarPanelTelemetry(spfront));
    QString second =
        QString("--- Back " + getDTSFromUint32UTC(spback.timestamp()) + " ---\n" + extractSolarPanelTelemetry(spback));
    QString third = QString(
        "--- Right " + getDTSFromUint32UTC(spright.timestamp()) + " ---\n" + extractSolarPanelTelemetry(spright));
    QString fourth =
        QString("--- Left " + getDTSFromUint32UTC(spleft.timestamp()) + " ---\n" + extractSolarPanelTelemetry(spleft));
    QString fifth =
        QString("--- Top " + getDTSFromUint32UTC(sptop.timestamp()) + " ---\n" + extractSolarPanelTelemetry(sptop));
    QString sixth = QString(
        "--- Bottom " + getDTSFromUint32UTC(spbottom.timestamp()) + " ---\n" + extractSolarPanelTelemetry(spbottom));
    auto ack = packet.acknowledgedCommands();
    QString ackLine = "ACK: " + processAcknowledgedCommands(ack);
    QString allLines = zeroth + "\n\n" + first + "\n" + second + "\n" + third + "\n" + fourth + "\n" + fifth + "\n" +
                       sixth + "\n\n" + ackLine;
    return allLines;
}

/**
 * @brief Takes an s1obc::PcuTelemetryPacket and processes it, returning the resulting QString
 * @param packet The packet
 * @return The QString that represents the packet's contents in an easily readable format.
 */
QString PacketDecoder::processTelemetry2(const s1obc::PcuTelemetryPacket &packet) {
    using namespace s1obc;
    Q_ASSERT(static_cast<char>(s1obc::DownlinkPacketType_Telemetry2) == packet.packetType());
    QString zeroth = getDTSFromUint32UTC(packet.timestamp()) + " === Telemetry2 ===";
    PcuDeploymentTelemetry pcu1D = packet.deployment1();
    PcuDeploymentTelemetry pcu2D = packet.deployment2();
    PcuBatteryTelemetry pcu1Ba = packet.battery1();
    PcuBatteryTelemetry pcu2Ba = packet.battery2();
    PcuBusTelemetry pcu1Bu = packet.bus1();
    PcuBusTelemetry pcu2Bu = packet.bus2();
    PcuSdcTelemetry pcu1S = packet.sdc1();
    PcuSdcTelemetry pcu2S = packet.sdc2();

    QString first = QString(
        "--- PCU1Depl " + getDTSFromUint32UTC(pcu1D.timestamp()) + " ---\n" + extractPCUDeploymentTelemetry(pcu1D));
    QString second = QString(
        "--- PCU2Depl " + getDTSFromUint32UTC(pcu2D.timestamp()) + " ---\n" + extractPCUDeploymentTelemetry(pcu2D));
    QString third = QString(
        "--- PCU1Battery " + getDTSFromUint32UTC(pcu1Ba.timestamp()) + " ---\n" + extractPCUBatteryTelemetry(pcu1Ba));
    QString fourth = QString(
        "--- PCU2Battery " + getDTSFromUint32UTC(pcu2Ba.timestamp()) + " ---\n" + extractPCUBatteryTelemetry(pcu2Ba));
    QString fifth =
        QString("--- PCU1Bus " + getDTSFromUint32UTC(pcu1Bu.timestamp()) + " ---\n" + extractPCUBusTelemetry(pcu1Bu));
    QString sixth =
        QString("--- PCU2Bus " + getDTSFromUint32UTC(pcu2Bu.timestamp()) + " ---\n" + extractPCUBusTelemetry(pcu2Bu));
    QString seventh =
        QString("--- PCU1SDC " + getDTSFromUint32UTC(pcu1S.timestamp()) + " ---\n" + extractPCUSDCTelemetry(pcu1S));
    QString eighth =
        QString("--- PCU2SDC " + getDTSFromUint32UTC(pcu2S.timestamp()) + " ---\n" + extractPCUSDCTelemetry(pcu2S));
    auto ack = packet.acknowledgedCommands();
    QString ackLine = "ACK: " + processAcknowledgedCommands(ack);
    QString allLines = zeroth + "\n\n" + first + "\n" + second + "\n" + third + "\n" + fourth + "\n" + fifth + "\n" +
                       sixth + "\n" + seventh + "\n" + eighth + "\n\n" + ackLine;
    return allLines;
}

/**
 * @brief Takes an s1obc::PcuTelemetryPacket and processes it for ATL1, returning the resulting QString
 * @param packet The packet
 * @return The QString that represents the packet's contents in an easily readable format.
 */
QString PacketDecoder::processTelemetry2A(const s1obc::PcuTelemetryPacket &packet) {
    using namespace s1obc;
    Q_ASSERT(static_cast<char>(s1obc::DownlinkPacketType_Telemetry2) == packet.packetType());
    QString zeroth = getDTSFromUint32UTC(packet.timestamp()) + " === Telemetry2 ===";
    PcuDeploymentTelemetry pcu1D = packet.deployment1();
    PcuDeploymentTelemetry pcu2D = packet.deployment2();
    PcuBusTelemetry pcu1Bu = packet.bus1();
    PcuBusTelemetry pcu2Bu = packet.bus2();
    PcuSdcTelemetry pcu1S = packet.sdc1();
    PcuSdcTelemetry pcu2S = packet.sdc2();

    QString first = QString(
        "--- PCU1Depl " + getDTSFromUint32UTC(pcu1D.timestamp()) + " ---\n" + extractPCUDeploymentTelemetry(pcu1D));
    QString second = QString(
        "--- PCU2Depl " + getDTSFromUint32UTC(pcu2D.timestamp()) + " ---\n" + extractPCUDeploymentTelemetry(pcu2D));
    QString fifth =
        QString("--- PCU1Bus " + getDTSFromUint32UTC(pcu1Bu.timestamp()) + " ---\n" + extractPCUBusTelemetry(pcu1Bu));
    QString sixth =
        QString("--- PCU2Bus " + getDTSFromUint32UTC(pcu2Bu.timestamp()) + " ---\n" + extractPCUBusTelemetry(pcu2Bu));
    QString seventh =
        QString("--- PCU1SDC " + getDTSFromUint32UTC(pcu1S.timestamp()) + " ---\n" + extractPCUSDCTelemetry(pcu1S));
    QString eighth =
        QString("--- PCU2SDC " + getDTSFromUint32UTC(pcu2S.timestamp()) + " ---\n" + extractPCUSDCTelemetry(pcu2S));
    auto ack = packet.acknowledgedCommands();
    QString ackLine = "ACK: " + processAcknowledgedCommands(ack);
    QString allLines = zeroth + "\n\n" + first + "\n" + second + "\n" + fifth + "\n" + sixth + "\n" + seventh + "\n" +
                       eighth + "\n\n" + ackLine;
    return allLines;
}

/**
 * @brief Takes an s1obc::OnboardTelemetryPacket and processes it, returning the resulting QString
 * @param packet The packet
 * @return The QString that represents the packet's contents in an easily readable format.
 */
QString PacketDecoder::processTelemetry3(const s1obc::OnboardTelemetryPacket &packet) {
    using namespace s1obc;
    Q_ASSERT(static_cast<char>(s1obc::DownlinkPacketType_Telemetry3) == packet.packetType());
    QString zeroth = getDTSFromUint32UTC(packet.timestamp()) + " === Telemetry3 ===";
    ObcTelemetry obc = packet.obc();
    ComTelemetry com = packet.com();
    TidTelemetry tid1 = packet.tid1();
    TidTelemetry tid2 = packet.tid2();

    QString first = "--- OBC ---\n" + extractOBCTelemetry(obc);
    QString second = "--- COM " + getDTSFromUint32UTC(com.timestamp()) + " ---\n" + extractCOMTelemetry(com);
    QString third = "--- TID1 " + getDTSFromUint32UTC(tid1.timestamp()) + " ---\n" + extractTIDTelemetry(tid1);
    QString fourth = "--- TID2 " + getDTSFromUint32UTC(tid2.timestamp()) + " ---\n" + extractTIDTelemetry(tid2);
    auto ack = packet.acknowledgedCommands();
    QString ackLine = "ACK: " + processAcknowledgedCommands(ack);
    QString allLines = zeroth + "\n\n" + first + "\n" + second + "\n" + third + "\n" + fourth + "\n\n" + ackLine;
    return allLines;
}

/**
 * @brief Takes an s1obc::OnboardTelemetryPacketPA and processes it for SMOGP, returning the resulting QString
 * @param packet The packet
 * @return The QString that represents the packet's contents in an easily readable format.
 */
QString PacketDecoder::processTelemetry3P(const s1obc::OnboardTelemetryPacketPA &packet) {
    using namespace s1obc;
    Q_ASSERT(static_cast<char>(s1obc::DownlinkPacketType_Telemetry3) == packet.packetType());
    QString zeroth = getDTSFromUint32UTC(packet.timestamp()) + " === Telemetry3 ===";
    ObcTelemetryPA obc = packet.obc();
    ComTelemetry com = packet.com();
    TidTelemetry tid1 = packet.tid1();
    TidTelemetry tid2 = packet.tid2();

    QString first = "--- OBC ---\n" + extractOBCTelemetryPA(obc);
    QString second = "--- COM " + getDTSFromUint32UTC(com.timestamp()) + " ---\n" + extractCOMTelemetry(com);
    QString third = "--- TID1 " + getDTSFromUint32UTC(tid1.timestamp()) + " ---\n" + extractTIDTelemetry(tid1);
    QString fourth = "--- TID2 " + getDTSFromUint32UTC(tid2.timestamp()) + " ---\n" + extractTIDTelemetry(tid2);
    auto ack = packet.acknowledgedCommands();
    QString ackLine = "ACK: " + processAcknowledgedCommands(ack);
    QString allLines = zeroth + "\n\n" + first + "\n" + second + "\n" + third + "\n" + fourth + "\n\n" + ackLine;
    return allLines;
}

/**
 * @brief Takes an s1obc::OnboardTelemetryPacketPA and processes it for ATL1, returning the resulting QString
 * @param packet The packet
 * @return The QString that represents the packet's contents in an easily readable format.
 */
QString PacketDecoder::processTelemetry3A(const s1obc::OnboardTelemetryPacketPA &packet) {
    using namespace s1obc;
    Q_ASSERT(static_cast<char>(s1obc::DownlinkPacketType_Telemetry3) == packet.packetType());
    QString zeroth = getDTSFromUint32UTC(packet.timestamp()) + " === Telemetry3 ===";
    ObcTelemetryPA obc = packet.obc();
    ComTelemetry com = packet.com();

    QString first = "--- OBC ---\n" + extractOBCTelemetryPA(obc);
    QString second = "--- COM " + getDTSFromUint32UTC(com.timestamp()) + " ---\n" + extractCOMTelemetry(com);
    auto ack = packet.acknowledgedCommands();
    QString ackLine = "ACK: " + processAcknowledgedCommands(ack);
    QString allLines = zeroth + "\n\n" + first + "\n" + second + "\n\n" + ackLine;
    return allLines;
}

/**
 * @brief Takes an s1obc::BeaconPacket and processes it, returning the resulting QString
 * @param packet The packet
 * @return The QString that represents the packet's contents in an easily readable format.
 */
QString PacketDecoder::processBeacon(const s1obc::BeaconPacket &packet) {
    using namespace s1obc;
    Q_ASSERT(static_cast<char>(s1obc::DownlinkPacketType_Beacon) == packet.packetType());
    QString zeroth = getDTSFromUint32UTC(packet.timestamp()) + " === Beacon ===";
    DiagnosticInfo diagnostic = packet.diagnosticInfo();

    QString first = "--- DiagnosticInfo ---\n" + extractDiagnosticTelemetry(diagnostic);
    BeaconMessage beacon = packet.message();
    auto ack = packet.acknowledgedCommands();
    QString ackLine = "ACK: " + processAcknowledgedCommands(ack);
    QString allLines = zeroth + "\n\n" + QString(reinterpret_cast<const char *>(beacon.characters)) + "\n\n" + first +
                       "\n\n" + ackLine;
    return allLines;
}

/**
 * @brief Takes an s1obc::BeaconPacketPA and processes it, returning the resulting QString
 * @param packet The packet
 * @return The QString that represents the packet's contents in an easily readable format.
 */
QString PacketDecoder::processBeaconPA(const s1obc::BeaconPacketPA &packet) {
    using namespace s1obc;
    Q_ASSERT(static_cast<char>(s1obc::DownlinkPacketType_Beacon) == packet.packetType());
    QString zeroth = getDTSFromUint32UTC(packet.timestamp()) + " === Beacon ===";
    DiagnosticInfoPA diagnostic = packet.diagnosticInfo();

    QString first = "--- DiagnosticInfo ---\n" + extractDiagnosticTelemetryPA(diagnostic);
    BeaconMessage beacon = packet.message();
    auto ack = packet.acknowledgedCommands();
    QString ackLine = "ACK: " + processAcknowledgedCommands(ack);
    QString allLines = zeroth + "\n\n" + QString(reinterpret_cast<const char *>(beacon.characters)) + "\n\n" + first +
                       "\n\n" + ackLine;
    return allLines;
}

/**
 * @brief Takes an s1obc::SpectrumPacket and processes it, returning the resulting QString
 * @param packet The packet
 * @return The QString that represents the packet's contents in an easily readable format.
 */
QString PacketDecoder::processSpectrumData(const s1obc::SpectrumPacket &packet) {
    using namespace s1obc;
    Q_ASSERT(static_cast<char>(s1obc::DownlinkPacketType_SpectrumResult) == packet.packetType());
    QString zeroth = getDTSFromUint32UTC(packet.timestamp()) + " === Spectrum ===";
    uint8_t pi = packet.index();
    uint8_t pc = packet.count();

    emit newSpectrumPacket(packet);

    QString allLines = zeroth + "\n\n" + QString("Part: %1 / %2").arg(pi + 1).arg(pc);
    return allLines;
}

/**
 * @brief Takes an s1obc::FileDownloadPacket and processes it, returning the resulting QString
 * @param packet The packet
 * @return The QString that represents the packet's contents in an easily readable format.
 */
QString PacketDecoder::processFileDownload(const s1obc::FileDownloadPacket &packet) {
    using namespace s1obc;
    Q_ASSERT(static_cast<char>(s1obc::DownlinkPacketType_FileDownload) == packet.packetType());
    static constexpr size_t maxDataLength = FileDownloadData::maxLengthPerPacket;
    QString zeroth = getDTSFromUint32UTC(packet.timestamp()) + " === FileDownload ===";

    auto fe = packet.entry();
    auto pc = packet.count();
    auto pi = packet.index();

    // The second condition ensures that even if we miss the fragment with a 0 pi, we'll start a new file upon the
    // reception of a fragment
    if (pi == 0 || (fileDownload_priv.data() && fileDownload_priv.data()->isFinished)) {
        if (fileDownload_priv.data()) {
            if (!fileDownload_priv.data()->isFinished) {
                bool result = fileDownload_priv.data()
                                  ->finishUpFile(); // finishing up the previous file if it has not timed out yet
                if (result) {
                    // successful file download
                }
                else {
                    // a partial file may have been created
                }
            }
        }
        auto filename = QString(QByteArray(reinterpret_cast<const char *>(fe.FileEntry::name().characters), 10));
        auto fn = filename.left(
            filename.indexOf(QChar(static_cast<char>(0))) == -1 ? 10 : filename.indexOf(QChar(static_cast<char>(0))));
        if (!fileDownload_priv.isNull()) {
            fileDownload_priv.data()->disconnect();
        }
        fileDownload_priv.reset(
            new FileDownload(downloadDirString + prefix + "_" + fn, pc, (s1obc::FileType) fe.type()));
        QObject::connect(fileDownload_priv.data(),
            &FileDownload::newSpectrumFile,
            spectrumReceiver_priv,
            &SpectrumReceiver::newSpectrumFile);
    }
    uint32_t filesize = fe.FileEntry::size();
    auto downloadsize = std::min(filesize - (pi * maxDataLength), maxDataLength);
    QByteArray downloadbytes(reinterpret_cast<const char *>(packet.data().bytes), (int) downloadsize);

    fileDownload_priv.data()->addFragment(pi, downloadbytes);

    if (pi == pc - 1) {
        bool result = fileDownload_priv.data()->finishUpFile();
        if (result) {
            // successful file download
        }
        else {
            // a partial file may have been created
        }
    }

    QString allLines = zeroth + "\n\n" + fileEntryToQString(fe) + "\n" + QString("Part: %1 / %2").arg(pi + 1).arg(pc);
    return allLines;
}

/**
 * @brief Takes an s1obc::BatteryPacketA and processes it for ATL1, returning the resulting QString
 * @param packet The packet
 * @return The QString that represents the packet's contents in an easily readable format.
 */
QString PacketDecoder::processBattery(const s1obc::BatteryPacketA &packet) {
    using namespace s1obc;
    Q_ASSERT(static_cast<uint8_t>(s1obc::DownlinkPacketType_Telemetry3_A) == packet.packetType());
    QString zeroth = getDTSFromUint32UTC(packet.timestamp()) + " === Battery telemetry ===";
    BatteryTelemetryA panel1 = packet.panel1();
    BatteryTelemetryA panel2 = packet.panel2();
    BatteryTelemetryA panel3 = packet.panel3();
    BatteryTelemetryA panel4 = packet.panel4();

    QString first = getDTSFromUint32UTC(panel1.timestamp()) + "--- Panel 1 ---\n" + extractBatteryTelemetryA(panel1, 1);
    QString second =
        getDTSFromUint32UTC(panel2.timestamp()) + "--- Panel 2 ---\n" + extractBatteryTelemetryA(panel2, 2);
    QString third = getDTSFromUint32UTC(panel3.timestamp()) + "--- Panel 3 ---\n" + extractBatteryTelemetryA(panel3, 3);
    QString fourth =
        getDTSFromUint32UTC(panel4.timestamp()) + "--- Panel 4 ---\n" + extractBatteryTelemetryA(panel4, 4);
    QString allLines = zeroth + "\n\n" + first + "\n" + second + "\n" + third + "\n" + fourth + "\n";
    return allLines;
}

/**
 * @brief Creates the QString representation of a solar panel (s1obc::SolarPanelTelemetry)
 * @param panel The solar panel to process
 * @return The QString representation
 */
QString PacketDecoder::extractSolarPanelTelemetry(const s1obc::SolarPanelTelemetry &panel) {
    using namespace s1obc;
    QString ret;
    ret += "\tT[°C]:" + QString::number((panel.temperature_C10()) / 10.0);
    ret += " Ii[mA]:" + QString::number(panel.inputCurrent_mA());
    ret += " Io[mA]:" + QString::number(panel.outputCurrent_mA());
    ret += " Vi[mV]:" + QString::number(panel.inputVoltage_mV());
    ret += " Vo[mV]:" + QString::number(panel.outputVoltage_mV());
    ret += " LSR[mV]:" + QString::number(panel.lightSensorReading_mV());
    ret += " Status:" + QString::number(static_cast<uint8_t>(panel.status().antennaStatus()));
    return ret;
}

/**
 * @brief Processes acknowledged commands (s1obc::AcknowledgedCommands) and creates the QString representation
 * @param commands The commands to process
 * @return The QString representation
 */
QString PacketDecoder::processAcknowledgedCommands(const s1obc::AcknowledgedCommands &commands) {
    using namespace s1obc;
    QString ret;
    for (size_t i = 0; i < AcknowledgedCommands::max; i++) {
        auto ack = commands.commands[i];
        auto ackUplinkId = ack.commandId();
        newCommandId(ackUplinkId);
        ret += QString::number(ackUplinkId);
        ret += "(" + QString::number(commands.commands[i].receivedRssi()) + " dBm) ";
        emit newCommandAcknowledged(ackUplinkId, getDateTimeString(QDateTime::currentDateTimeUtc()));
    }
    return ret;
}

/**
 * @brief Creates the QString representation of a PCU deployment telemetry segment (s1obc::PcuDeploymentTelemetry)
 * @param panel The PCU deployment telemetry segment to process
 * @return The QString representation
 */
QString PacketDecoder::extractPCUDeploymentTelemetry(const s1obc::PcuDeploymentTelemetry &deployment) {
    using namespace s1obc;
    QString ret;
    ret += "\tDEPSW1:" + QString::number(deployment.status().switch1());
    ret += " DEPSW2:" + QString::number(deployment.status().switch2());
    ret += " RBF:" + QString::number(deployment.status().removeBeforeFlight());
    ret += " Deployment:" + QString::number(deployment.status().pcuDeploymentFlag());
    ret += " Antenna open:" + QString::number(deployment.status().pcuAntennaFlag());
    ret += " Boot counter:" + QString::number(deployment.pcuBootCounter());
    ret += " Uptime[m]:" + QString::number(deployment.pcuUptimeMinutes());
    return ret;
}

/**
 * @brief Creates the QString representation of a PCU battery telemetry segment (s1obc::PcuDeploymentTelemetry)
 * @param battery The PCU battery telemetry segment to process
 * @return The QString representation
 */
QString PacketDecoder::extractPCUBatteryTelemetry(const s1obc::PcuBatteryTelemetry &battery) {
    using namespace s1obc;
    QString ret;
    ret += "\tV[mV]:" + QString::number(battery.voltage_mV());
    ret += " Ic[mA]:" + QString::number(battery.chargeCurrent_mA());
    ret += " Idc[mA]:" + QString::number(battery.dischargeCurrent_mA());
    ret += " Charge:" + QString::number(battery.status().isChargeEnabled());
    ret += " Discharge:" + QString::number(battery.status().isDischargeEnabled());
    ret += "\n\tCharge Overcurrent:" + QString::number(battery.status().chargeOvercurrent());
    ret += " Charge Overvoltage:" + QString::number(battery.status().chargeOvervoltage());
    ret += " Discharge overcurrent:" + QString::number(battery.status().dischargeOvercurrent());
    ret += " Discharge overvoltage:" + QString::number(battery.status().dischargeOvervoltage());
    return ret;
}

/**
 * @brief Creates the QString representation of a PCU bus telemetry segment (s1obc::PcuDeploymentTelemetry)
 * @param battery The PCU bus telemetry segment to process
 * @return The QString representation
 */
QString PacketDecoder::extractPCUBusTelemetry(const s1obc::PcuBusTelemetry &bus) {
    using namespace s1obc;
    QString ret;
    ret += "\tRBV[mV]:" + QString::number(bus.regulatedBusVoltage_mV());
    ret += " URBV[mV]:" + QString::number(bus.unregulatedBusVoltage_mV());
    ret += " OBC1I[mA]:" + QString::number(bus.obc1CurrentConsumption_mA());
    ret += " OBC2I[mA]:" + QString::number(bus.obc2CurrentConsumption_mA());
    ret += "\n\tOBC1 Limiter overcurrent:" + QString::number(bus.status().obc1Overcurrent());
    ret += " OBC2 Limiter overcurrent:" + QString::number(bus.status().obc2Overcurrent());
    return ret;
}

/**
 * @brief Creates the QString representation of a PCU SDC telemetry segment (s1obc::PcuDeploymentTelemetry)
 * @param battery The PCU SDC telemetry segment to process
 * @return The QString representation
 */
QString PacketDecoder::extractPCUSDCTelemetry(const s1obc::PcuSdcTelemetry &sdc) {
    using namespace s1obc;
    QString ret;
    ret += "\t1Ii[mA]:" + QString::number(sdc.chain1InputCurrent_mA());
    ret += " 1Io[mA]:" + QString::number(sdc.chain1OutputCurrent_mA());
    ret += " 1Vo[mV]:" + QString::number(sdc.chain1OutputVoltage_mV());
    ret += " 2Ii[mA]:" + QString::number(sdc.chain2InputCurrent_mA());
    ret += " 2Io[mA]:" + QString::number(sdc.chain2OutputCurrent_mA());
    ret += " 2Vo[mV]:" + QString::number(sdc.chain2OutputVoltage_mV());
    ret += "\n\t1Overcurrent:" + QString::number(sdc.status().chain1Overcurrent());
    ret += " 1Overvoltage:" + QString::number(sdc.status().chain1Overvoltage());
    ret += " 2Overcurrent:" + QString::number(sdc.status().chain2Overcurrent());
    ret += " 2Overvoltage:" + QString::number(sdc.status().chain1Overvoltage());
    return ret;
}

/**
 * @brief Creates the QString representation of an OBC telemetry segment (s1obc::ObcTelemetry)
 * @param battery The OBC telemetry segment to process
 * @return The QString representation
 */
QString PacketDecoder::extractOBCTelemetry(const s1obc::ObcTelemetry &obc) {
    using namespace s1obc;
    QString ret;
    ret += "\tT[°C]:" + QString::number((obc.activeObcTemperature_C10()) / 10.0);
    ret += " V[mV]:" + QString::number(obc.supplyVoltage_mV());
    ret += " RTCC1T[°C]:" + QString::number((obc.rtcc1Temperature_C10()) / 10.0);
    ret += " RTCC2T[°C]:" + QString::number((obc.rtcc2Temperature_C10()) / 10.0);
    ret += " EPS2T1[°C]:" + QString::number((obc.eps2PanelATemperature1_C10()) / 10.0);
    ret += " EPS2T2[°C]:" + QString::number((obc.eps2PanelATemperature2_C10()) / 10.0);
    ret += "\n\tCOMItx[mA]:" + QString::number(obc.comTxCurrent_mA());
    ret += " COMIrx[mA]:" + QString::number(obc.comRxCurrent_mA());
    ret += " COMTXPL:" + QString::number(obc.comTxStatus().powerLevel(), 16);
    ret += " COMTXDR:" + QString::number(obc.comTxStatus().dataRate(), 16);
    ret += "\n\tMotion1:";
    ret += "\n\t\tGyroscopeX[°/sec]" + QString::number(obc.motionSensor1().gyroscope().x(), 10);
    ret += "\n\t\tGyroscopeY[°/sec]" + QString::number(obc.motionSensor1().gyroscope().y(), 10);
    ret += "\n\t\tGyroscopeZ[°/sec]" + QString::number(obc.motionSensor1().gyroscope().z(), 10);
    ret += "\n\t\tMagnetometerX[uT]" + QString::number(obc.motionSensor1().magnetometer().x(), 10);
    ret += "\n\t\tMagnetometerY[uT]" + QString::number(obc.motionSensor1().magnetometer().y(), 10);
    ret += "\n\t\tMagnetometerZ[uT]" + QString::number(obc.motionSensor1().magnetometer().z(), 10);
    ret += "\n\t\tAccelerometerX[g]" + QString::number(obc.motionSensor1().accelerometer().x(), 10);
    ret += "\n\t\tAccelerometerY[g]" + QString::number(obc.motionSensor1().accelerometer().y(), 10);
    ret += "\n\t\tAccelerometerZ[g]" + QString::number(obc.motionSensor1().accelerometer().z(), 10);
    ret += "\n\tMotion2:";
    ret += "\n\t\tGyroscopeX[°/sec]" + QString::number(obc.motionSensor2().gyroscope().x(), 10);
    ret += "\n\t\tGyroscopeY[°/sec]" + QString::number(obc.motionSensor2().gyroscope().y(), 10);
    ret += "\n\t\tGyroscopeZ[°/sec]" + QString::number(obc.motionSensor2().gyroscope().z(), 10);
    ret += "\n\t\tMagnetometerX[uT]" + QString::number(obc.motionSensor2().magnetometer().x(), 10);
    ret += "\n\t\tMagnetometerY[uT]" + QString::number(obc.motionSensor2().magnetometer().y(), 10);
    ret += "\n\t\tMagnetometerZ[uT]" + QString::number(obc.motionSensor2().magnetometer().z(), 10);
    ret += "\n\t\tAccelerometerX[g]" + QString::number(obc.motionSensor2().accelerometer().x(), 10);
    ret += "\n\t\tAccelerometerY[g]" + QString::number(obc.motionSensor2().accelerometer().y(), 10);
    ret += "\n\t\tAccelerometerZ[g]" + QString::number(obc.motionSensor2().accelerometer().z(), 10);
    ret += "\n\tOBC:" + QString::number(obc.obcBoardStatus().activeObc());
    ret += " COM:" + QString::number(obc.obcBoardStatus().activeCom());
    ret += " Flash1:" + QString::number(obc.obcBoardStatus().flash1Status());
    ret += " Flash2:" + QString::number(obc.obcBoardStatus().flash2Status());
    ret += " RTCC1:" + QString::number(obc.obcBoardStatus().rtcc1Status());
    ret += " RTCC2:" + QString::number(obc.obcBoardStatus().rtcc2Status());
    ret += " MS1:" + QString::number(obc.obcBoardStatus().motionSensor1Status());
    ret += " MS2:" + QString::number(obc.obcBoardStatus().motionSensor2Status());
    return ret;
}

/**
 * @brief Creates the QString representation of an OBC telemetry segment (s1obc::ObcTelemetryPA)
 * @param battery The OBC telemetry segment to process
 * @return The QString representation
 */
QString PacketDecoder::extractOBCTelemetryPA(const s1obc::ObcTelemetryPA &obc) {
    using namespace s1obc;
    QString ret;
    ret += " V[mV]:" + QString::number(obc.supplyVoltage_mV());
    ret += " RTCCT[°C]:" + QString::number(obc.rtccTemperature_C());
    ret += " EPS2T1[°C]:" + QString::number((obc.eps2PanelATemperature1_C10()) / 10.0);
    ret += " EPS2T2[°C]:" + QString::number((obc.eps2PanelATemperature2_C10()) / 10.0);
    ret += "\n\tCOMItx[mA]:" + QString::number(obc.comTxCurrent_mA());
    ret += " COMIrx[mA]:" + QString::number(obc.comRxCurrent_mA());
    ret += " COMTXDR:" + QString::number(obc.comTxStatus().dataRate(), 16);
    ret += "\n\tMotion:";
    ret += "\n\t\tGyroscopeX[°/sec]" + QString::number(obc.motionSensor().gyroscope().x(), 10);
    ret += "\n\t\tGyroscopeY[°/sec]" + QString::number(obc.motionSensor().gyroscope().y(), 10);
    ret += "\n\t\tGyroscopeZ[°/sec]" + QString::number(obc.motionSensor().gyroscope().z(), 10);
    ret += "\n\t\tMagnetometerX[uT]" + QString::number(obc.motionSensor().magnetometer().x(), 10);
    ret += "\n\t\tMagnetometerY[uT]" + QString::number(obc.motionSensor().magnetometer().y(), 10);
    ret += "\n\t\tMagnetometerZ[uT]" + QString::number(obc.motionSensor().magnetometer().z(), 10);
    ret += "\n\t\tAccelerometerX[g]" + QString::number(obc.motionSensor().accelerometer().x(), 10);
    ret += "\n\t\tAccelerometerY[g]" + QString::number(obc.motionSensor().accelerometer().y(), 10);
    ret += "\n\t\tAccelerometerZ[g]" + QString::number(obc.motionSensor().accelerometer().z(), 10);
    ret += "\n\tOBC:" + QString::number(obc.obcBoardStatus().activeObc());
    ret += " COM:" + QString::number(obc.obcBoardStatus().activeCom());
    ret += " Flash:" + QString::number(obc.obcBoardStatus().flashStatus());
    ret += " RTCC:" + QString::number(obc.obcBoardStatus().rtccStatus());
    ret += " MS:" + QString::number(obc.obcBoardStatus().motionSensorStatus());
    return ret;
}

/**
 * @brief Creates the QString representation of a COM telemetry segment (s1obc::ComTelemetry)
 * @param battery The COM telemetry segment to process
 * @return The QString representation
 */
QString PacketDecoder::extractCOMTelemetry(const s1obc::ComTelemetry &com) {
    using namespace s1obc;
    auto spStatus = com.spectrumAnalyzerStatus();
    QString ret;
    ret += "\tT[°C]:" + QString::number((com.activeComTemperature_C10()) / 10.0);
    ret += " V[mV]:" + QString::number(com.activeComVoltage_mV());
    ret += " LRSSI:" + QString::number(com.lastReceivedRssi());
    ret += " SWR:" + QString::number(com.swrBridgeReading());
    ret += " Spectrum:" + QString::number(spStatus & 0x01);
    ret += " Spectrum V[mV]:" + QString::number((spStatus >> 1) * 30);
    ret += " Spectrum T[°C]:" + QString::number((com.activeComSpectrumAnalyzerTemperature_C10()) / 10.0);
    return ret;
}

/**
 * @brief Creates the QString representation of a TID telemetry segment (s1obc::TidTelemetry)
 * @param tid The TID telemetry segment to process
 * @return The QString representation
 */
QString PacketDecoder::extractTIDTelemetry(const s1obc::TidTelemetry &tid) {
    using namespace s1obc;
    QString ret;
    ret += "\tT[°C]:" + QString::number((tid.temperature_C10()) / 10.0);
    ret += " Supply voltage[mV]:" + QString::number(tid.supplyVoltage_mV());
    ret += " R1[uV]:" + QString::number(tid.radfet1_uV());
    ret += " R2[uV]:" + QString::number(tid.radfet2_uV());
    ret += " SN:" + QString::number(tid.measurementSerial());
    return ret;
}

/**
 * @brief Creates the QString representation of a DiagnosticInfo telemetry segment (s1obc::DiagnosticInfo)
 * @param diagnostic The Diagnostic telemetry segment to process
 * @return The QString representation
 */
QString PacketDecoder::extractDiagnosticTelemetry(const s1obc::DiagnosticInfo &diagnostic) {
    using namespace s1obc;
    QString ret;
    ret += ";Flashchecksum:" + QString::number(diagnostic.flashChecksum());
    ret += ";LastUplinkTimestamp:" + getDTSFromUint32UTC(diagnostic.lastUplinkTimestamp());
    ret += ";OBCUptime[Min]:" + QString::number(diagnostic.obcUptimeMin());
    ret += ";COMUptime[Min]:" + QString::number(diagnostic.comUptimeMin());
    ret += ";TXVoltageDrop[mV]:" + QString::number((diagnostic.txVoltageDrop_10mV()) * 10.0);
    ret += ";TaskCount:" + QString::number(diagnostic.taskCount());
    ret += ";EnergyMode:" + QString::number(diagnostic.diagnosticStatus().energyMode());
    ret += ";TCXO works:" + QString::number(diagnostic.diagnosticStatus().tcxoWorks());
    return ret;
}

/**
 * @brief Creates the QString representation of a DiagnosticInfoPA telemetry segment (s1obc::DiagnosticInfoPA)
 * @param diagnostic The Diagnostic telemetry segment to process
 * @return The QString representation
 */
QString PacketDecoder::extractDiagnosticTelemetryPA(const s1obc::DiagnosticInfoPA &diagnostic) {
    using namespace s1obc;
    QString ret;
    ret += ";ValidPackets:" + QString::number(diagnostic.validPackets());
    ret += ";WrongSizedPackets:" + QString::number(diagnostic.wrongSizedPackets());
    ret += ";PacketsWithFailedGolay:" + QString::number(diagnostic.packetsWithFailedGolayDecoding());
    ret += ";PacketsWithWrongSignature:" + QString::number((diagnostic.packetsWithWrongSignature()) * 10.0);
    ret += ";PacketsWithInvalidSerial:" + QString::number(diagnostic.packetsWithInvalidSerialNumber());
    ret += ";UARTErrorCounter:" + QString::number(diagnostic.uartErrorCounter());
    return ret;
}

/**
 * @brief Creates the QString representation of a BatteryTelemetryA telemetry segment (s1obc::BatteryTelemetryA)
 * @param diagnostic The Diagnostic telemetry segment to process
 * @return The QString representation
 */
QString PacketDecoder::extractBatteryTelemetryA(const s1obc::BatteryTelemetryA &battery, uint8_t panel) {
    using namespace s1obc;
    QString ret;
    ret += "Valid:" + QString::number(battery.valid());
    ret += " ;One-wire bus:" + QString::number(battery.oneWireBus() + 1);
    ret += " ;Current [mA]:" + QString::number(battery.current_mA()[panel - 1].toInt());
    ret += " ;T1 [°C]:" + QString::number(battery.temperature1_mC()[panel - 1].toDouble() / 1000.0);
    ret += " ;T2 [°C]:" + QString::number(battery.temperature2_mC()[panel - 1].toDouble() / 1000.0);
    ret += " ;T3 [°C]:" + QString::number(battery.temperature3_mC()[panel - 1].toDouble() / 1000.0);
    ret += " ;T4 [°C]:" + QString::number(battery.temperature4_mC()[panel - 1].toDouble() / 1000.0);
    ret += " ;T5 [°C]:" + QString::number(battery.temperature5_mC()[panel - 1].toDouble() / 1000.0);
    return ret;
}

/**
 * @brief Instructs the receiver to wait for the sync packet
 */
void PacketDecoder::waitForSyncPacket() {
    emit stopSyncTimeoutTimer();
    emit newDataRate(1250);
    emit newPacketLength(s1sync::syncPacketLength);
}

void PacketDecoder::startSyncPacketTimeout(unsigned int datarateBPS, s1sync::OperatingMode operatingMode) {
    unsigned int MS = 0;
    using namespace s1sync;
    double datarateBytesPerSecond = datarateBPS / 8.0;
    unsigned int additional = 4;
    switch (operatingMode) {
    case OperatingMode::AO40:
        MS = static_cast<unsigned int>(1000 * (650 + additional) / datarateBytesPerSecond);
        break;
    case OperatingMode::AO40Short:
        MS = static_cast<unsigned int>(1000 * (333 + additional) / datarateBytesPerSecond);
        break;
    case OperatingMode::RA_128_to_260:
        MS = static_cast<unsigned int>(1000 * (260 + additional) / datarateBytesPerSecond);
        break;
    case OperatingMode::RA_256_to_514:
        MS = static_cast<unsigned int>(1000 * (514 + additional) / datarateBytesPerSecond);
        break;
    case OperatingMode::RA_512_to_1028:
        MS = static_cast<unsigned int>(1000 * (1028 + additional) / datarateBytesPerSecond);
        break;
    case OperatingMode::RA_1024_to_2050:
        MS = static_cast<unsigned int>(1000 * (2050 + additional) / datarateBytesPerSecond);
        break;
    case OperatingMode::RA_2048_to_4100:
        MS = static_cast<unsigned int>(1000 * (4100 + additional) / datarateBytesPerSecond);
        break;
    case OperatingMode::Receive:
    case OperatingMode::Invalid:
        break;
    }
    emit startSyncTimeoutTimer(200 + static_cast<int>(4 * MS));
}

/**
 * @brief Decodes \p encodedData with AO40 short and returns the result.
 * @param encodedData The QByteArray containing the encoded data.
 * @return Returns a DecodedPacket that contains the result of the operation.
 */
const DecodedPacket PacketDecoder::decodeWithAO40SHORT(const QByteArray encodedData) {
    uint8_t ao40short_bits[AO40SHORT_DOWNLINKLENGTH * 8];
    uint8_t ao40short_dec_data[AO40SHORT_DECDOWNLINKLENGTH];
    int8_t ao40short_error;

    if (encodedData.length() < AO40SHORT_DOWNLINKLENGTH) {
        return DecodedPacket(DecodedPacket::Failure, 0, nullptr, QByteArray());
    }

    int bi = 0;
    for (int i = 0; i < AO40SHORT_DOWNLINKLENGTH; i++) {
        int j;
        unsigned char t = 0x80;
        for (j = 0; j < 8; j++) {
            ao40short_bits[bi] = encodedData[i] & t ? 255 : 0;
            t >>= 1;
            bi++;
        }
    }

    // bits size doesn't match decode_short_data input
    // AO40 short decoding
    ao40short_decode_data(ao40short_bits, ao40short_dec_data, &ao40short_error);
    QByteArray decoded = QByteArray(reinterpret_cast<char *>(ao40short_dec_data), AO40SHORT_DECDOWNLINKLENGTH);

    return DecodedPacket(
        (ao40short_error == -1) ? DecodedPacket::Failure : DecodedPacket::Success, 1, &ao40short_error, decoded);
}

/**
 * @brief Decodes \p encodedData with AO40 long and returns the result.
 * @param encodedData The QByteArray containing the encoded data.
 * @return Returns a DecodedPacket that contains the result of the operation.
 */
const DecodedPacket PacketDecoder::decodeWithAO40LONG(const QByteArray encodedData) {
    uint8_t ao40_bits[AO40_DOWNLINKLENGTH * 8];
    uint8_t ao40_dec_data[AO40_DECDOWNLINKLENGTH];
    int8_t ao40_error[2];

    if (encodedData.length() < AO40_DOWNLINKLENGTH) {
        return DecodedPacket(DecodedPacket::Failure, 0, nullptr, QByteArray());
    }

    int bi = 0;
    for (int i = 0; i < AO40_DOWNLINKLENGTH; i++) {
        int j;
        unsigned char t = 0x80;
        for (j = 0; j < 8; j++) {
            ao40_bits[bi] = encodedData[i] & t ? 255 : 0;
            t >>= 1;
            bi++;
        }
    }

    // AO40 long decoding
    ao40_decode_data(ao40_bits, ao40_dec_data, ao40_error);
    QByteArray decoded = QByteArray(reinterpret_cast<char *>(ao40_dec_data), AO40_DECDOWNLINKLENGTH);

    return DecodedPacket(
        (ao40_error[0] == -1 || ao40_error[1] == -1) ? (DecodedPacket::Failure) : (DecodedPacket::Success),
        2,
        ao40_error,
        decoded);
}

static int findRacoderInputLength(int encodedLength) {
    if (!encodedLength) {
        return 0;
    }

    // The input length is a little less than, or maybe equal to, half of the encoded length
    encodedLength >>= 1;

    // We know that SMOG only uses racoder with power-of-2 sizes, so we find the closest
    int ret = 1;
    while (encodedLength >>= 1) {
        ret <<= 1;
    }
    return ret;
}

/**
 * @brief Decodes \p encodedData with RA and returns the result.
 * @param encodedData The QByteArray containing the encoded data.
 * @return Returns a DecodedPacket that contains the result of the operation.
 */
const DecodedPacket PacketDecoder::decodeWithRA(const QByteArray encodedData) {
    int inputLength = findRacoderInputLength(encodedData.length());
    ra_index_t ra_length = static_cast<ra_index_t>(inputLength) / sizeof(ra_word_t);
    ra_length_init(ra_length);

    int encodedSize = ra_code_length * sizeof(ra_word_t);
    if (encodedSize != encodedData.length()) {
        qWarning() << "mismatching packet size for racoder: expected:" << encodedSize
                   << "actual:" << encodedData.length();
        return DecodedPacket(DecodedPacket::Failure, 0, nullptr, QByteArray());
    }

    const ra_word_t *ra_encoded_words = reinterpret_cast<const ra_word_t *>(encodedData.data());
    std::vector<float> ra_encoded_bits(ra_code_length * RA_BITCOUNT, 0.0f);
    std::vector<ra_word_t> ra_decoded_data(ra_length, 0);

    for (ra_index_t i = 0; i < ra_code_length; i++) {
        ra_word_t word = ra_encoded_words[i];
        for (int j = 0; j < RA_BITCOUNT; j++) {
            if ((word & (1 << j)) == 0) {
                ra_encoded_bits[RA_BITCOUNT * i + j] = 1.0;
            }
            else {
                ra_encoded_bits[RA_BITCOUNT * i + j] = -1.0;
            }
        }
    }

    ra_decoder_gen(ra_encoded_bits.data(), ra_decoded_data.data(), 20);

    QByteArray decoded = QByteArray(reinterpret_cast<char *>(ra_decoded_data.data()), inputLength);
    uint8_t packetType = static_cast<uint8_t>(decoded.at(0));
    if ((packetType >= 1 && packetType <= 7) ||
        (currentSatellite == SatelliteChanger::Satellites::SMOGP && packetType >= 33 && packetType <= 34) ||
        (currentSatellite == SatelliteChanger::Satellites::ATL1 && packetType >= 129 && packetType <= 131)) {
        return DecodedPacket(DecodedPacket::Success, 0, nullptr, decoded);
    }
    else {
        // Packet type is invalid so even though decoding was successful, input was incorrect
        return DecodedPacket(DecodedPacket::Failure, 0, nullptr, QByteArray());
    }
}

/**
 * @brief Returns \p datetime as a formatted QString
 * @param datetime the QDateTime object that will be formatted
 * @return A QString containing \p datetime in a formatted manner
 */
const QString PacketDecoder::getDateTimeString(QDateTime datetime) const {
    return datetime.toString(Qt::ISODate);
}

/**
 * @brief Processes a decoded packet: Updates the UI, emits other signals
 * @param timestamp Timestamp of reception
 * @param source Source of the packet
 * @param encoding Encoding of the packet
 * @param decodedPacket The QByteArray containing the decoded data
 * @param rssi The RSSI that the packet was received with
 */
void PacketDecoder::processDecodedPacket(
    const QDateTime &timestamp, const QString &source, const QString &encoding, QByteArray &decodedPacket, int rssi) {
    emit stopSyncTimeoutTimer();
    unsigned int packetTypeSize = 0;
    using namespace s1obc;
    uint8_t packetType = static_cast<uint8_t>(decodedPacket.at(0));
    switch (packetType) {
    case DownlinkPacketType_Telemetry1:
        packetTypeSize = sizeof(SolarPanelTelemetryPacket);
        break;
    case DownlinkPacketType_Telemetry2:
        packetTypeSize = sizeof(PcuTelemetryPacket);
        break;
    case DownlinkPacketType_Telemetry3:
        packetTypeSize = sizeof(OnboardTelemetryPacket);
        break;
    case DownlinkPacketType_Beacon:
        packetTypeSize = sizeof(BeaconPacket);
        break;
    case DownlinkPacketType_SpectrumResult:
        packetTypeSize = sizeof(SpectrumPacket);
        break;
    case DownlinkPacketType_FileDownload:
        packetTypeSize = sizeof(FileDownloadPacket);
        break;
    case DownlinkPacketType_FileInfo:
    case DownlinkPacketType_Telemetry1_B:
    case DownlinkPacketType_Telemetry2_B:
    case DownlinkPacketType_Telemetry1_A:
    case DownlinkPacketType_Telemetry2_A:
    case DownlinkPacketType_Telemetry3_A:
        packetTypeSize = 128;
        break;
    default:
        // Invalid packet type
        qInfo() << "Invalid packet type";
        startSyncPacketTimeout(dataRate_priv, decodeMode_priv);
        return;
    }

    if (static_cast<unsigned int>(decodedPacket.length()) < packetTypeSize) {
        // Packet was too short for its type
        qInfo() << "Packet was too short for its type";
        startSyncPacketTimeout(dataRate_priv, decodeMode_priv);
        return;
    }
    else if (static_cast<unsigned int>(decodedPacket.length()) > packetTypeSize) {
        // Packet was too long for its type
        qInfo() << "Packet was too long for its type";
        startSyncPacketTimeout(dataRate_priv, decodeMode_priv);
        return;
    }
#ifdef CHECK_SIGNATURE
    if (!checkSignature(decodedPacket)) {
        // signature mismatch
        startSyncPacketTimeout(dataRate_priv, decodeMode_priv);
        return;
    }
#endif // CHECK_SIGNATURE
    constexpr size_t signatureLength = sizeof(s1obc::DownlinkSignature);
    if (DownlinkPacketType_Telemetry1 == packetType) {
        SolarPanelTelemetryPacket p;
        p.loadBinary(reinterpret_cast<uint8_t *>(decodedPacket.data()));
        QByteArray auth = QByteArray(reinterpret_cast<char *>(p.signature().bytes), sizeof(DownlinkSignature));
        if (currentSatellite == SatelliteChanger::Satellites::ATL1) {
            // Front and left panels are swapped in ATL
            auto old_left = p.left();
            p.setLeft(p.front());
            p.setFront(old_left);
        }
        QString readableQString = processTelemetry1(p);
        packetSuccessfullyDecoded(timestamp,
            source,
            "Telemetry 1/4",
            encoding,
            auth,
            decodedPacket,
            readableQString,
            QVariant::fromValue(p),
            rssi);
    }
    else if (DownlinkPacketType_Telemetry2 == packetType) {
        switch (currentSatellite) {
        case SatelliteChanger::Satellites::SMOG1:
        case SatelliteChanger::Satellites::SMOGP: {
            PcuTelemetryPacket p;
            p.loadBinary(reinterpret_cast<uint8_t *>(decodedPacket.data()));
            QByteArray auth = QByteArray(reinterpret_cast<char *>(p.signature().bytes), sizeof(DownlinkSignature));
            QString readableQString = processTelemetry2(p);
            packetSuccessfullyDecoded(timestamp,
                source,
                "Telemetry 2/4",
                encoding,
                auth,
                decodedPacket,
                readableQString,
                QVariant::fromValue(p),
                rssi);
            break;
        }
        case SatelliteChanger::Satellites::ATL1: {
            PcuTelemetryPacket pa;
            pa.loadBinary(reinterpret_cast<uint8_t *>(decodedPacket.data()));
            QByteArray autha = QByteArray(reinterpret_cast<char *>(pa.signature().bytes), sizeof(DownlinkSignature));
            QString readableQStringa = processTelemetry2A(pa);
            packetSuccessfullyDecoded(timestamp,
                source,
                "Telemetry 2/4",
                encoding,
                autha,
                decodedPacket,
                readableQStringa,
                QVariant::fromValue(pa),
                rssi);
            break;
        }
        }
    }
    else if (DownlinkPacketType_Telemetry3 == packetType) {
        switch (currentSatellite) {
        case SatelliteChanger::Satellites::SMOG1: {
            OnboardTelemetryPacket p;
            p.loadBinary(reinterpret_cast<uint8_t *>(decodedPacket.data()));
            QByteArray auth = QByteArray(reinterpret_cast<char *>(p.signature().bytes), sizeof(DownlinkSignature));
            QString readableQString = processTelemetry3(p);
            packetSuccessfullyDecoded(timestamp,
                source,
                "Telemetry 3/4",
                encoding,
                auth,
                decodedPacket,
                readableQString,
                QVariant::fromValue(p),
                rssi);
            break;
        }
        case SatelliteChanger::Satellites::SMOGP: {
            OnboardTelemetryPacketPA pp;
            pp.loadBinary(reinterpret_cast<uint8_t *>(decodedPacket.data()));
            QByteArray authp = QByteArray(reinterpret_cast<char *>(pp.signature().bytes), sizeof(DownlinkSignature));
            QString readableQStringp = processTelemetry3P(pp);
            packetSuccessfullyDecoded(timestamp,
                source,
                "Telemetry 3/4",
                encoding,
                authp,
                decodedPacket,
                readableQStringp,
                QVariant::fromValue(pp),
                rssi);
            break;
        }
        case SatelliteChanger::Satellites::ATL1: {
            OnboardTelemetryPacketPA pa;
            pa.loadBinary(reinterpret_cast<uint8_t *>(decodedPacket.data()));
            QByteArray autha = QByteArray(reinterpret_cast<char *>(pa.signature().bytes), sizeof(DownlinkSignature));
            QString readableQStringa = processTelemetry3A(pa);
            packetSuccessfullyDecoded(timestamp,
                source,
                "Telemetry 3/4",
                encoding,
                autha,
                decodedPacket,
                readableQStringa,
                QVariant::fromValue(pa),
                rssi);
            break;
        }
        }
    }
    else if (DownlinkPacketType_Beacon == packetType) {
        waitForSyncPacket();
        switch (currentSatellite) {
        case SatelliteChanger::Satellites::SMOG1: {
            BeaconPacket p;
            p.loadBinary(reinterpret_cast<uint8_t *>(decodedPacket.data()));
            QByteArray auth = QByteArray(reinterpret_cast<char *>(p.signature().bytes), sizeof(DownlinkSignature));
            QString readableQString = processBeacon(p);
            packetSuccessfullyDecoded(timestamp,
                source,
                "Telemetry 4/4",
                encoding,
                auth,
                decodedPacket,
                readableQString,
                QVariant::fromValue(p),
                rssi);
            break;
        }
        case SatelliteChanger::Satellites::SMOGP:
        case SatelliteChanger::Satellites::ATL1: {
            BeaconPacketPA ppa;
            ppa.loadBinary(reinterpret_cast<uint8_t *>(decodedPacket.data()));
            QByteArray authpa = QByteArray(reinterpret_cast<char *>(ppa.signature().bytes), sizeof(DownlinkSignature));
            QString readableQStringpa = processBeaconPA(ppa);
            packetSuccessfullyDecoded(timestamp,
                source,
                "Telemetry 4/4",
                encoding,
                authpa,
                decodedPacket,
                readableQStringpa,
                QVariant::fromValue(ppa),
                rssi);
            break;
        }
        }
    }
    else if (DownlinkPacketType_SpectrumResult == packetType) {
        SpectrumPacket p;
        p.loadBinary(reinterpret_cast<uint8_t *>(decodedPacket.data()));
        QByteArray auth = QByteArray(reinterpret_cast<char *>(p.signature().bytes), sizeof(DownlinkSignature));
        QString readableQString = processSpectrumData(p);
        QString packetName = QStringLiteral("Spectrum data ") + QString::number(p.index() + 1) + QStringLiteral("/") +
                             QString::number(p.count());
        packetSuccessfullyDecoded(timestamp,
            source,
            packetName,
            encoding,
            auth,
            decodedPacket,
            readableQString,
            QVariant::fromValue(p),
            rssi);
        auto pi = p.index();
        auto pc = p.count();
        if (pi == pc - 1) {
            waitForSyncPacket();
            return; // Returning so that the sync packet timer does not restart
        }
    }
    else if (DownlinkPacketType_FileDownload == packetType) {
        FileDownloadPacket p;
        p.loadBinary(reinterpret_cast<uint8_t *>(decodedPacket.data()));
        QByteArray auth = QByteArray(reinterpret_cast<char *>(p.signature().bytes), sizeof(DownlinkSignature));
        QString readableQString = processFileDownload(p);
        QString packetName = QStringLiteral("File download ") + QString::number(p.index() + 1) + QStringLiteral("/") +
                             QString::number(p.count());
        packetSuccessfullyDecoded(timestamp,
            source,
            packetName,
            encoding,
            auth,
            decodedPacket,
            readableQString,
            QVariant::fromValue(p),
            rssi);
        auto pc = p.count();
        auto pi = p.index();
        if (pi == pc - 1) {
            waitForSyncPacket();
            return; // Returning so that the sync packet timer does not restart
        }
    }
    else if (DownlinkPacketType_FileInfo == packetType) {
        if (currentSatellite == SatelliteChanger::Satellites::ATL1 ||
            currentSatellite == SatelliteChanger::Satellites::SMOGP) {
            QByteArray auth = decodedPacket.right(signatureLength);
            QString packetName = QStringLiteral("File info");
            packetSuccessfullyDecoded(timestamp,
                source,
                packetName,
                encoding,
                auth,
                decodedPacket,
                "",
                QVariant::fromValue(false),
                rssi);
        }
        else {
            // Invalid packet type for satellite
        }
    }
    else if (DownlinkPacketType_Telemetry1_B == packetType) {
        if (currentSatellite == SatelliteChanger::Satellites::SMOGP) {
            QByteArray auth = decodedPacket.right(signatureLength);
            QString packetName = QStringLiteral("SMOG-P - Telemetry 1/2");
            packetSuccessfullyDecoded(timestamp,
                source,
                packetName,
                encoding,
                auth,
                decodedPacket,
                "",
                QVariant::fromValue(false),
                rssi);
        }
        else {
            // Invalid packet type for satellite
        }
    }
    else if (DownlinkPacketType_Telemetry2_B == packetType) {
        if (currentSatellite == SatelliteChanger::Satellites::SMOGP) {
            QByteArray auth = decodedPacket.right(signatureLength);
            QString packetName = QStringLiteral("SMOG-P - Telemetry 2/2");
            packetSuccessfullyDecoded(timestamp,
                source,
                packetName,
                encoding,
                auth,
                decodedPacket,
                "",
                QVariant::fromValue(false),
                rssi);
        }
        else {
            // Invalid packet type for satellite
        }
    }
    else if (DownlinkPacketType_Telemetry1_A == packetType) {
        if (currentSatellite == SatelliteChanger::Satellites::ATL1) {
            QByteArray auth = decodedPacket.right(signatureLength);
            QString packetName = QStringLiteral("ATL-1 - Telemetry 1/3");
            packetSuccessfullyDecoded(timestamp,
                source,
                packetName,
                encoding,
                auth,
                decodedPacket,
                "",
                QVariant::fromValue(false),
                rssi);
        }
        else {
            // Invalid packet type for satellite
        }
    }
    else if (DownlinkPacketType_Telemetry2_A == packetType) {
        if (currentSatellite == SatelliteChanger::Satellites::ATL1) {
            QByteArray auth = decodedPacket.right(signatureLength);
            QString packetName = QStringLiteral("ATL-1 - Telemetry 2/3");
            packetSuccessfullyDecoded(timestamp,
                source,
                packetName,
                encoding,
                auth,
                decodedPacket,
                "",
                QVariant::fromValue(false),
                rssi);
        }
        else {
            // Invalid packet type for satellite
        }
    }
    else if (DownlinkPacketType_Telemetry3_A == packetType) {
        if (currentSatellite == SatelliteChanger::Satellites::ATL1) {
            BatteryPacketA p;
            p.loadBinary(reinterpret_cast<uint8_t *>(decodedPacket.data()));
            QByteArray authp = QByteArray(reinterpret_cast<char *>(p.signature().bytes), sizeof(DownlinkSignature));
            QString readableQString = processBattery(p);
            packetSuccessfullyDecoded(timestamp,
                source,
                "ATL-1 - Telemetry 3/3",
                encoding,
                authp,
                decodedPacket,
                readableQString,
                QVariant::fromValue(p),
                rssi);
        }
        else {
            // Invalid packet type for satellite
        }
    }
    startSyncPacketTimeout(dataRate_priv, decodeMode_priv);
}

/**
 * @brief Called whenever a new packet has been decoded. Emits the packet data (that is received by PacketTableModel and
 * Uploader). Eliminates duplicates from the same source (e.g. SDR demodulates the same packet twice)
 * @param timestamp Timestamp of the packet
 * @param source Source of the packet
 * @param type Type of the packet
 * @param encoding Encoding of the packet
 * @param auth Authentication segment of the packet as a QByteArray (encapsulating a char array)
 * @param decodedData Complete decoded data as a QByteArray (encapsulating a char array)
 * @param readableQString The packets contents in an easily readable format
 * @param rssi The RSSI that the packet was received with
 */
void PacketDecoder::packetSuccessfullyDecoded(QDateTime timestamp,
    QString source,
    QString type,
    QString encoding,
    QByteArray auth,
    QByteArray decodedData,
    QString readableQString,
    QVariant packet,
    int rssi) {

    QString decodedQString = QString(decodedData.toHex()).toUpper();
    QString authQString = QString(auth.toHex()).toUpper();
    if (type.contains("Sync") || recentPackets_priv.take(source) != authQString) {
        QString satelliteQString;
        switch (currentSatellite) {
        case SatelliteChanger::Satellites::SMOG1:
            satelliteQString = "SMOG-1";
            break;
        case SatelliteChanger::Satellites::SMOGP:
            satelliteQString = "SMOG-P";
            break;
        case SatelliteChanger::Satellites::ATL1:
            satelliteQString = "ATL-1";
            break;
        case SatelliteChanger::Satellites::UNSET:
            satelliteQString = "UNSET";
            break;
        }
        emit newPacket(getDateTimeString(timestamp),
            source,
            type,
            encoding,
            authQString,
            satelliteQString,
            decodedQString,
            readableQString,
            packet,
            rssi);
        if (!(type.contains("Sync") || source == "QMLMANUAL" ||
                currentSatellite == SatelliteChanger::Satellites::UNSET)) {
            emit newPacketMinimal(decodedQString, this->currentSatellite);
        }
    }
    recentPackets_priv[source] = authQString;
}

/**
 * @brief Processes the contents of a sync packet
 * @param datarateBPS The datarate that the sync packet contained
 * @param operatingMode The operating mode that the sync packet contained
 */
void PacketDecoder::processSyncContents(unsigned int datarateBPS, s1sync::OperatingMode operatingMode) {
    if (operatingMode == s1sync::OperatingMode::Invalid) {
        return;
    }
    if (operatingMode == s1sync::OperatingMode::Receive) {
        // send commands
        emit sendCommands(500, 650, 9500);
        waitForSyncPacket();
        return;
    }
    emit newDataRate(datarateBPS);
    dataRate_priv = datarateBPS;
    decodeMode_priv = operatingMode;
    using namespace s1sync;
    switch (operatingMode) {
    case OperatingMode::AO40Short:
        emit newPacketLength(333);
        break;
    case OperatingMode::AO40:
        emit newPacketLength(650);
        break;
    case OperatingMode::RA_128_to_260:
        emit newPacketLength(260);
        break;
    case OperatingMode::RA_256_to_514:
        emit newPacketLength(514);
        break;
    case OperatingMode::RA_512_to_1028:
        emit newPacketLength(1028);
        break;
    case OperatingMode::RA_1024_to_2050:
        emit newPacketLength(2050);
        break;
    case OperatingMode::RA_2048_to_4100:
        emit newPacketLength(4100);
        break;
    default:
        break;
    }
    startSyncPacketTimeout(datarateBPS, operatingMode);
}

/**
 * @brief Returns whether \p input is an uppercase string only containing 0-9A-F
 * @param input The input QString
 * @return True if \p input fits the criteria.
 */
bool PacketDecoder::isUpperHexString(QString input) const {
    QString pattern("[0-9A-F]+");
    QRegularExpression re(pattern);
    QRegularExpressionMatch match = re.match(input, 0, QRegularExpression::NormalMatch);
    return (match.hasMatch() && QString::compare(match.captured(0), input) == 0);
}

#ifdef CHECK_SIGNATURE
/**
 * @brief Calculates the signature for \p data and checks it against \p signature.
 *
 * Uses \p to check the signature for the specific satellite. Defaults to false if any error occurs.
 *
 * @param data The data that is checked
 * @return True if the \p signature matches the calculated signature for \p data
 */
bool PacketDecoder::checkSignature(const QByteArray &packet) const {
    // Not included in this version
    Q_UNUSED(packet)
    return true;
}
#endif // CHECK_SIGNATURE

/**
 * @brief Sets \p recentPackets_priv to \p newId and emits PacketDecoder::newCommandIdSignal() if necessary
 * @param newId The new id value
 */
void PacketDecoder::newCommandId(uint16_t newId) {
    if (newId >= lastCommandId_priv) {
        lastCommandId_priv = newId;
        emit newCommandIdSignal(lastCommandId_priv);
    }
    // we may need to deal with overflow if necessary
}

/**
 * @brief Turns a FileType to QString
 * @param type the type
 * @return The QString describing \p type
 */
QString PacketDecoder::fileTypeToQString(s1obc::FileType type) {
    switch (type) {
    case s1obc::FileType_Deleted:
        return "Deleted";
    case s1obc::FileType_Text:
        return "Text";
    case s1obc::FileType_Custom:
        return "Custom";
    case s1obc::FileType_Spectrum:
        return "Spectrum";
    case s1obc::FileType_UniversalMeasurement:
        return "UniversalMeasurement";
    default:
        return "UNKNOWN";
    }
}

/**
 * @brief Turns a FileEntry to a QString
 * @param entry the file entry
 * @return The QString describing \p entry
 */
QString PacketDecoder::fileEntryToQString(s1obc::FileEntry entry) {
    using namespace s1utils;
    QString ret;
    auto filename = QString(QByteArray(reinterpret_cast<const char *>(entry.name().characters), 10));
    auto fn = filename.left(
        filename.indexOf(QChar(static_cast<char>(0))) == -1 ? 10 : filename.indexOf(QChar(static_cast<char>(0))));
    ret.append("name: ").append(fn);
    ret.append(", type: ").append(fileTypeToQString((s1obc::FileType) entry.type()));
    ret.append(QString(", size: %1").arg(static_cast<uint32_t>(entry.size())));
    ret.append(", timestamp: ").append(getDTSFromUint32UTC(entry.timestamp()));
    return ret;
}

/**
 * @brief Turns a file (FileId + FileEntry) to a QString
 * @param id id of the file
 * @param entry file entry
 * @return QString describing the file
 */
QString PacketDecoder::fileToQString(uint8_t id, s1obc::FileEntry entry) {
    using namespace s1utils;
    if (id != 0) {
        QString ret = QString("ID: %1").arg(id);
        ret.append(" " + fileEntryToQString(entry));
        return ret;
    }
    else {
        return QString("File is missing");
    }
}

/**
 * @brief A slot, that performs decoding on \p packet.
 *
 * Handles different types of packets accordingly.
 *
 * @param timestamp Timestamp of reception
 * @param source Source of the packet
 * @param packetUpperHexString QString that represents the data contained in the packet as an UPPERCASE Hex QString.
 * @param The RSSI that the packet was received with
 */
void PacketDecoder::decodablePacketReceivedWithRssi(QDateTime timestamp,
    QString source,
    QString packetUpperHexString,
    int rssi) {

    if (packetUpperHexString.length() == 0) {
        qWarning() << "PacketDecoder received packetUpperHexString with a length of 0 from " << source;
        return;
    }

    // Checking if packetUpperHexString really is an upper hex string
    if (!isUpperHexString(packetUpperHexString)) {
        qWarning() << "Received a non upperHexString as packetUpperHexString in decodablePacketReceived: "
                   << packetUpperHexString;
        return;
    }
    // Logging to file
    QFile packetFile(this->logDirString + this->prefix + "_" + this->fileName);
    packetFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text);
    if (packetFile.isOpen()) {
        QTextStream writer(&packetFile);
        writer << "Timestamp: " << getDateTimeString(timestamp) << "\tSource: " << source
               << "\tENCODED data: " << packetUpperHexString << "\n";
        writer.flush();
        packetFile.close();
    }
    else {
        qWarning() << "PacketDecoder could not open the file for writing";
    }
    QByteArray received = QByteArray::fromHex(packetUpperHexString.toLocal8Bit());
    if (received.length() == s1sync::syncPacketLength) {
        // Validate sync packet
        unsigned syncErrors = 0;
        const char *syncStartBytes = reinterpret_cast<const char *>(s1sync::syncBytes);
        for (unsigned i = 0; i < sizeof(s1sync::syncBytes); i++) {
            char expected = syncStartBytes[i];
            char actual = received.at(i);
            if (actual != expected) {
                syncErrors += __builtin_popcount(actual ^ expected);
            }
        }

        if (syncErrors > 200) {
            qWarning() << "too many bit errors in sync packet:" << syncErrors;
            qWarning() << received.toHex();
            waitForSyncPacket();
            return;
        }

        auto res = s1sync::getSyncContents(received);
        processSyncContents(res.first, res.second);

        QDateTime datetime = QDateTime::currentDateTimeUtc();
        QString opmode = s1sync::operatingModeToQString(res.second);
        QString readableQString = "=== Sync packet ===\n\n" + QString::number(res.first) + " bps, mode: " + opmode;
        QString packetName;
        if (res.second == s1sync::OperatingMode::Receive) {
            packetName = QStringLiteral("RX Sync");
        }
        else {
            packetName = QStringLiteral("TX Sync");
        }

        qInfo() << readableQString;
        packetSuccessfullyDecoded(datetime,
            source,
            packetName,
            QStringLiteral("-"),
            QByteArray(),
            received,
            readableQString,
            QVariant(),
            rssi);
        return;
    }
    using namespace s1sync;
    switch (decodeMode_priv) {
    case OperatingMode::AO40:
        if (received.length() != 650) {
            qWarning() << "Wrong packet length for AO40:" << received.length();
        }
        else {
            DecodedPacket ao40_result = decodeWithAO40LONG(received);
            if (ao40_result.getResult() == DecodedPacket::Success) {
                QByteArray decoded = ao40_result.getDecodedPacket();
                processDecodedPacket(timestamp, source, "AO40", decoded, rssi);
            }
            else {
                qWarning() << "Failed AO40 decoding";
            }
        }
        break;
    case OperatingMode::AO40Short:
        if (received.length() != 333) {
            qWarning() << "Wrong packet length for AO40Short:" << received.length();
        }
        else {
            DecodedPacket ao40short_result = decodeWithAO40SHORT(received);
            if (ao40short_result.getResult() == DecodedPacket::Success) {
                QByteArray decoded = ao40short_result.getDecodedPacket();
                processDecodedPacket(timestamp, source, "AO40Short", decoded, rssi);
            }
            else {
                qWarning() << "Failed AO40Short decoding";
            }
        }
        break;
    case OperatingMode::RA_128_to_260:
        if (received.length() != 260) {
            qWarning() << "Wrong packet length for RA128:" << received.length();
        }
        else {
            DecodedPacket ra_result = decodeWithRA(received);
            if (ra_result.getResult() != DecodedPacket::Failure) {
                QByteArray decoded = ra_result.getDecodedPacket();
                processDecodedPacket(timestamp, source, "RA128", decoded, rssi);
            }
            else {
                qWarning() << "Failed RA128 decoding";
            }
        }
        break;
    case OperatingMode::RA_256_to_514:
        if (received.length() != 514) {
            qWarning() << "Wrong packet length for RA256:" << received.length();
        }
        else {
            DecodedPacket ra_result = decodeWithRA(received);
            if (ra_result.getResult() != DecodedPacket::Failure) {
                QByteArray decoded = ra_result.getDecodedPacket();
                processDecodedPacket(timestamp, source, "RA256", decoded, rssi);
            }
            else {
                qWarning() << "Failed RA256 decoding";
            }
        }
        break;
    case OperatingMode::RA_512_to_1028:
        if (received.length() != 1028) {
            qWarning() << "Wrong packet length for RA512:" << received.length();
        }
        else {
            DecodedPacket ra_result = decodeWithRA(received);
            if (ra_result.getResult() != DecodedPacket::Failure) {
                QByteArray decoded = ra_result.getDecodedPacket();
                processDecodedPacket(timestamp, source, "RA512", decoded, rssi);
            }
            else {
                qWarning() << "Failed RA512 decoding";
            }
        }
        break;
    case OperatingMode::RA_1024_to_2050:
        if (received.length() != 2050) {
            qWarning() << "Wrong packet length for RA1024:" << received.length();
        }
        else {
            DecodedPacket ra_result = decodeWithRA(received);
            if (ra_result.getResult() != DecodedPacket::Failure) {
                QByteArray decoded = ra_result.getDecodedPacket();
                processDecodedPacket(timestamp, source, "RA1024", decoded, rssi);
            }
            else {
                qWarning() << "Failed RA1024 decoding";
            }
        }
        break;
    case OperatingMode::RA_2048_to_4100:
        if (received.length() != 4100) {
            qWarning() << "Wrong packet length for RA2048:" << received.length();
        }
        else {
            DecodedPacket ra_result = decodeWithRA(received);
            if (ra_result.getResult() != DecodedPacket::Failure) {
                QByteArray decoded = ra_result.getDecodedPacket();
                processDecodedPacket(timestamp, source, "RA2048", decoded, rssi);
            }
            else {
                qWarning() << "Failed RA2048 decoding";
            }
        }
        break;
    case OperatingMode::Invalid:
        qWarning() << "The previous processed sync-length packet was invalid";
        break;
    case OperatingMode::Receive:
        qWarning() << "Received a packet even though the satellite should be receiving";
        break;
    }
}

void PacketDecoder::decodablePacketReceived(QDateTime timestamp, QString source, QString packetUpperHexString) {
    decodablePacketReceivedWithRssi(timestamp, source, packetUpperHexString, 0);
}

/**
 * @brief The slot that prefix changes are connected to. Changes PacketDecoder::prefix to \p prefix
 * @param prefix The new prefix to use
 */
void PacketDecoder::changePrefix(QString prefix) {
    this->prefix = prefix;
}

/**
 * @brief The slot that satellite changes are connected to. Changes PacketDecoder::activeSatellite to \p satellite
 * @param satellite The new satellite
 */
void PacketDecoder::changeSatellite(SatelliteChanger::Satellites satellite) {
    this->currentSatellite = satellite;
}
