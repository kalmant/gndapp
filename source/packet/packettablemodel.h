#ifndef PACKETTABLEMODEL_H
#define PACKETTABLEMODEL_H
#include <QAbstractTableModel>
#include <QClipboard>
#include <QDateTime>
#include <QDebug>
#include <QFile>
#include <QList>
#include <QTextStream>
#include <QTimer>
#include <QVariant>

/**
 * @brief Struct that stores information in a packet that has been decoded.
 */
struct PacketData {

    QString timestamp;       //!< Timestamp for the packet (as QString!)
    QString source;          //!< Source of the packet (audio, sdr, etc.)
    QString type;            //!< Type of the packet (telemetry1, etc.)
    QString encoding;        //!< Encoding of the packet (ao40short, ao40, racoder)
    QString auth;            //!< The authentication segment of the packet
    QString satellite;       //!< The satellite
    QString decodedData;     //!< Decoded data in the packet as an UPPERCASE Hex encoded QString
    QString readableQString; //!< Contents of the packet as a readable QString
    QVariant packet;         //!< Contents of the packet wrapped in a QVariant
    int rssi;                //!< The rssi that the packet was received with
};

/**
 * @brief Custom TableModel for storing timestamps and packet data.
 *
 * Every row represents a packet. There is also a timestamp for each packet.
 */
class PacketTableModel : public QAbstractTableModel {
    Q_OBJECT
    Q_PROPERTY(bool insertAtEnd READ insertAtEnd WRITE setInsertAtEnd NOTIFY insertAtEndChanged)
    Q_PROPERTY(int currentPPS READ currentPPS NOTIFY currentPPSChanged)
    Q_PROPERTY(int selectedRow READ selectedRow WRITE setSelectedRow NOTIFY selectedRowChanged)

    /**
     * @brief The roles that identify the timestamp and packetdata fields in the table.
     */
    enum roles {
        timestampRole = Qt::UserRole + 1, //!< Role that represents the timestamp
        sourceRole,                       //!< Role that represents the source of the packet
        typeRole,                         //!< Role that represents the type of the packet
        encodingRole,                     //!< Role that represents the encoding used
        authRole,                         //!< Role that represents the authentication segment
        satelliteRole,                    //!< Role that represents the satellite
        decodedDataRole,                  //!< Role that represents raw packet data
        packetRole,                       //!< Role that represents the packet wrapped in a QVariant
        rssiRole,                         //!< Role that represents the rssi
    };

public:
    PacketTableModel(QClipboard *clipboard, QObject *parent = 0);
    //  ~PacketTableModel();

    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    void addData(PacketData newData);

    bool insertAtEnd() const;
    void setInsertAtEnd(bool value);
    int currentPPS() const;
    int selectedRow() const;
    void setSelectedRow(int value);

    QHash<int, QByteArray> roleNames() const;
    Q_INVOKABLE QList<QString> detailedInformation(int index) const;
    Q_INVOKABLE QString readableQString(int index) const;
    Q_INVOKABLE QVariant getPacket(int index) const;
    Q_INVOKABLE QString getSatelliteName(int index) const;
    Q_INVOKABLE void copyToClipboard(int index) const;

private:
    const int MAXPACKETSLEN = 200;
    QList<PacketData> packets_priv; //!< The list that is used to store the received PacketData objects.
    bool insertAtEnd_priv;          //!< Defines where new packets will be added on the list (start/end)
    QTimer PPSTimer_priv;           //!< The timer that updates the PPS value.
    int selectedRow_priv;           //!< Index of the row that is currently selected on the UI
    int PPS_priv; //!< We keep a record of the packets per second metric in order to avoid too much writing on the
                  //!< screen.
    QClipboard *clipboard; //!< Pointer to the clipboard

public slots:
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

signals:
    /**
     * @brief Signal that is emitted when \p insertAtEnd_priv has changed.
     * @param value The new value for \p insertAtEnd_priv.
     */
    void insertAtEndChanged(bool value);

    /**
     * @brief Emitted whenever the \p PPS_priv variable changes.
     * @param value New value of \p PPS_priv.
     */
    void currentPPSChanged(int value);

    /**
     * @brief Signal emitted when \p selectedRow_priv has changed.
     * @param value The new value for \p selectedRow_priv.
     */
    void selectedRowChanged(int value);

    void packetAdded(QVariant packet);
};
#endif // PACKETTABLEMODEL_H
