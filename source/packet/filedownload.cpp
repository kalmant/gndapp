#include "filedownload.h"
#include "dependencies/obc-packet-helpers/uplink.h"
#include <QDataStream>

using namespace s1utils;
using namespace s1obc;

/**
 * @brief Populates \p missingFragments_priv.
 * One number is added for every segment (its index) in \p dataVector_priv that is equal to \p MISSINGFRAGMENT_PRIV
 */
void FileDownload::populateMissingFragments() {
    missingFragments_priv.clear();
    unsigned int i = 0;
    for (const auto &d : dataVector_priv) {
        if (d == MISSINGFRAGMENT_PRIV) {
            missingFragments_priv.append(QString::number(i));
        }
    }
}

/**
 * @brief Resets the variables in the object. FileDownload MUST NOT BE REUSED!
 */
void FileDownload::reset() {
    QVector<QByteArray>().swap(dataVector_priv);
    QStringList().swap(missingFragments_priv);
    fragmentCount_priv = 0;
    nextIndex_priv = 0;
    fileType_priv = s1obc::FileType_Deleted;
    fileName_priv = "UNINITIALIZED.invalidfile";
}

/**
 * @brief Constructs the object with the given parameters. FileDownload objects MUST NOT BE REUSED!
 * @param fileName The filename's first part
 * @param fragmentCount Number of fragments expected to be in the file
 * @param fileType the type of file that is being downloaded
 */
FileDownload::FileDownload(QString fileName, uint16_t fragmentCount, s1obc::FileType fileType, QObject *parent)
: QObject(parent) {
    fragmentCount_priv = fragmentCount;
    dataVector_priv.fill(MISSINGFRAGMENT_PRIV, fragmentCount);
    nextIndex_priv = 0;
    fileType_priv = fileType;
    fileName_priv = fileName + ".satfile";
    partialFileName_priv = fileName_priv + ".partial";
}

/**
 * @brief Sets the \p indexth element of FileDownload::dataVector_priv to data (if possible)
 * @param index Index of \p data in the whole file
 * @param data The data
 */
void FileDownload::addFragment(uint16_t index, QByteArray data) {
    if (index >= fragmentCount_priv) {
        qWarning() << "Fragment index should be less(!) than packetCount_priv";
        return;
    }
    if (index < nextIndex_priv) {
        qWarning() << "Out of order fragment, expected no." + QString::number(nextIndex_priv) + "but received no." +
                          QString::number(index);
    }
    nextIndex_priv = index + 1;
    dataVector_priv[index] = data;
}

/**
 * @brief Writes the contents of \p partialFileName_priv to \p partialFileName_priv
 *
 * The data segments are written on separate lines, in order, as QByteArrays without any formatting whatsoever.
 */
void FileDownload::writePartialFile() const {
    QFile file(partialFileName_priv);
    if (file.open(QFile::WriteOnly | QFile::Truncate)) {
        QTextStream out(&file);
        for (const QByteArray &d : dataVector_priv) {
            out << d << "\n";
        }
        out << "Missing fragment indices: " + missingFragments_priv.join(", ") << "\n";
        file.flush();
        file.close();
    }
    else {
        qWarning() << "Could not open file" + partialFileName_priv + "for writing, discarding data";
    }
}

void FileDownload::writeTextFile(QTextStream &out, const QByteArray &data) {
    out << QString::fromLatin1(data);
}

