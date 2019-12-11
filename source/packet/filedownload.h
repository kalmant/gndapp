#ifndef FILEDOWNLOAD_H
#define FILEDOWNLOAD_H

#include "dependencies/obc-packet-helpers/downlink.h"
#include <QByteArray>
#include <QDateTime>
#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QString>
#include <QStringList>
#include <QTextStream>
#include <QVector>

class FileDownload : public QObject {
    Q_OBJECT
private:
    const QByteArray MISSINGFRAGMENT_PRIV = "[FRAGMENT_MISSING]"; //!< Missing fragment indicator
    QVector<QByteArray> dataVector_priv;                          //!< The QVector used to store the fragments
    s1obc::FileType fileType_priv;                              //!< File type
    size_t nextIndex_priv;                                        //!< Index of the next expected fragment
    size_t fragmentCount_priv;                                    //!< Number of expected fragments in the file
    QString fileName_priv; //!< Name of the file that will be created upon calling FileDownload::finishUpFile()
    QString partialFileName_priv;
    QStringList missingFragments_priv; //!< List of the missing fragment indices
    void writeToFile(QByteArray data);
    void writeMissingFragments();
    QByteArray getCompleteData();
    void populateMissingFragments();
    bool writeToFile();
    void attemptLoadFromPartialFile();
    void writePartialFile() const;
    void writeTextFile(QTextStream &out, const QByteArray &data);
    void writeSpectrumFile(QTextStream &out, const QByteArray &data);
    void writeUniversalMeasurementFile(QTextStream &out, const QByteArray &data);
    void writeInfoTableFile(QTextStream &out, const QByteArray &data);
    bool writeCompleteFile();
    void reset();

public:
    bool isFinished = false; //!< Indicates whether FileDonwload::finishUpFile() has been called already on the file
    FileDownload(QString fileName,
        uint16_t fragmentCount,
        s1obc::FileType fileType,
        QObject *parent = 0);

    void addFragment(uint16_t index, QByteArray data);
    bool finishUpFile();

signals:
    void newSpectrumFile(QByteArray data,
        uint32_t timestamp,
        uint16_t measurementId,
        uint32_t startFrequency,
        uint32_t stepSize,
        uint8_t rbw);
};

#endif // FILEDOWNLOAD_H
