#include "commandtracker.h"

void CommandTracker::commandSent(uint16_t uplinkId, QString commandString, QString sentTimestampString) {
    for (int index = 0; index < trackedCommands_priv.size(); index++) {
        auto current = trackedCommands_priv.at(index);
        if (std::get<0>(current) == uplinkId) {
            if (std::get<3>(current)) {
                qWarning() << "The command with this uplink ID has already been acknowledged";
            }
            if (std::get<1>(current) != commandString) {
                qWarning() << "Different commandString: (old:" << std::get<1>(current) << ", new:" << commandString
                           << ") for the same uplinkID:" << uplinkId;
            }
            trackedCommands_priv[index] =
                commandTrackPenta{uplinkId, commandString, sentTimestampString, false, QString()};
            auto modelIndex = QAbstractItemModel::createIndex(index, 0);
            emit dataChanged(modelIndex, modelIndex);
            return;
        }
    }
    /*
     * Making sure there aren't more than MAXLEN items in the list.
     * First, We discard items that have been acknowledged already
     * If there are not enough items like that, we remove the oldest(last) item
     */
    while (trackedCommands_priv.size() >= static_cast<int>(MAXLEN)) {
        int index = trackedCommands_priv.size() - 1;
        for (; index >= 0; index--) {
            if (std::get<3>(trackedCommands_priv.at(index))) {
                break;
            }
        }
        if (index >= 0) {
            beginRemoveRows(QModelIndex(), index, index);
            trackedCommands_priv.removeAt(index);
        }
        else {
            beginRemoveRows(QModelIndex(), trackedCommands_priv.size() - 1, trackedCommands_priv.size() - 1);
            trackedCommands_priv.removeLast();
        }
        endRemoveRows();
    }
    beginInsertRows(QModelIndex(), 0, 0);
    trackedCommands_priv.prepend(commandTrackPenta{uplinkId, commandString, sentTimestampString, false, QString()});
    endInsertRows();
}

void CommandTracker::commandAcknowledged(uint16_t uplinkId, QString ackTimestampString) {
    for (int index = 0; index < trackedCommands_priv.size(); index++) {
        auto current = trackedCommands_priv.at(index);
        if (std::get<0>(current) == uplinkId && !std::get<3>(current)) {
            trackedCommands_priv[index] = commandTrackPenta{
                std::get<0>(current), std::get<1>(current), std::get<2>(current), true, ackTimestampString};
            auto modelIndex = QAbstractItemModel::createIndex(index, 0);
            emit dataChanged(modelIndex, modelIndex);
            return;
        }
    }
}

CommandTracker::CommandTracker(QObject *parent) : QAbstractListModel(parent) {
    trackedCommands_priv.reserve(static_cast<int>(MAXLEN));
}

/**
 * @brief Creates and returns a hash that contains the roles.
 *
 * This is utilized when creating the TableView in QML.
 *
 * @return The hash that contains the roles.
 */
QHash<int, QByteArray> CommandTracker::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[CommandTracker::roles::uplinkIdRole] = "uplinkId";
    roles[CommandTracker::roles::commandStringRole] = "commandString";
    roles[CommandTracker::roles::sentTimeStringRole] = "sentTimeString";
    roles[CommandTracker::roles::acknowledgedRole] = "acknowledged";
    roles[CommandTracker::roles::acknowledgedTimeStringRole] = "acknowledgedTimeString";
    return roles;
}

int CommandTracker::rowCount(const QModelIndex &parent) const {
    return parent.isValid() ? 0 : trackedCommands_priv.length();
}

/**
 * @brief Returns the data stored under the given role for the item referred to by the index.
 *
 * \p Return the corresponding \p index th element from \p trackedCommands_priv.
 *
 * @param[in] index The index for the element that you want returned.
 * @param[in] role The role for the element that you want returned.
 * @return Returns a QVariant that represents the data for that role. Returns an invalid
 * QVariant if the role was invalid.
 */
QVariant CommandTracker::data(const QModelIndex &index, int role) const {
    Q_ASSERT(index.row() >= 0 && index.row() < trackedCommands_priv.length());
    int rowIndex = index.row();
    switch (role) {
    case CommandTracker::roles::uplinkIdRole:
        return std::get<0>(trackedCommands_priv.at(rowIndex));
    case CommandTracker::roles::commandStringRole:
        return std::get<1>(trackedCommands_priv.at(rowIndex));
    case CommandTracker::roles::sentTimeStringRole:
        return std::get<2>(trackedCommands_priv.at(rowIndex));
    case CommandTracker::roles::acknowledgedRole:
        return std::get<3>(trackedCommands_priv.at(rowIndex));
    case CommandTracker::roles::acknowledgedTimeStringRole:
        return std::get<4>(trackedCommands_priv.at(rowIndex));
    default:
        return QVariant();
    }
}

void CommandTracker::newCommandSent(uint16_t uplinkId, QString commandString, QString sentTimestampString) {
    commandSent(uplinkId, commandString, sentTimestampString);
}

void CommandTracker::newCommandAcknowledged(uint16_t uplinkId, QString ackTimestampString) {
    commandAcknowledged(uplinkId, ackTimestampString);
}
