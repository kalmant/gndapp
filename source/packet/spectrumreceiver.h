#ifndef SPECTRUMRECEIVER_H
#define SPECTRUMRECEIVER_H

#include "../utilities/common.h"
#include "chartwindow.h"
#include "dependencies/obc-packet-helpers/downlink.h"
#include <QColor>
#include <QDateTime>
#include <QDebug>
#include <QLineSeries>
#include <QObject>
#include <QScopedPointer>
#include <QTimeZone>
#include <QTimer>
#include <QValueAxis>

/**
 * @brief The class that handles building a QChart that holds the information of the measurement.
 *
 * This class also takes care of opening a ChartWindow that will display the results.
 */
class SpectrumReceiver : public QObject {
    Q_OBJECT
private:
    QString spectrumDirString; //!< Folder that spectrum files will be saved to
    QString prefix;            //!< QString that stores the fileName prefix that will be prepended to the file name.
    QString satelliteName;     //!< QString that stores which satellite is currently in used. Displayed in the title.
    static constexpr size_t maxLengthPerPacket = s1obc::SpectrumData::maxLengthPerPacket;
    QScopedPointer<ChartWindow> openedWindow_priv; //!< Pointer to the ChartWindow that has been opened
    uint nextPacketCounter_priv;                   //!< Measurement packed ID of the next packet that should arrive
    uint32_t startFrequency_priv;                  //!< Start frequency for the measurement
    // uint32_t stopFrequency_priv;  //!< Stop frequency for the measurement, it is not included in the measurement
    // (excl.)
    uint32_t measurementStepSize_priv; //!< Step size for the measurement
    uint8_t measurementRbw_priv;
    uint8_t packetId_priv; //!< FUTURE - Used to verify that the response to the command from the satellite was valid
    uint16_t measurementId_priv; //!< The measurement ID of the current measurement
    uint32_t measurementTimestamp_priv;
    bool currentlyReceiving_priv = false;              //!< FUTURE - Usedto indicate whether we are currently receiving
    QScopedPointer<QtCharts::QChart> chart_priv;       //!< The QChart that the new window will be constructed with
    QScopedPointer<QtCharts::QLineSeries> series_priv; //!< Data for \p chart_priv
    bool neverReceived = true;                         //!< Indicates whether the receiver has ever received a packet
    bool isFinished_priv = false;                      //!< Indicates whether spectrum data is complete
    void displayWindow(bool fromDownload = false,
        QtCharts::QLineSeries *measurementSeries = nullptr,
        uint32_t measurementTimestamp = 0,
        uint16_t measurementId = 0,
        uint32_t measurementStartFrequency = 0,
        uint32_t measurementStepSize = 0,
        uint8_t measurementRbw = 0);
    void logToFile(bool fromDownload = false,
        QtCharts::QLineSeries *measurementSeries = nullptr,
        uint32_t measurementTimestamp = 0,
        uint16_t measurementId = 0,
        uint8_t measurementRbw = 0);
    void fillWithNoData();
    void resetVariables();
    void finishUp();

public:
    explicit SpectrumReceiver(QString spectrumDirString, QObject *parent = 0);

signals:

public slots:
    void ackForSpectrumAnalysisCommand(uint8_t packetID, uint16_t numOfPackets);
    void newSpectrumPacket(s1obc::SpectrumPacket spectrumPacket);
    void timeOut();
    void changePrefix(QString prefix);
    void newSpectrumFile(
        QByteArray data, uint32_t timestamp, uint16_t id, uint32_t startFrequency, uint32_t stepSize, uint8_t rbw);
    void changeSatelliteName(QString satelliteName);
};

#endif // SPECTRUMRECEIVER_H
