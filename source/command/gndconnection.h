#ifndef GNDCONNECTION_H
#define GNDCONNECTION_H

#include "../utilities/satellitechanger.h"
#include "commandqueue.h"
#include "dependencies/obc-packet-helpers/uplink.h"
#ifdef UPLINK_ENABLED
#include "dependencies/uplink-codec/s1-uplink-codec.h"
#endif
#include <QDataStream>
#include <QDateTime>
#include <QFile>
#include <QObject>
#include <QTcpSocket>
#include <QTimer>

#define OUTPUTLENGTH 64

/**
 * @brief Class that is used to establish connection through an IP and a port, with the ground station Raspberry PI.
 */
class GNDConnection : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool isConnected READ isConnected NOTIFY isConnectedChanged)
    Q_PROPERTY(QStringList commands READ commandList NOTIFY commandListChanged)

private:
    QScopedPointer<QTcpSocket, QScopedPointerDeleteLater> tcpSocket; //!< QSP to the object that handles TCP traffic
    bool isConnected() const;
    CommandQueue commands_priv;      //!< Holds the commands that should be sent
    QTimer commandQueueTimer_priv;   //!< Initiates the sending of a command upon timeout
    QTimer commandTimeoutTimer_priv; //!< Stops GNDConnection::commandQueueTimer_priv
    unsigned int dataRate_priv;      //!< Current datarate
    unsigned int packetLength_priv;  //!< Current packet length
    QString receivedMessage_priv;    //!< Received message so far
    SatelliteChanger::Satellites currentSatellite;

    void sendCommand(uint16_t commandId, QByteArray command, QString commandString);
    void queueCommand(unsigned int times, QByteArray command, QString commandString, QDateTime timestamp);
    void stopTimers();
    void processMessage(const QString &message);

public:
    explicit GNDConnection(QObject *parent = 0);
    ~GNDConnection();

    Q_INVOKABLE void connectToServer(QString ipAddress, quint16 port);
    Q_INVOKABLE void disconnectFromServer();
    Q_INVOKABLE void queueUplink(quint32 repeat, quint64 sendTimestamp, QVariant command);
    Q_INVOKABLE void clearCommandQueue();

    QStringList commandList() const;

signals:
    void isConnectedChanged();
    void decodablePacket(QDateTime timestamp, QString source, QString packetUpperHexString, int rssi);
    void commandListChanged();
    void newDatarate(unsigned int datarateBPS);
    void newPacketLength(unsigned int packetLength);
    void newCommandSent(uint16_t uplinkId, QString commandString, QString ackTimestampString);
public slots:
    void connectedToServer();
    void disconnectedFromServer();
    void bytesWritten(qint64 bytes);
    void readyRead();
    void startSendingCommands(
        unsigned long startDelayMs, unsigned int timeBetweenMs, unsigned long stopAfterMilliseconds);
    void newCommandIdSlot(uint16_t newId);
    void error(QAbstractSocket::SocketError socketError);
    void setDatarate(unsigned int datarateBPS);
    void setPacketLength(unsigned int packetLength);
    void changeSatellite(SatelliteChanger::Satellites satellite);
};

#endif // GNDCONNECTION_H
