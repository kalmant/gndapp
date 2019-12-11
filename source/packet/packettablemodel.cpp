#include "packettablemodel.h"

/**
 * @brief Constructor for the class.
 *
 * Sets \p PacketTableModel::clipboard to \p clipboard and initializes \p insertAtEnd_priv to true.
 * Starts \p PPSTimer_priv.
 *
 * @param[in] clipboard Pointer to the QClipboard objects that gives access to the clipboard
 * @param[in] parent Parent QObject, should be left empty.
 */
PacketTableModel::PacketTableModel(QClipboard *clipboard, QObject *parent) : QAbstractTableModel(parent) {
    Q_ASSERT(clipboard != nullptr);
    this->clipboard = clipboard;
    insertAtEnd_priv = false;
    selectedRow_priv = -1;
    PPS_priv = 0;
    QObject::connect(&PPSTimer_priv, &QTimer::timeout, [&]() {
        PPS_priv = 0;
        emit currentPPSChanged(PPS_priv);
    });
    PPSTimer_priv.start(1000);
}

/**
 * @brief Returns the number of columns for the children of the given parent.
 *
 * Has to be implemented when subclassing QAbstractTableModel.
 *
 * @param[in] parent The parent whose children we get the number of columns for.
 * @return Returns 5. The 5 columns are: timestamp, source, type, encoding, satellite and rssi.
 */
int PacketTableModel::columnCount(const QModelIndex &parent) const {
    return parent.isValid() ? 0 : 6;
}

/**
 * @brief Returns the number of rows under the given parent.
 *
 * Has to be implemented when subclassing QAbstractTableModel.
 *
 * @param[in] parent The parent whose children we get the number of columns for.
 * @return Returns 0 if \p parent is valid. Check Qt documentation for QAbstractItemModel.
 */
int PacketTableModel::rowCount(const QModelIndex &parent) const {
    return parent.isValid() ? 0 : packets_priv.length();
}

/**
 * @brief Returns the data stored under the given role for the item referred to by the index.
 *
 * Return the corresponding \p index th element from \p packets_priv.
 *
 * @param[in] index The index for the element that you want returned.
 * @param[in] role The role for the element that you want returned.
 * @return Returns a QVariant that represents the data for that role. Returns an invalid
 * QVariant if the role was invalid.
 */
QVariant PacketTableModel::data(const QModelIndex &index, int role) const {
    Q_ASSERT(index.row() >= 0 && index.row() < packets_priv.length());
    switch (role) {
    case timestampRole:
        return packets_priv.at(index.row()).timestamp;
    case sourceRole:
        return packets_priv.at(index.row()).source;
    case typeRole:
        return packets_priv.at(index.row()).type;
    case encodingRole:
        return packets_priv.at(index.row()).encoding;
    case authRole:
        return packets_priv.at(index.row()).auth;
    case satelliteRole:
        return packets_priv.at(index.row()).satellite;
    case decodedDataRole:
        return packets_priv.at(index.row()).decodedData;
    case packetRole:
        return packets_priv.at(index.row()).packet;
    case rssiRole:
        return packets_priv.at(index.row()).rssi;
    default:
        return QVariant();
    }
}