void FileDownload::writeSpectrumFile(QTextStream &out, const QByteArray &data) {
    struct SpectrumFileHeader {
        uint32_t timestamp;
        uint32_t startFrequency;
        uint32_t stopFrequency;
        uint32_t stepSize;
        uint16_t uplinkSerial;
        uint16_t measurementId;
        uint8_t rbw;
    } S1_PACKED;

    const uint8_t *rawbytes = reinterpret_cast<const uint8_t *>(data.data());
    const SpectrumFileHeader *header = reinterpret_cast<const SpectrumFileHeader *>(rawbytes);

    bool missingHeader = missingFragments_priv.length() > 0 && MISSINGFRAGMENT_PRIV.at(0) == QString::number(0);

    auto timestamp = header->timestamp;
    auto startFrequency = header->startFrequency;
    auto stepSize = header->stepSize;
    auto measurementId = header->measurementId;
    auto rbw = header->rbw;

    if (missingHeader) {
        out << QStringLiteral(
                   "The fragment containing the SpectrumFileHeader is missing, measurement bytes are as follows")
            << QStringLiteral("\n");
    }
    else {
        out << QStringLiteral("time stamp: ") + QString::number(header->timestamp) << QStringLiteral("\n");
        out << QStringLiteral("start frequency: ") + QString::number(header->startFrequency) << QStringLiteral("\n");
        out << QStringLiteral("stop frequency: ") + QString::number(header->stopFrequency) << QStringLiteral("\n");
        out << QStringLiteral("step size: ") + QString::number(header->stepSize) << QStringLiteral("\n");
        out << QStringLiteral("rbw: ") + QString::number(header->rbw) << QStringLiteral("\n");
        out << QStringLiteral("uplink serial: ") + QString::number(header->uplinkSerial) << QStringLiteral("\n");
        out << QStringLiteral("measurement id: ") + QString::number(header->measurementId) << QStringLiteral("\n");
    }
    rawbytes += sizeof(SpectrumFileHeader); // The header will be skipped
    int len = data.length() - static_cast<int>(sizeof(SpectrumFileHeader));
    if (len < 0) {
        qWarning() << "Downloaded spectrum file is shorter than a SpectrumFileHeader";
        return;
    }

    for (unsigned int i = 0; i < static_cast<unsigned int>(len); i++) {
        out << QString("%1: %2").arg(startFrequency + i * stepSize).arg((static_cast<uint8_t>(rawbytes[i]) / 2) - 131)
            << "\n";
    }

    out.flush();

    // If the header is missing, then we can not draw because we don't have the header
    if (!missingHeader) {
        emit newSpectrumFile(QByteArray(reinterpret_cast<const char *>(rawbytes), len),
            timestamp,
            measurementId,
            startFrequency,
            stepSize,
            rbw);
    }
    else {
        qWarning() << "The downloaded spectrum measurement has not been printed to a file since its header is missing";
    }
}

QDataStream &operator>>(QDataStream &stream, s1_uint24_t &u24) {
    u24 = 0;
    uint8_t u8 = 0;

    stream >> u8;
    u24.setByte(0, u8);
    stream >> u8;
    u24.setByte(1, u8);
    stream >> u8;
    u24.setByte(2, u8);

    return stream;
}

