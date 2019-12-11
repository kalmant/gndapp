#include "commandqueue.h"

CommandQueue::CommandQueue(uint16_t initialCommandId) {
    commandId_priv = initialCommandId;
}

void CommandQueue::setCommandId(uint16_t commandId) {
    commandId_priv = commandId;
    previousCommandString_priv = QString();
}

/**
 * @brief Queues the command with the provided parameters. CommandQueue::commandList_priv and
 * CommandQueue::commandQueue_priv are updated. The new command is put into the right location on both lists.
 * @param times How many times the command should be sent out.
 * @param command The command data.
 * @param commandString The string describing the command.
 * @param timestamp The timestamp that the command should not be sent out before.
 */
void CommandQueue::addToQueue(unsigned int times, QByteArray &command, QString &commandString, QDateTime timestamp) {
    if (0 == times) {
        return;
    }
    QString displayString = commandString;
    QString timestampString;
    if (timestamp <= QDateTime::currentDateTimeUtc()) {
        timestampString = "ASAP";
    }
    else {
        timestampString = timestamp.toString(Qt::ISODate);
    }
    displayString.append(QString(" (x%1) @").arg(times)).append(timestampString);
    int index = 0;
    for (; index < commandQueue_priv.size(); index++) {
        auto timeStampAtIndex = std::get<3>(commandQueue_priv.at(index));
        if (timestamp < timeStampAtIndex) {
            break;
        }
    }
    commandQueue_priv.insert(index, commandQuad{times, command, commandString, timestamp});
    commandList_priv.insert(index, displayString);
}

bool CommandQueue::isQueueEmpty() const {
    return commandQueue_priv.isEmpty();
}

QStringList CommandQueue::getCommandList() const {
    QStringList newList;
    for (int i = 0; i < commandList_priv.size(); i++) {
        newList.append((QString("%1. ").arg(i + 1)).append(commandList_priv.at(i)));
    }
    return newList;
}

/**
 * @brief Returns the first command in CommandQueue::commandQueue_priv if its timestamp is <= \p timestamp. The
 * command's count is updated in both CommandQueue::commandQueue_priv and CommandQueue::commandList_priv.
 * @param timestamp The timestamp that is not allowed to be smaller than the returned command's timestamp
 * @return Returns a CommandQueue::commandTriple describing the next command in CommandQueue::commandQueue_priv before
 * \p before. Returns an "empty" CommandQueueu::commandTriple if there are no such commands in the queue.
 */
commandTriple CommandQueue::takeCommand(QDateTime timestamp) {
    if (commandQueue_priv.isEmpty()) {
        return commandTriple{0, QByteArray(), QString()};
    }
    commandQuad first = commandQueue_priv.first();
    if (std::get<3>(first) > timestamp) {
        return commandTriple{0, QByteArray(), QString()};
    }
    commandQueue_priv.removeFirst();
    if (previousCommandString_priv != std::get<2>(first)) {
        previousCommandString_priv = std::get<2>(first);
        commandId_priv++;
    }
    commandTriple ret{commandId_priv, std::get<1>(first), std::get<2>(first)};
    std::get<0>(first)--;
    if (std::get<0>(first) < 1) {
        commandList_priv.removeFirst();
    }
    else {
        commandQueue_priv.prepend(first);
        commandList_priv[0] = std::get<2>(first).append(QString(" (x%1) @ASAP").arg(std::get<0>(first)));
    }
    return ret;
}

void CommandQueue::clearQueue() {
    commandList_priv.clear();
    previousCommandString_priv = QString();
    commandQueue_priv.clear();
}
