#include "spectrumreceiver.h"

/**
 * @brief Constructor for the class.
 *
 * Initializes the \p timer and its timeout action.
 *
 * @param spectrumDirString Folder that spectrum files will be saved to
 * @param parent The parent QObject, should be left empty.
 */
SpectrumReceiver::SpectrumReceiver(QString spectrumDirString, QObject *parent) : QObject(parent) {
    this->spectrumDirString = spectrumDirString;
    this->prefix = "startup";
    resetVariables();
}

/**
 * @brief Finishes up the data collecting process. Called whenever it is time to end the packet collection.
 */
void SpectrumReceiver::finishUp() {
    if (!currentlyReceiving_priv) {
        return;
    }
    currentlyReceiving_priv = false;
    logToFile();
    displayWindow();
}

/**
 * @brief Fills a maxLengthPerPacket measurement points with -131 dBm.
 */
void SpectrumReceiver::fillWithNoData() {
    for (uint i = 0; i < maxLengthPerPacket; i++) {
        series_priv->append(startFrequency_priv +
                                (nextPacketCounter_priv) * (maxLengthPerPacket * measurementStepSize_priv) +
                                i * measurementStepSize_priv,
            -131);
    }
}

/**
 * @brief Sets up the chart display settings, saves the measurement into a png and displays it.
 *
 * If \p fromDownload is `true`, then the other parameters are used instead of the object's private variables.
 * In this case, the chart is not displayed, it is only printed.
 */
void SpectrumReceiver::displayWindow(bool fromDownload,
    QtCharts::QLineSeries *measurementSeries,
    uint32_t measurementTimestamp,
    uint16_t measurementId,
    uint32_t measurementStartFrequency,
    uint32_t measurementStepSize,
    uint8_t measurementRbw) {

    ChartWindow download_window;
    ChartWindow *window;
    if (openedWindow_priv.isNull()) {
        openedWindow_priv.reset(new ChartWindow());
    }
    window = fromDownload ? &download_window : openedWindow_priv.data();
    QString windowTitle = this->satelliteName + ": Spectrum analysis";
    window->setWindowTitle(windowTitle);

    QtCharts::QLineSeries *series;
    series = fromDownload ? measurementSeries : series_priv.data();

    series->setColor(QColor("#000"));
    QString titleString("%1 - %2, %3 (%4), measured at ");
    auto stopFreq = series->at(series->count() - 1).x();
    titleString = titleString.arg(fromDownload ? measurementStartFrequency : startFrequency_priv)
                      .arg(static_cast<int>(stopFreq))
                      .arg(fromDownload ? measurementStepSize : measurementStepSize_priv)
                      .arg(fromDownload ? measurementRbw : measurementRbw_priv)
                      .append(getDTSFromUint32UTC(fromDownload ? measurementTimestamp : measurementTimestamp_priv))
                      .append(QString(", ID: %1").arg(fromDownload ? measurementId : measurementId_priv));

    if (fromDownload) {
        titleString.prepend("DOWLOADED: ");
    }
    if (!fromDownload && !isFinished_priv) {
        titleString.prepend(QStringLiteral("INCOMPLETE! "));
        chart_priv->setTitleBrush(QBrush(QColor(Qt::darkRed)));
    }

    QScopedPointer<QtCharts::QChart> QSP_chart;
    QtCharts::QChart *chart;
    if (fromDownload) {
        QSP_chart.reset(new QtCharts::QChart());
    }
    chart = fromDownload ? QSP_chart.data() : chart_priv.data();

    chart->setTitle(titleString);
    chart->legend()->hide();
    chart->addSeries(fromDownload ? measurementSeries : series_priv.take());
    chart->createDefaultAxes();

    auto ax = static_cast<QtCharts::QValueAxis *>(chart->axes(Qt::Horizontal).first());
    ax->setTitleText("Frequency [Hz]");
    ax->setLabelsColor(QColor("#000"));
    ax->setLinePenColor(QColor("#000"));
    ax->setLabelFormat("%d");
    ax->setTickCount(8);

    auto ay = static_cast<QtCharts::QValueAxis *>(chart->axes(Qt::Vertical).first());
    ay->setMin(-130);
    ay->setMax(-10);
    ay->setTickCount(13);
    ay->setLabelFormat("%d");
    ay->setTitleText("Amplitude [dBm]");
    ay->setLabelsColor(QColor("#000"));
    ay->setLinePenColor(QColor("#000"));

    QDateTime timestamp = QDateTime::fromSecsSinceEpoch(
        fromDownload ? measurementTimestamp : measurementTimestamp_priv, QTimeZone::utc());
    QString timestampString = timestamp.toString("yyyyMMdd_HHmmss");
    QString pngFileName = spectrumDirString + prefix + "_spectrum_" + timestampString + ".png";

    window->setChart(fromDownload ? QSP_chart.take() : chart_priv.take());
    window->renderPng(pngFileName);

    if (!fromDownload && !window->isVisible()) {
        window->show();
    }

    if (!fromDownload) {
        resetVariables();
        isFinished_priv = true;
    }
}

/**
 * @brief Logs measurement data to the log file in csv format.
 *
 * If \p fromDownload is `true`, then the other parameters are used as the source.
 */