void FileDownload::writeUniversalMeasurementFile(QTextStream &out, const QByteArray &data) {
    QDataStream stream(data);
    stream.setByteOrder(QDataStream::LittleEndian);

    uint32_t timestamp;
    stream >> timestamp;
    uint16_t measurement_id;
    stream >> measurement_id;
    uint16_t durationMin;
    stream >> durationMin;
    MeasurementSelectionSolar::BasicType solarBits;
    stream >> solarBits;
    MeasurementSelectionPcu::BasicType pcuBits;
    stream >> pcuBits;
    MeasurementSelectionMpu::BasicType mpuBits;
    stream >> mpuBits;
    MeasurementSelectionObcCom::BasicType obcComBits;
    stream >> obcComBits;
    MeasurementSelectionTid::BasicType tidBits;
    stream >> tidBits;
    uint8_t intervalSolarSec;
    stream >> intervalSolarSec;
    uint8_t intervalPcuSec;
    stream >> intervalPcuSec;
    uint8_t intervalMpuDecisec;
    stream >> intervalMpuDecisec;
    uint8_t intervalObcComSec;
    stream >> intervalObcComSec;
    uint8_t intervalTid5Min;
    stream >> intervalTid5Min;

    bool hasSolar = solarBits && intervalSolarSec;
    bool hasPcu = pcuBits && intervalPcuSec;
    bool hasMpu = mpuBits && intervalMpuDecisec;
    bool hasObcCom = obcComBits && intervalObcComSec;
    bool hasTid = tidBits && intervalTid5Min;
    uint32_t intervalSolarMs = intervalSolarSec * 1000;
    uint32_t intervalPcuMs = intervalPcuSec * 1000;
    uint32_t intervalMpuMs = intervalMpuDecisec * 100;
    uint32_t intervalObcComMs = intervalObcComSec * 1000;
    uint32_t intervalTidMs = intervalTid5Min * 5 * 60 * 1000;

    MeasurementSelectionSolar solar;
    solar.load(solarBits);
    MeasurementSelectionPcu pcu;
    pcu.load(pcuBits);
    MeasurementSelectionMpu mpu;
    mpu.load(mpuBits);
    MeasurementSelectionObcCom obcCom;
    obcCom.load(obcComBits);
    MeasurementSelectionTid tid;
    tid.load(tidBits);

    uint32_t interval = 1000;
    if (hasMpu && (intervalMpuMs % 1000) != 0) {
        interval = 100;
    }

    uint32_t durationMs = durationMin * 60 * 1000;

    uint8_t u8;
    int16_t i16;
    uint16_t u16;
    s1_uint24_t u24;

    for (uint32_t elapsedMs = interval; elapsedMs < durationMs; elapsedMs += interval) {
        bool processSolar = (hasSolar && (elapsedMs % intervalSolarMs) == 0);
        bool processPcu = (hasPcu && (elapsedMs % intervalPcuMs) == 0);
        bool processMpu = (hasMpu && (elapsedMs % intervalMpuMs) == 0);
        bool processObcCom = (hasObcCom && (elapsedMs % intervalObcComMs) == 0);
        bool processTid = (hasTid && (elapsedMs % intervalTidMs) == 0);

        bool processAnything = processSolar || processPcu || processMpu || processObcCom || processTid;

        if (processAnything) {
            out << QStringLiteral("========== Elapsed time: ") << elapsedMs << "\n";
        }

        if (processSolar) {
            for (int solarid = 1; solarid <= 6; solarid++) {
                if (solar.selectTemperature()) {
                    stream >> i16;
                    out << QStringLiteral("Solar panel ") << solarid << " temperature: " << i16 / 10.0 << " C\n";
                }
                if (solar.selectLightSensor()) {
                    stream >> u16;
                    out << QStringLiteral("Solar panel ") << solarid << " light sensor: " << u16 << "\n";
                }
                if (solar.selectInputCurrent()) {
                    stream >> u16;
                    out << QStringLiteral("Solar panel ") << solarid << " input current: " << u16 << " mA\n";
                }
                if (solar.selectInputVoltage()) {
                    stream >> u16;
                    out << QStringLiteral("Solar panel ") << solarid << " input voltage: " << u16 << " mV\n";
                }
                if (solar.selectOutputCurrent()) {
                    stream >> u16;
                    out << QStringLiteral("Solar panel ") << solarid << " output current: " << u16 << " mA\n";
                }
                if (solar.selectOutputVoltage()) {
                    stream >> u16;
                    out << QStringLiteral("Solar panel ") << solarid << " output voltage: " << u16 << " mV\n";
                }
            }
        }
        if (processPcu) {
            int pcuNumber = pcu.usePcu2() ? 2 : 1;

            if (pcu.selectSdc1InputCurrent()) {
                stream >> u16;
                out << QStringLiteral("PCU") << pcuNumber << " input current: " << u16 << " mA\n";
            }
            if (pcu.selectSdc1OutputCurrent()) {
                stream >> u16;
                out << QStringLiteral("PCU") << pcuNumber << " output current: " << u16 << " mA\n";
            }
            if (pcu.selectSdc1OutputVoltage()) {
                stream >> u16;
                out << QStringLiteral("PCU") << pcuNumber << " output voltage: " << u16 << " mV\n";
            }
            if (pcu.selectSdc2InputCurrent()) {
                stream >> u16;
                out << QStringLiteral("PCU") << pcuNumber << " input current: " << u16 << " mA\n";
            }
            if (pcu.selectSdc2OutputCurrent()) {
                stream >> u16;
                out << QStringLiteral("PCU") << pcuNumber << " output current: " << u16 << " mA\n";
            }
            if (pcu.selectSdc2OutputVoltage()) {
                stream >> u16;
                out << QStringLiteral("PCU") << pcuNumber << " output voltage: " << u16 << " mV\n";
            }
            if (pcu.selectRegulatedBusVoltage()) {
                stream >> u16;
                out << QStringLiteral("PCU") << pcuNumber << " regulated bus voltage: " << u16 << " mV\n";
            }
            if (pcu.selectUnregulatedBusVoltage()) {
                stream >> u16;
                out << QStringLiteral("PCU") << pcuNumber << " unregulated bus voltage: " << u16 << " mV\n";
            }
            if (pcu.selectObc1Current()) {
                stream >> u16;
                out << QStringLiteral("PCU") << pcuNumber << " OBC1 current: " << u16 << " mA\n";
            }
            if (pcu.selectObc2Current()) {
                stream >> u16;
                out << QStringLiteral("PCU") << pcuNumber << " OBC2 current: " << u16 << " mA\n";
            }
            if (pcu.selectBatteryVoltage()) {
                stream >> u16;
                out << QStringLiteral("PCU") << pcuNumber << " battery voltage: " << u16 << " mV\n";
            }
            if (pcu.selectBatteryChargeCurrent()) {
                stream >> u16;
                out << QStringLiteral("PCU") << pcuNumber << " battery charge current: " << u16 << " mA\n";
            }
            if (pcu.selectBatteryDischargeCurrent()) {
                stream >> u16;
                out << QStringLiteral("PCU") << pcuNumber << " battery dischage current: " << u16 << " mA\n";
            }
            if (pcu.selectBatteryChargeEnabled()) {
                stream >> u8;
                out << QStringLiteral("PCU") << pcuNumber << " battery charge current: " << (int) u8 << "\n";
            }
            if (pcu.selectBatteryDischargeEnabled()) {
                stream >> u8;
                out << QStringLiteral("PCU") << pcuNumber << " battery dischage current: " << (int) u8 << "\n";
            }
        }
        if (processMpu) {
            int16_t x, y, z;

            if (mpu.selectMpu1Temperature()) {
                stream >> i16;
                out << QStringLiteral("MPU1 temperature: ") << i16 / 10.0 << " C\n";
            }
            if (mpu.selectMpu1Gyroscope()) {
                stream >> x >> y >> z;
                out << QStringLiteral("MPU1 gyroscope: X=") << x << " Y=" << y << " Z=" << z << "\n";
            }
            if (mpu.selectMpu1Magnetometer()) {
                stream >> x >> y >> z;
                out << QStringLiteral("MPU1 magnetometer: X=") << x << " Y=" << y << " Z=" << z << "\n";
            }
            if (mpu.selectMpu2Temperature()) {
                stream >> i16;
                out << QStringLiteral("MPU2 temperature: ") << i16 / 10.0 << " C\n";
            }
            if (mpu.selectMpu2Gyroscope()) {
                stream >> x >> y >> z;
                out << QStringLiteral("MPU2 gyroscope: X=") << x << " Y=" << y << " Z=" << z << "\n";
            }
            if (mpu.selectMpu2Magnetometer()) {
                stream >> x >> y >> z;
                out << QStringLiteral("MPU2 magnetometer: X=") << x << " Y=" << y << " Z=" << z << "\n";
            }
        }
        if (processObcCom) {
            if (obcCom.selectRtcc1Temperature()) {
                stream >> i16;
                out << QStringLiteral("RTCC1 temperature: ") << i16 / 10.0 << " C\n";
            }
            if (obcCom.selectRtcc2Temperature()) {
                stream >> i16;
                out << QStringLiteral("RTCC2 temperature: ") << i16 / 10.0 << " C\n";
            }
            if (obcCom.selectObcTemperature()) {
                stream >> i16;
                out << QStringLiteral("OBC temperature: ") << i16 / 10.0 << " C\n";
            }
            if (obcCom.selectComCurrent()) {
                stream >> u16;
                out << QStringLiteral("COM current: ") << u16 << " mA\n";
            }
            if (obcCom.selectEps2aTemperature1()) {
                stream >> i16;
                out << QStringLiteral("EPS2A temperature#1: ") << i16 / 10.0 << " C\n";
            }
            if (obcCom.selectEps2aTemperature2()) {
                stream >> i16;
                out << QStringLiteral("EPS2A temperature#2: ") << i16 / 10.0 << " C\n";
            }
            if (obcCom.selectComVoltage()) {
                stream >> u16;
                out << QStringLiteral("COM voltage: ") << u16 << " mV\n";
            }
            if (obcCom.selectComTemperature()) {
                stream >> i16;
                out << QStringLiteral("COM temperature: ") << i16 / 10.0 << " C\n";
            }
            if (obcCom.selectSpectrumTemperature()) {
                stream >> i16;
                out << QStringLiteral("Spectrum analyzer temperature: ") << i16 / 10.0 << " C\n";
            }
        }

        if (processTid) {
            if (tid.selectTid1Temperature()) {
                stream >> i16;
                out << QStringLiteral("TID1 temperature: ") << i16 / 10.0 << " C\n";
            }
            if (tid.selectTid1Serial()) {
                stream >> u16;
                out << QStringLiteral("TID1 serial: ") << u16 << "\n";
            }
            if (tid.selectTid1Radfet1()) {
                stream >> u24;
                out << QStringLiteral("TID1 RADFET1: ") << u24 << "uV\n";
            }
            if (tid.selectTid1Radfet2()) {
                stream >> u24;
                out << QStringLiteral("TID1 RADFET2: ") << u24 << "uV\n";
            }
            if (tid.selectTid1Voltage()) {
                stream >> u16;
                out << QStringLiteral("TID2 voltage: ") << u16 << "mV\n";
            }
            if (tid.selectTid2Temperature()) {
                stream >> i16;
                out << QStringLiteral("TID2 temperature: ") << i16 / 10.0 << " C\n";
            }
            if (tid.selectTid2Serial()) {
                stream >> u16;
                out << QStringLiteral("TID2 serial: ") << u16 << "\n";
            }
            if (tid.selectTid2Radfet1()) {
                stream >> u24;
                out << QStringLiteral("TID2 RADFET1: ") << u24 << "uV\n";
            }
            if (tid.selectTid2Radfet2()) {
                stream >> u24;
                out << QStringLiteral("TID2 RADFET2: ") << u24 << "uV\n";
            }
            if (tid.selectTid2Voltage()) {
                stream >> u16;
                out << QStringLiteral("TID2 voltage: ") << u16 << "mV\n";
            }
        }

        out.flush();
    }
}

