#ifndef PACKETDECODER_H
#define PACKETDECODER_H

#define AO40_DOWNLINKLENGTH 650
#define AO40_DECDOWNLINKLENGTH 256
#define AO40SHORT_DOWNLINKLENGTH 333
#define AO40SHORT_DECDOWNLINKLENGTH 128

#include "../packet/decodedpacket.h"
#include "../packet/spectrumreceiver.h"
#include "../utilities/common.h"
#include "../utilities/satellitechanger.h"
#include "dependencies/FEC-AO40/ao40-short/decode/ao40short_decode_message.h"
#include "dependencies/FEC-AO40/ao40/decode/ao40_decode_message.h"
#include "dependencies/obc-packet-helpers/downlink.h"
#include "dependencies/racoder/ra_decoder_gen.h"
#ifdef UPLINK_ENABLED
#include "dependencies/uplink-codec/signature.h"
#endif
#include "filedownload.h"
#include "syncpacket.h"
#include <QDateTime>
#include <QDebug>
#include <QFile>
#include <QHash>
#include <QObject>
#include <QRegularExpression>
#include <QScopedPointer>
#include <QTimeZone>
#include <QTimer>
#include <algorithm>

/**
 * @brief The class that handles decoding of coded messages.
 *
 * This class decodes messages and emits signals that update the data model.
 *
 */
class PacketDecoder : public QObject {
    Q_OBJECT
public:
    explicit PacketDecoder(QString logDirString,
        QString fileName,
        QString downloadDirString,
        SpectrumReceiver *spectrumReceiver,
        QObject *parent = 0);
    Q_INVOKABLE void manualPacketInput(QString source, QString packetUpperHexString);

private:
    QString logDirString;
    QString fileName;
    QString prefix;
    QString downloadDirString;
    SpectrumReceiver *spectrumReceiver_priv;
    SatelliteChanger::Satellites currentSatellite;
    QHash<QString, QString> recentPackets_priv;
    uint16_t lastCommandId_priv = 0;
    QTimer fileDownloadTimer_priv;
    QTimer syncTimeoutTimer_priv;
    unsigned int dataRate_priv;
    QScopedPointer<FileDownload> fileDownload_priv;
    s1sync::OperatingMode decodeMode_priv;
    QString processTelemetry1(const s1obc::SolarPanelTelemetryPacket &packet);
    QString processTelemetry2(const s1obc::PcuTelemetryPacket &packet);
    QString processTelemetry2A(const s1obc::PcuTelemetryPacket &packet);
    QString processTelemetry3(const s1obc::OnboardTelemetryPacket &packet);
    QString processTelemetry3P(const s1obc::OnboardTelemetryPacketPA &packet);
    QString processTelemetry3A(const s1obc::OnboardTelemetryPacketPA &packet);
    QString processBeacon(const s1obc::BeaconPacket &packet);
    QString processBeaconPA(const s1obc::BeaconPacketPA &packet);
    QString processSpectrumData(const s1obc::SpectrumPacket &packet);
    QString processFileDownload(const s1obc::FileDownloadPacket &packet);
    QString processBattery(const s1obc::BatteryPacketA &packet);
    QString extractSolarPanelTelemetry(const s1obc::SolarPanelTelemetry &panel);
    QString processAcknowledgedCommands(const s1obc::AcknowledgedCommands &commands);
    QString extractPCUDeploymentTelemetry(const s1obc::PcuDeploymentTelemetry &deployment);
    QString extractPCUBatteryTelemetry(const s1obc::PcuBatteryTelemetry &battery);
    QString extractPCUBusTelemetry(const s1obc::PcuBusTelemetry &bus);
    QString extractPCUSDCTelemetry(const s1obc::PcuSdcTelemetry &sdc);
    QString extractOBCTelemetry(const s1obc::ObcTelemetry &obc);
    QString extractOBCTelemetryPA(const s1obc::ObcTelemetryPA &obc);
    QString extractCOMTelemetry(const s1obc::ComTelemetry &com);
    QString extractTIDTelemetry(const s1obc::TidTelemetry &tid);
    QString extractDiagnosticTelemetry(const s1obc::DiagnosticInfo &diagnostic);
    QString extractDiagnosticTelemetryPA(const s1obc::DiagnosticInfoPA &diagnostic);
    QString extractBatteryTelemetryA(const s1obc::BatteryTelemetryA &battery, uint8_t panel);
    void waitForSyncPacket();
    void startSyncPacketTimeout(unsigned int datarateBPS, s1sync::OperatingMode operatingMode);
    const DecodedPacket decodeWithAO40SHORT(const QByteArray encodedData);
    const DecodedPacket decodeWithAO40LONG(const QByteArray encodedData);
    const DecodedPacket decodeWithRA(const QByteArray encodedData);
    const QString getDateTimeString(QDateTime datetime) const;
    void processDecodedPacket(const QDateTime &timestamp,
        const QString &source,
        const QString &encoding,
        QByteArray &decodedPacket,
        int rssi);
    void packetSuccessfullyDecoded(QDateTime timestamp,
        QString source,
        QString type,
        QString encoding,
        QByteArray auth,
        QByteArray decodedData,
        QString readableQString,
        QVariant packet,
        int rssi);
    void processSyncContents(unsigned int datarateBPS, s1sync::OperatingMode operatingMode);
    bool isUpperHexString(QString input) const;
    bool checkSignature(const QByteArray &data) const;
    void newCommandId(uint16_t newId);
    QString fileTypeToQString(s1obc::FileType type);
    QString fileEntryToQString(s1obc::FileEntry entry);
    QString fileToQString(uint8_t id, s1obc::FileEntry entry);

