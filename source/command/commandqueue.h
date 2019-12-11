#ifndef COMMANDQUEUE_H
#define COMMANDQUEUE_H

#include <QByteArray>
#include <QDateTime>
#include <QString>
#include <QStringList>
#include <QVector>
#include <tuple>

using commandTriple = std::tuple<uint16_t, QByteArray, QString>;

class CommandQueue {
private:
    using commandQuad = std::tuple<unsigned int, QByteArray, QString, QDateTime>;
    uint16_t commandId_priv;
    QString previousCommandString_priv;
    QStringList commandList_priv;           //!< Strings describing the commands, sorted by the commands's timestamp
    QVector<commandQuad> commandQueue_priv; //!< CommandQuad's sorted by their 4th param (ascending)

public:
    CommandQueue(uint16_t initialCommandId = 0);
    void setCommandId(uint16_t commandId);
    void addToQueue(unsigned int times, QByteArray &command, QString &commandString, QDateTime timestamp);
    bool isQueueEmpty() const;
    QStringList getCommandList() const;
    commandTriple takeCommand(QDateTime timestamp);
    void clearQueue();
};

#endif // COMMANDQUEUE_H