void FileDownload::writeInfoTableFile(QTextStream &out, const QByteArray &data) {
    // Struct copied from SMOG OBC source and replaced size_t with uint32_t
    struct s1fs_fileinfo_data {
        char name[FileName::length];
        uint8_t type;
        uint32_t timestamp;
        uint32_t size;
    } S1_PACKED;

    for (int start = 0, i = 0; start < data.length(); i += 1, start += sizeof(s1fs_fileinfo_data)) {
        int end = start + sizeof(s1fs_fileinfo_data);
        if (data.length() < end) {
            break;
        }

        s1fs_fileinfo_data info;
        memcpy(&info, data.data() + start, sizeof(s1fs_fileinfo_data));
        QDateTime timestamp = QDateTime::fromSecsSinceEpoch(info.timestamp);
        QString timeString = timestamp.toUTC().toString(Qt::ISODate);

        out << QStringLiteral("========== File info ") << QString::number(i) << "\n";
        out << QStringLiteral("name: ") << QString::fromLatin1(info.name, info.name[9] == 0 ? (-1) : 10) << "\n";
        out << QStringLiteral("type: ") << QString::number(info.type) << "\n";
        out << QStringLiteral("timestamp: ") << QString::number(info.timestamp) << " | " << timeString << "\n";
        out << QStringLiteral("size: ") << QString::number(info.size) << "\n";
        out << "\n";

        out.flush();
    }
}

