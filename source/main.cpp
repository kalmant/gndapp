
#include "dependencies/obc-packet-helpers/packethelper.h"
#include "source/audio/audioindemodulatorthread.h"
#include "source/audio/waterfallitem.h"
#include "source/command/gndconnection.h"
#include "source/connection/uploadcontroller.h"
#include "source/packet/packetdecoder.h"
#include "source/packet/packettablemodel.h"
#include "source/packet/spectrumreceiver.h"
#include "source/predict/predictercontroller.h"
#include "source/radios/ft817radio.h"
#include "source/radios/smogradio.h"
#include "source/radios/ts2000radio.h"
#include "source/radios/ft991radio.h"
#include "source/radios/icomradio.h"
#include "source/rotators/g5500rotator.h"
#include "source/sdr/sdrthread.h"
#include "source/settings/settingsholder.h"
#include "source/settings/settingsproxy.h"
#include "source/utilities/common.h"
#include "source/utilities/devicediscovery.h"
#include "source/utilities/eventfilter.h"
#include "source/utilities/logger.h"
#include "source/utilities/messageproxy.h"
#include "source/utilities/satellitechanger.h"
#include <QApplication>
#include <QDebug>
#include <QFile>
#include <QFontDatabase>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQmlDebuggingEnabler>
#include <QScopedPointer>
#include <QtQml>
#include <source/command/commandtracker.h>

MessageProxy messageProxy;
Logger logger;

