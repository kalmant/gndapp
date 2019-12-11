#include "logger.h"
#include "QDebug"

/**
 * @brief Constructor for the class.
 *
 * Initializes \p fileName.
 *
 * @param[in] parent The parent QObject, should be left empty.
 */
Logger::Logger(QObject *parent) : QObject(parent) {
    this->logDirString = "";
    this->prefix = "startup";
    this->fileName = QDateTime::currentDateTimeUtc().toString(QString("yyyyMMdd"));
}

/**
 * @brief Sets the log folder string that is used for file names
 * @param logDirString The log folder string
 */
void Logger::setLogFolder(QString logDirString) {
    this->logDirString = logDirString;
}

/**
 * @brief The slot that prefix changes are connected to. Changes Logger::prefix to \p prefix
 * @param prefix The new prefix to use
 */
void Logger::changePrefix(QString prefix) {
    this->prefix = prefix;
}

/**
 * @brief Appends the current logfile with a timestamp and \p message.
 *
 * Creates a new file if the date(UTC) has changed.
 * Time timestamp has a yyyy.MM.dd. HH:mm:ss format.
 *
 * @param[in] message The message to log.
 * @return Returns true if \p message was succesfully logged, returns false otherwise.
 */
bool Logger::writeToLog(QString message) {
    QString timestamp = QDateTime::currentDateTimeUtc().toString(Qt::ISODate);
    if (!((timestamp.left(4).compare(fileName.left(4)) == 0) &&
            (timestamp.mid(5, 2).compare(fileName.mid(4, 2)) == 0) &&
            (timestamp.mid(8, 2).compare(fileName.mid(6, 2)) == 0))) {
        fileName = timestamp.left(4) + timestamp.mid(5, 2) + timestamp.mid(8, 2);
    }
    QFile f(logDirString + prefix + "_" + fileName + ".txt");
    if (f.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
        QTextStream out(&f);
        out << timestamp << ": " << message << "\n";
        out.flush();
        f.close();
        return true;
    }
    qWarning() << "Could not write log file";
    return false;
}