/**
 * @brief Writes \p dataVector_priv to \p fileName_priv, in the appropriate format depending on \p fileType_priv.
 * @return True if the data was successfully written to \p fileName_priv.
 */
bool FileDownload::writeCompleteFile() {
    QFile file(fileName_priv);
    if (file.open(QFile::WriteOnly | QFile::Truncate)) {
        QByteArray data;
        for (const auto &d : dataVector_priv) {
            data.append(d);
        }
        QTextStream out(&file);
        switch (fileType_priv) {
        case FileType_Deleted:
            // Intentionally NO OPERATION! This package type should never appear on the ground.
            qWarning() << "Received deleted file, this should never happen";
            break;
        case FileType_Text:
            writeTextFile(out, data);
            out.flush();
            break;
        case FileType_Spectrum:
            writeSpectrumFile(out, data);
            out.flush();
            break;
        case FileType_UniversalMeasurement:
            writeUniversalMeasurementFile(out, data);
            out.flush();
            break;
        case FileType_InfoTable:
            writeInfoTableFile(out, data);
            out.flush();
            break;
        default:
            file.write(data);
            file.flush();
            break;
        }

        file.close();
        return true;
    }
    else {
        qWarning() << "Could not open file" + fileName_priv + "for writing, discarding data";
        return false;
    }
}