    bool isTimestampValid(int32_t timestamp) const;
    bool checkForAnomalies(QByteArray &decodedPacket) const;

public slots:
    void decodablePacketReceived(QDateTime timestamp, QString source, QString packetUpperHexString);
    void decodablePacketReceivedWithRssi(QDateTime timestamp, QString source, QString packetUpperHexString, int rssi);
    void changePrefix(QString prefix);
    void changeSatellite(SatelliteChanger::Satellites satellite);

signals:
    /**
     * @brief Signal that is emmitted when a packet has been decoded
     * @param decodedQString Full, decoded data of the packet as a QString
     * @param satellite The satellite that the packet was received for
     */
    void newPacketMinimal(QString decodedQString, SatelliteChanger::Satellites satellite);
    /**
     * @brief Signal that is emmitted when a packet has been decoded
     * @param timestamp QString Timestamp for the packet
     * @param source Source of the packet
     * @param type Type of the packet
     * @param encoding Encoding of the packet
     * @param satelliteString The satellite as a QString
     * @param authQString Authentication segment of the packet as a QString
     * @param decodedQString Full, decoded data of the packet as a QString
     * @param readableQString The contents of the packet as a readable QString
     * @param packet Contents of the packet wrapped in a QVariant
     * @package rssi The RSSI that the packet was received with
     */
    void newPacket(QString timestamp,
        QString source,
        QString type,
        QString encoding,
        QString authQString,
        QString satelliteString,
        QString decodedQString,
        QString readableQString,
        QVariant packet,
        int rssi);

    /**
     * @brief Signal, that lets SpectrumReceiver know, that the spectrum analysis command was received by the satellite.
     * @param packetID ID of the acknowledged spectrum analysis command.
     * @param numberOfPackets The number of packets that will be sent as a response to the command
     */
    void ackReceivedForSpectrum(uint8_t packetID, uint16_t numberOfPackets);

    /**
     * @brief Signal that carries data for a new packet
     * @param spectrumPacket Spectrum packet
     */
    void newSpectrumPacket(s1obc::SpectrumPacket spectrumPacket);

    /**
     * @brief Signal that is emitted when the datarate should change
     * @param newDataRateBPS New datarate [BPS]
     */
    void newDataRate(unsigned int newDataRateBPS);

    /**
     * @brief Signal that is emitted when the packet length should change
     * @param newPacketLengthBytes New packet length [bytes]
     */
    void newPacketLength(unsigned int newPacketLengthBytes);

    /**
     * @brief Signals that commands can be sent
     * @param startDelayMs The delay before the first command
     * @param timeBetweenMs Delay between commands
     * @param stopAfterMilliseconds Stop sending commands after this many milliseconds
     */
    void sendCommands(unsigned long startDelayMs, unsigned int timeBetweenMs, unsigned long stopAfterMilliseconds);

    /**
     * @brief Signal that is emitted whenever a new id has been received, that is greater than the previous one
     * @param newId New (and greatest to date) command id
     */
    void newCommandIdSignal(uint16_t newId);

    /**
     * @brief Signal that is emitted to start \p PacketDecoder::syncTimeoutTimer_priv
     * @param msec Milliseconds
     */
    void startSyncTimeoutTimer(int msec);

    /**
     * @brief Signal that is emitted to stop \p PacketDecoder::syncTimeoutTimer_priv
     */
    void stopSyncTimeoutTimer();

    /**
     * @brief Signal that is emitted whenever the signal reception should be timed out due to sync timeout.
     */
    void timeOutSpectrumReception();

    /**
     * @brief Signal that is emitted when a packet has been acknowledged
     * @param uplinkId The uplink ID of the command that was acknowledged
     * @param ackTimestampString The QString describing the time when the ack has arrived
     */
    void newCommandAcknowledged(uint16_t uplinkId, QString ackTimestampString);
};

#endif // PACKETDECODER_H
