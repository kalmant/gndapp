#ifndef COMMANDTRACKER_H
#define COMMANDTRACKER_H

#include <QAbstractListModel>
#include <QDateTime>
#include <QDebug>
#include <QString>
#include <QVector>
#include <tuple>

class CommandTracker : public QAbstractListModel {
    Q_OBJECT
private:
    using commandTrackPenta = std::tuple<uint16_t, QString, QString, bool, QString>;
    const unsigned int MAXLEN = 50;

    enum roles {
        uplinkIdRole = Qt::UserRole + 1,
        commandStringRole,
        sentTimeStringRole,
        acknowledgedRole,
        acknowledgedTimeStringRole,
    };

    QVector<commandTrackPenta> trackedCommands_priv;

    void commandSent(uint16_t uplinkId, QString commandString, QString sentTimestampString);
    void commandAcknowledged(uint16_t uplinkId, QString ackTimestampString);

public:
    explicit CommandTracker(QObject *parent = nullptr);

    QHash<int, QByteArray> roleNames() const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
signals:

public slots:
    void newCommandSent(uint16_t uplinkId, QString commandString, QString sentTimestampString);
    void newCommandAcknowledged(uint16_t uplinkId, QString ackTimestampString);
};

#endif // COMMANDTRACKER_H