/**
 * @brief Writes \p dataVector_priv to a file. If the download was not complete, then a partial file is created.
 * @return Returns True if the download was completed and was successfully written to \p fileName_priv
 */
bool FileDownload::writeToFile() {
    populateMissingFragments();
    if (missingFragments_priv.length() > 0) {
        attemptLoadFromPartialFile();
        populateMissingFragments();
    }
    if (missingFragments_priv.length() > 0) {
        writePartialFile();
        return false;
    }
    else {
        bool result = writeCompleteFile();
        if (result) {
            QFile::remove(partialFileName_priv);
        }
        return result;
    }
}

/**
 * @brief Tries to load existing partial data from \p partialFileName_priv
 */
void FileDownload::attemptLoadFromPartialFile() {
    if (!(QFileInfo::exists(partialFileName_priv) && QFileInfo(partialFileName_priv).isFile())) {
        return; // No partial file exists yet, won't try to load it
    }

    QFile file(partialFileName_priv);
    if (file.open(QFile::ReadOnly)) {
        QTextStream in(&file);
        unsigned int index = 0;
        while (!in.atEnd()) {
            if (index >= fragmentCount_priv) {
                file.close();
                return;
            }
            QString line = in.readLine();
            QByteArray data = line.toLocal8Bit();
            if (data != MISSINGFRAGMENT_PRIV) {
                dataVector_priv[static_cast<int>(index)] = line.toLocal8Bit();
            }
            index++;
        }
        file.close();
    }
    else {
        qWarning()
            << "Could not open file" + partialFileName_priv +
                   "for reading, will not load partial data! It will also be overwritten if the write is successful!";
    }
}

/**
 * @brief Creates the file and calls FileDownload::reset() and then sets FileDownload::isFinished to true.
 */
bool FileDownload::finishUpFile() {
    // The file is already finished or it was not even started
    if (isFinished || (0 == nextIndex_priv)) {
        return false;
    }
    if (fileType_priv == s1obc::FileType_Deleted) {
        qInfo() << "Downloaded a deleted file, it will not be created";
        reset();
        isFinished = true;
        return false;
    }
    bool result = writeToFile();
    if (result) {
        qInfo() << "File download successful," << fileName_priv << "has been created.";
    }
    else {
        qInfo() << "File download unsuccessful," << partialFileName_priv << "may have been created.";
    }
    reset();
    isFinished = true;
    return result;
}