// Used for logging and capturing certain messages
void myMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg) {
    QByteArray localMsg = msg.toLocal8Bit();
    switch (type) {
    case QtDebugMsg:
        fprintf(stderr, "Debug: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        logger.writeToLog("Debug: " + QString(localMsg.constData()));
        break;
    case QtInfoMsg:
        fprintf(stderr, "Info: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        logger.writeToLog("Info: " + QString(localMsg.constData()));
        break;
    case QtWarningMsg:
        if (msg.left(26).compare("QSslSocket: cannot resolve") == 0) {
            break;
        }
        fprintf(
            stderr, "Warning: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        logger.writeToLog("Warning: " + QString(localMsg.constData()));
        if ((msg.left(28)).compare(QString("QAudioInput: failed to setup")) == 0) {
            messageProxy.audioDeviceDisc();
        }
        else if ((msg.left(36)).compare(QString("QAudioInput: failed to prepare block")) == 0) {
            messageProxy.audioDeviceDisc();
        }
        else if ((msg.left(27)).compare(QString("QAudioInput: failed to open")) == 0) {
            messageProxy.audioDeviceFailedToOpen();
        }
        break;
    case QtCriticalMsg:
        fprintf(
            stderr, "Critical: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        logger.writeToLog("Critical: " + QString(localMsg.constData()));
        break;
    case QtFatalMsg:
        fprintf(stderr, "Fatal: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        logger.writeToLog("Fatal: " + QString(localMsg.constData()));
        abort();
    }
}

/**
 * @brief Creates the directory \p dirString and appends a separator after it. If the directory can not be created, \p
 * dirString is cleared.
 * @param dirString The name of the directory to create and "navigate into".
 * @param baseDir The directory that \p will be created in.
 */
void createOrClearDir(QString &dirString, const QDir &baseDir) {
    baseDir.mkdir(dirString);
    if (QDir(dirString).exists()) {
        dirString.append("/");
    }
    else {
        dirString.clear();
    }
}

Q_DECL_EXPORT int main(int argc, char *argv[]) {
    QDir currentDir("");
    QString logDirString("logs");
    createOrClearDir(logDirString, currentDir);
    QString uploadDirString("pending_uploads");
    createOrClearDir(uploadDirString, currentDir);
    QString tleDirString("tle");
    createOrClearDir(tleDirString, currentDir);
    QString downloadsDirString("downloads");
    createOrClearDir(downloadsDirString, currentDir);

    QApplication app(argc, argv);
    logger.setLogFolder(logDirString);
    qInstallMessageHandler(myMessageHandler);
    DeviceDiscovery deviceDiscovery;
    EventFilter eventFilter(&deviceDiscovery);
    app.installNativeEventFilter(&eventFilter);

    QString gndIp("192.168.0.106");
    int gndIpIndex = app.arguments().indexOf(QStringLiteral("-gnd_ip"));
    if (gndIpIndex >= 0 && app.arguments().length() > (gndIpIndex + 1)) {
        gndIp = app.arguments().at(gndIpIndex + 1);
    }
    int gndPort = 1111;
    int gndPortIndex = app.arguments().indexOf(QStringLiteral("-gnd_port"));
    if (gndPortIndex >= 0 && app.arguments().length() > (gndPortIndex + 1)) {
        QString gndPortString = app.arguments().at(gndPortIndex + 1);
        gndPort = gndPortString.toInt();
    }

    // for QSettings
    QApplication::setOrganizationName("SMOG-1 Team");
    QApplication::setApplicationName("SMOGGNDAPP");

    SettingsHolder settingsHolder;
    SettingsProxy settingsProxy(&settingsHolder);

    SatelliteChanger satelliteChanger;
    QObject::connect(&satelliteChanger, &SatelliteChanger::newFilePrefix, &logger, &Logger::changePrefix);

    qRegisterMetaType<uint8_t>("uint8_t");   // has to be declared so that it can be used in queued connections
    qRegisterMetaType<uint16_t>("uint16_t"); // has to be declared so that it can be used in queued connections
    qRegisterMetaType<uint32_t>("uint32_t"); // has to be declared so that it can be used in queued connections
    qRegisterMetaType<QList<unsigned int>>("QList<uint>");
    qRegisterMetaType<SatelliteChanger::Satellites>("Satellites");
    qmlRegisterType<WaterfallItem>("hu.timur", 1, 0, "Waterfall");
    s1obc::registerObcPacketTypesQt();

    QQmlApplicationEngine engine;

    engine.rootContext()->setContextProperty("Packet", new s1obc::PacketHelper(&app));
    engine.rootContext()->setContextProperty("satelliteChanger", &satelliteChanger);
    engine.rootContext()->setContextProperty("baseFrequency", INITIALBASEFREQUENCY);
#ifdef UPLINK_ENABLED
    engine.rootContext()->setContextProperty("uplinkEnabled", true);
#else
    engine.rootContext()->setContextProperty("uplinkEnabled", false);
#endif
    engine.rootContext()->setContextProperty("logger", &logger);
    engine.rootContext()->setContextProperty("settingsHolder", &settingsHolder);
    engine.rootContext()->setContextProperty("settingsHandler", &settingsProxy);
    engine.rootContext()->setContextProperty("gndIp", gndIp);
    engine.rootContext()->setContextProperty("gndPort", gndPort);

    QScopedPointer<PacketTableModel> ptm(new PacketTableModel(QApplication::clipboard(), &app));
    engine.rootContext()->setContextProperty("packetTableModel", ptm.data());

    QScopedPointer<QThread, QScopedPointerThreadDeleter> decoderThread(new QThread);

    SpectrumReceiver spectrumReceiver(downloadsDirString);

    PacketDecoder packetDecoder(logDirString, "packets.txt", downloadsDirString, &spectrumReceiver);
    packetDecoder.moveToThread(decoderThread.data());
    decoderThread->start();
    QObject::connect(&packetDecoder, &PacketDecoder::newPacket, ptm.data(), &PacketTableModel::newPacket);
    QObject::connect(&satelliteChanger, &SatelliteChanger::newFilePrefix, &packetDecoder, &PacketDecoder::changePrefix);
    QObject::connect(
        &satelliteChanger, &SatelliteChanger::newSatellite, &packetDecoder, &PacketDecoder::changeSatellite);
    engine.rootContext()->setContextProperty("packetDecoder", &packetDecoder);

    QObject::connect(&packetDecoder,
        &PacketDecoder::ackReceivedForSpectrum,
        &spectrumReceiver,
        &SpectrumReceiver::ackForSpectrumAnalysisCommand);
    QObject::connect(
        &packetDecoder, &PacketDecoder::newSpectrumPacket, &spectrumReceiver, &SpectrumReceiver::newSpectrumPacket);
    QObject::connect(
        &packetDecoder, &PacketDecoder::timeOutSpectrumReception, &spectrumReceiver, &SpectrumReceiver::timeOut);
    QObject::connect(
        &satelliteChanger, &SatelliteChanger::newFilePrefix, &spectrumReceiver, &SpectrumReceiver::changePrefix);
    QObject::connect(&satelliteChanger,
        &SatelliteChanger::newSpectrumTitle,
        &spectrumReceiver,
        &SpectrumReceiver::changeSatelliteName);

    PredicterController predicterController(tleDirString, static_cast<long>(settingsHolder.tsi()));
    QObject::connect(&satelliteChanger,
        &SatelliteChanger::newBaseFrequency,
        &predicterController,
        &PredicterController::newBaseFrequency);
    QObject::connect(
        &satelliteChanger, &SatelliteChanger::newFilePrefix, &predicterController, &PredicterController::changePrefix);
    engine.rootContext()->setContextProperty("predicterController", &predicterController);

    QScopedPointer<SDRThread> sdrThread(new SDRThread(&packetDecoder, &predicterController));
    sdrThread->start();
    QObject::connect(
        &satelliteChanger, &SatelliteChanger::newBaseFrequency, sdrThread.data(), &SDRThread::newBaseFrequencySlot);
    engine.rootContext()->setContextProperty("sdrThread", sdrThread.data());
    engine.rootContext()->setContextProperty("messageProxy", &messageProxy);

    UploadController uploadController(QUrl("https://gnd.bme.hu:8080/api/"),
        uploadDirString,
        "upload_queue.txt",
        "rejected_packets.txt");
    QObject::connect(&packetDecoder, &PacketDecoder::newPacketMinimal, &uploadController, &UploadController::newPacket);
    engine.rootContext()->setContextProperty("uploader", &uploadController);

    engine.rootContext()->setContextProperty("deviceDiscovery", &deviceDiscovery);
    ICOMRadio icom(&predicterController);
    engine.rootContext()->setContextProperty("icom", &icom);
    FT991Radio ft991(&predicterController);
    engine.rootContext()->setContextProperty("ft991", &ft991);
    TS2000Radio ts2000(&predicterController);
    engine.rootContext()->setContextProperty("ts2000", &ts2000);
    FT817Radio ft817(&predicterController);
    engine.rootContext()->setContextProperty("ft817", &ft817);
    QObject::connect(&satelliteChanger, &SatelliteChanger::newBaseFrequency, &ft817, &Radio::newBaseFrequency);
    SMOGRadio smogradio(&predicterController, &packetDecoder);
    engine.rootContext()->setContextProperty("smogradio", &smogradio);
    QObject::connect(&satelliteChanger, &SatelliteChanger::newBaseFrequency, &smogradio, &Radio::newBaseFrequency);
    G5500Rotator g5500(&predicterController);
    engine.rootContext()->setContextProperty("g5500", &g5500);

    GNDConnection gndConnection;
    engine.rootContext()->setContextProperty("gndConnection", &gndConnection);
    QObject::connect(&gndConnection,
        &GNDConnection::decodablePacket,
        &packetDecoder,
        &PacketDecoder::decodablePacketReceivedWithRssi);
    QObject::connect(
        &packetDecoder, &PacketDecoder::sendCommands, &gndConnection, &GNDConnection::startSendingCommands);
    QObject::connect(
        &packetDecoder, &PacketDecoder::newCommandIdSignal, &gndConnection, &GNDConnection::newCommandIdSlot);
    QObject::connect(&packetDecoder, &PacketDecoder::newPacketLength, &gndConnection, &GNDConnection::setPacketLength);
    QObject::connect(&packetDecoder, &PacketDecoder::newDataRate, &gndConnection, &GNDConnection::setDatarate);

    CommandTracker commandTracker;
    engine.rootContext()->setContextProperty("commandTracker", &commandTracker);
    QObject::connect(&gndConnection, &GNDConnection::newCommandSent, &commandTracker, &CommandTracker::newCommandSent);
    QObject::connect(&packetDecoder,
        &PacketDecoder::newCommandAcknowledged,
        &commandTracker,
        &CommandTracker::newCommandAcknowledged);

    engine.rootContext()->setContextProperty(
        "defaultMonotypeFontFamily", QFontDatabase::systemFont(QFontDatabase::FixedFont).family());

    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    if (engine.rootObjects().isEmpty()) {
        qWarning() << "Could not load QML file, quitting. Are you sure you are running a necessary Qt version?";
        return 1;
    }

    WaterfallItem *wtfptr =
        engine.rootObjects().at(0)->findChild<WaterfallItem *>("waterfallObject", Qt::FindChildrenRecursively);

    QScopedPointer<AudioInDemodulatorThread> adem1250Thread(new AudioInDemodulatorThread(&packetDecoder));

    QObject::connect(wtfptr->getAS(),
        &AudioSampler::audioSamplesReadyFor1250,
        adem1250Thread.data(),
        &AudioInDemodulatorThread::demodulate);

    adem1250Thread->start();

#if defined(Q_OS_LINUX)
    // Initializing notifier on linux
    QTimer::singleShot(0, &deviceDiscovery, &DeviceDiscovery::initializeNotifier);
#elif defined(Q_OS_WIN)
#else
    // Starting device detection on OS X
    QTimer::singleShot(0, &deviceDiscovery, &DeviceDiscovery::startTimer);
#endif

    return app.exec();
}
