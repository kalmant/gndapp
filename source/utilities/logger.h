#ifndef LOGGER_H
#define LOGGER_H
#include <QDateTime>
#include <QFile>
#include <QObject>
#include <QString>
#include <QTextStream>

/**
 * @brief Logs messages into a file.
 *
 * Automatically logs messages with timestamps into a file. The file's name depends on the current date.
 * UTC is used in the timestamps and filenames.
 */
class Logger : public QObject {
    Q_OBJECT
private:
    QString logDirString; //!< The folder that the log files will be written to.
    QString fileName;     //!< QString that stores the current filename.
    QString prefix;       //!< QString that stores the fileName prefix that will be prepended to the file name.

public:
    explicit Logger(QObject *parent = 0);
    void setLogFolder(QString logDirString);
    Q_INVOKABLE bool writeToLog(QString message);

public slots:
    void changePrefix(QString prefix);
};

#endif // LOGGER_H