void SpectrumReceiver::logToFile(bool fromDownload,
    QtCharts::QLineSeries *measurementSeries,
    uint32_t measurementTimestamp,
    uint16_t measurementId,
    uint8_t measurementRbw) {
    QDateTime timestamp;
    timestamp.setTime_t(fromDownload ? measurementTimestamp : measurementTimestamp_priv);
    QFile spectrumLogFile(
        spectrumDirString + prefix + "_" + "spectrum_" + timestamp.toString("yyyyMMdd_HHmmss") + ".csv");
    spectrumLogFile.open(QIODevice::WriteOnly | QIODevice::Truncate);
    if (!spectrumLogFile.isOpen()) {
        qWarning() << "Could not open spectrum log file (" + spectrumLogFile.fileName() + ") for writing";
        return;
    }
    QTextStream writer(&spectrumLogFile);
    writer << "Timestamp;" << getDTSFromUint32UTC(fromDownload ? measurementTimestamp : measurementTimestamp_priv)
           << "\nID;" << (fromDownload ? measurementId : measurementId_priv) << "\nRBW;"
           << (fromDownload ? measurementRbw : measurementRbw_priv) << "\nFrequency;Amplitude";
    QtCharts::QLineSeries *series = fromDownload ? measurementSeries : series_priv.data();
    qInfo() << "SPECTRUMFILEWRITE: fromDownload: " << fromDownload << "series count: " << series->count()
            << "filename:" << spectrumLogFile.fileName();
    for (int i = 0; i < series->count(); i++) {
        auto measurementPoint = series->at(i);
        writer << "\n" << measurementPoint.x() << ";" << measurementPoint.y();
    }
    writer << "\n";
    writer.flush();
    spectrumLogFile.close();
}

/**
 * @brief Resets certain variables so that we are ready to receive once again.
 */
void SpectrumReceiver::resetVariables() {
    currentlyReceiving_priv = false;
    isFinished_priv = false;
    nextPacketCounter_priv = 0;
    chart_priv.reset(new QtCharts::QChart());
    series_priv.reset(new QtCharts::QLineSeries());
    series_priv->setName("Spectrum data");
}

// currently unused
/**
 * @brief Slot that is invoked when the satellite has acknowledged the spectrum analysis command.
 * @param packetID The ID of the packet that initialized the measurement (the command packet ID)
 * @param numOfPackets Number of packets that will be sent, containing measurement data.
 */
void SpectrumReceiver::ackForSpectrumAnalysisCommand(uint8_t packetID, uint16_t numOfPackets) {
    (void) numOfPackets;
    if (this->packetId_priv == packetID) {
        currentlyReceiving_priv = true;
    }
}

/**
 * @brief Slot that is invoked when a new measurement packet has arrived.
 * @param spectrumPacket Spectrum packet object
 */
void SpectrumReceiver::newSpectrumPacket(s1obc::SpectrumPacket spectrumPacket) {
    uint8_t packetIndex = spectrumPacket.index();
    uint8_t packetCount = spectrumPacket.count();
    uint16_t dataLength = spectrumPacket.dataLength();
    s1obc::SpectrumData spectrumData = spectrumPacket.data();
    s1obc::MeasurementId measurementId = spectrumPacket.measurementId();
    if (measurementId != this->measurementId_priv || neverReceived == true || isFinished_priv) {
        if (neverReceived == true) {
            neverReceived = false;
        }
        else if (!isFinished_priv) {
            finishUp();
        }
        isFinished_priv = false;
        this->measurementId_priv = measurementId;
        this->startFrequency_priv = spectrumPacket.startFreq();
        this->measurementStepSize_priv = spectrumPacket.stepSize();
        this->measurementRbw_priv = spectrumPacket.rbw();
        this->measurementTimestamp_priv = spectrumPacket.timestamp();
    }
    currentlyReceiving_priv = true;
    // If this is the last packet then it's time to draw
    // if not, then the timer should be reset
    while (packetIndex > nextPacketCounter_priv) {
        fillWithNoData();
        nextPacketCounter_priv++;
    }
    for (uint i = 0; i < dataLength; i++) {
        series_priv->append(startFrequency_priv +
                                nextPacketCounter_priv * (maxLengthPerPacket * measurementStepSize_priv) +
                                i * measurementStepSize_priv,
            (static_cast<uint8_t>(spectrumData.bytes[i]) / 2) - 131);
    }

    if (packetIndex == (packetCount - 1)) {
        isFinished_priv = true;
        finishUp();
    }
    else {
        nextPacketCounter_priv++;
    }
}

/**
 * @brief The slot that is emitted to whenever the spectrum reception should time out
 */
void SpectrumReceiver::timeOut() {
    finishUp();
}

/**
 * @brief The slot that prefix changes are connected to. Changes SpectrumReceiver::prefix to \p prefix
 * @param prefix The new prefix to use
 */
void SpectrumReceiver::changePrefix(QString prefix) {
    this->prefix = prefix;
}

/**
 * @brief The slot that spectrum file downloads are connected to.
 * @param data The downloaded spectrum data.
 * @param timestamp The measurement timestamp.
 * @param id The measurement id.
 * @param startFrequency The starting frequency of the measurement.
 * @param stepSize The measurement step size.
 * @param rbw The measurement RBW.
 */
void SpectrumReceiver::newSpectrumFile(
    QByteArray data, uint32_t timestamp, uint16_t id, uint32_t startFrequency, uint32_t stepSize, uint8_t rbw) {

    QScopedPointer<QtCharts::QLineSeries> measurementSeries;
    measurementSeries.reset(new QtCharts::QLineSeries());
    measurementSeries->setName("Spectrum data");

    for (unsigned int i = 0; i < static_cast<unsigned int>(data.length()); i++) {
        measurementSeries->append(startFrequency + i * stepSize, (static_cast<uint8_t>(data[i]) / 2) - 131);
    }

    logToFile(true, measurementSeries.data(), timestamp, id, rbw);
    displayWindow(true, measurementSeries.take(), timestamp, id, startFrequency, stepSize, rbw);
}

/**
 * @brief The slot is used to change the satellite's name used in the window's title.
 * @param satelliteName The new satellite name to use
 */
void SpectrumReceiver::changeSatelliteName(QString satelliteName) {
    this->satelliteName = satelliteName;
}