Qt::ItemFlags PacketTableModel::flags(const QModelIndex &index) const {
    if (index.row() < 0 || index.row() >= packets_priv.length()) {
        return Qt::NoItemFlags;
    }
    else {
        return (Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    }
}

/**
 * @brief Add a new row to the table with the data \p newData.
 *
 * Depending on \p insertAtEnd_priv it is either appended to the end or inserted to the beginning of \p packets_priv.
 *
 * @param[in] newData New packet that should be added to \p packets.
 */
void PacketTableModel::addData(PacketData newData) {
    emit packetAdded(newData.packet);

    PPS_priv += 1;
    emit currentPPSChanged(PPS_priv);
    if (insertAtEnd_priv) {
        if (packets_priv.length() >= MAXPACKETSLEN) {
            beginRemoveRows(QModelIndex(), 0, 0);
            packets_priv.removeFirst();
            endRemoveRows();
        }
        beginInsertRows(QModelIndex(),
            std::max(0, packets_priv.length() - 1),
            std::max(0, packets_priv.length() - 1)); // A lot less CPU consumption since we don't have to draw as much
        packets_priv.append(newData);
    }
    else {
        if (packets_priv.length() >= MAXPACKETSLEN) {
            beginRemoveRows(QModelIndex(), packets_priv.length() - 1, packets_priv.length() - 1);
            packets_priv.removeLast();
            endRemoveRows();
        }
        beginInsertRows(QModelIndex(), 0, 0); // Uses a lot of CPU time due to the constant drawing
        packets_priv.prepend(newData);
    }
    endInsertRows();
    auto sr = selectedRow();
    // If no row is selected, then no row should be selected after inserting
    // If the insertion is top insertion, then the selected row should shift "down" (unless it's at the end)
    // If the insertion is bottom insertion, then the selected row should not change
    // When the list is full and an insertion occurs, the selected row needs to shift regardless of insertion type
    // If the selected row gets out of range, it is set to -1
    if (sr == -1) {
        // Intentionally no operation when no row is selected
    }
    else {
        if (packets_priv.length() >= MAXPACKETSLEN) {
            if (insertAtEnd_priv) {
                if (sr <= 0) {
                    setSelectedRow(-1);
                }
                else {
                    setSelectedRow(sr - 1);
                }
            }
            else {
                if (selectedRow() == MAXPACKETSLEN - 1) {
                    setSelectedRow(-1);
                }
                else {
                    setSelectedRow(sr + 1);
                }
            }
        }
        else {
            setSelectedRow(insertAtEnd_priv ? sr : sr + 1);
        }
    }
}

/**
 * @brief Returns the value of \p _insertAtEnd.
 * @return Returns the value of \p _insertAtEnd.
 */
bool PacketTableModel::insertAtEnd() const {
    return insertAtEnd_priv;
}

/**
 * @brief Sets \p _insertAtEnd to \p value. ALWAYS SETS TO FALSE
 * @param[in] value The new value for \p _insertAtEnd.
 */
void PacketTableModel::setInsertAtEnd(bool value) {
    (void) value;
    insertAtEnd_priv = false;
    // insertAtEnd_priv = value; // functionality disabled until the UI has been fixed
    // inserting a row at the bottom displays incorrect values in the table!
    emit insertAtEndChanged(insertAtEnd_priv);
}

int PacketTableModel::currentPPS() const {
    return PPS_priv;
}

int PacketTableModel::selectedRow() const {
    return selectedRow_priv;
}

void PacketTableModel::setSelectedRow(int value) {
    if (selectedRow_priv != value) {
        selectedRow_priv = value;
        emit selectedRowChanged(selectedRow_priv);
    }
}

/**
 * @brief Creates and returns a hash that contains the roles.
 *
 * This is utilized when creating the TableView in QML.
 *
 * @return The hash that contains the roles.
 */
QHash<int, QByteArray> PacketTableModel::roleNames() const {
    QHash<int, QByteArray> retRoles;
    retRoles[timestampRole] = "timestamp";
    retRoles[sourceRole] = "source";
    retRoles[typeRole] = "type";
    retRoles[encodingRole] = "encoding";
    retRoles[authRole] = "auth";
    retRoles[satelliteRole] = "satellite";
    retRoles[decodedDataRole] = "decodedData";
    retRoles[packetRole] = "packet";
    retRoles[rssiRole] = "rssi";
    return retRoles;
}

/**
 * @brief Returns every available data(timestamp, source, type, encoding, auth, satellite, decodedData, readableQString
 * - in this order) about the packet indicated by \p index in \p packets_priv
 * @param index Index of the packet in \p packets_priv
 * @return Returns a QList<QString> with a length of 8
 */
QList<QString> PacketTableModel::detailedInformation(int index) const {
    Q_ASSERT(index >= 0 && index < packets_priv.length());
    QList<QString> ret;
    PacketData tmp = packets_priv.at(index);
    ret << tmp.timestamp << tmp.source << tmp.type;
    ret << tmp.encoding << tmp.auth << tmp.satellite << tmp.decodedData << tmp.readableQString;
    return ret;
}

/**
 * @brief Returns the readableQString of the packet indicated by \p index in \p packets_priv
 * @param index Index of the packet in \p packets_priv
 * @return
 */
QString PacketTableModel::readableQString(int index) const {
    Q_ASSERT(index >= 0 && index < packets_priv.length());
    return packets_priv.at(index).readableQString;
}

QVariant PacketTableModel::getPacket(int index) const {
    Q_ASSERT(index >= 0 && index < packets_priv.length());
    return packets_priv.at(index).packet;
}

QString PacketTableModel::getSatelliteName(int index) const {
    Q_ASSERT(index >= 0 && index < packets_priv.length());
    return packets_priv.at(index).satellite;
}

/**
 * @brief Copies the decodedData of the packet indicated by \p index in \p packets_priv
 * @param index Index of the packet in \p packets_priv
 */
void PacketTableModel::copyToClipboard(int index) const {
    Q_ASSERT(index >= 0 && index < packets_priv.length());
    clipboard->setText(packets_priv.at(index).decodedData);
}

/**
 * @brief Slot that is used to add new packets to the list. Calls PacketTableModel::addData()
 * @param timestamp QString Timestamp for the packet
 * @param source Source of the packet
 * @param type Type of the packet
 * @param encoding Encoding of the packet
 * @param authQString Authentication segment of the packet as a QString
 * @param satelliteString The satellite as a QString
 * @param decodedQString Full, decoded data of the packet as a QString
 * @param readableQString The contents of the packet as a readable QString
 * @param packet The full packet wrapped in a QVariant
 * @param rssi The rssi that the packet was received with
 */
void PacketTableModel::newPacket(QString timestamp,
    QString source,
    QString type,
    QString encoding,
    QString authQString,
    QString satelliteString,
    QString decodedQString,
    QString readableQString,
    QVariant packet,
    int rssi) {

    PacketData packetData = {
        timestamp,
        source,
        type,
        encoding,
        authQString,
        satelliteString,
        decodedQString,
        readableQString,
        packet,
        rssi
    };

    addData(packetData);
}
