TEMPLATE = app

QT += qml quick widgets multimedia network serialport charts quickcontrols2

equals(QT_MAJOR_VERSION, 5) {} else {
    error("5 is the required major version.")
}
lessThan(QT_MINOR_VERSION, 12) {
    error("Minor version 12 is the minimum required version.")
}
equals(QT_MINOR_VERSION, 12):lessThan(QT_PATCH_VERSION, 6) {
    error("Patch version 6 is the minimum required version for 5.12.")
}
equals(QT_MINOR_VERSION, 13):lessThan(QT_PATCH_VERSION, 2) {
    error("Patch version 2 is the minimum required version for 5.13.")
}

CONFIG += c++11 resources_big \


DEFINES += \
    QT_DEPRECATED_WARNINGS \
    # S1_USE_COUT_DEBUGGING \ # I've marked this unnecessary on 2019.10.23. since I've found no reference to it. If you run into problems you may need to readd it.
    CBC=0 \ # CBC = 0 and ECB = 1 are required for uplink communication
    ECB=1 \ # CBC = 0 and ECB = 1 are required for uplink communication
    # UPLINK_ENABLED \ # Feature flag for uplink features
    # CHECK_SIGNATURE \ # Check signatures for received packets
    # S1GND_USE_CONFIDENTIAL_REPO \ # SMOG 1 confidential repo flag
    # SPGND_USE_CONFIDENTIAL_REPO \ # SMOG P confidential repo flag
    # A1GND_USE_CONFIDENTIAL_REPO \ # ATL 1 confidential repo flag
    INITIALBASEFREQUENCY=437345000 \

RESOURCES += source/qml/qml.qrc

INCLUDEPATH += "$$PWD/dependencies/3rdparty/rtlsdr"

QMAKE_CXXFLAGS += -Wno-type-limits
QMAKE_CXXFLAGS += -Wall -Wextra -Wpedantic -Wunreachable-code #-Wshadow  #-Wswitch-default  -Wconversion

win32 {
    LIBS += -L"$$_PRO_FILE_PWD_/dependencies/3rdparty/fftw3" -lfftw3 \
            -L$$PWD/dependencies/3rdparty/libusb -lusb-1.0 \
            -L$$PWD/dependencies/3rdparty/openssl -lcrypto-1_1 # You need to acquire this on your own

    INCLUDEPATH +=  "$$_PRO_FILE_PWD_/dependencies/3rdparty/fftw3" \
                    "$$PWD/dependencies/3rdparty/libusb" \
                    "$$PWD/dependecies/3rdparty/openssl" # You need to acquire this on your own

    DEPENDPATH += $$PWD/dependencies/3rdparty/libusb
    PRE_TARGETDEPS += $$PWD/dependencies/3rdparty/libusb/libusb-1.0.a
}

# Windows icon
RC_ICONS = "icon/smoggnd.ico"

linux {
    CONFIG += link_pkgconfig
    PKGCONFIG += libusb-1.0 fftw3 libudev openssl
}

linux-g++ {
    QMAKE_CXXFLAGS -= -g
    QMAKE_CXXFLAGS_DEBUG -= -g
    QMAKE_CXXFLAGS_DEBUG += -g3
    QMAKE_CXXFLAGS_RELEASE += -fvisibility-inlines-hidden -fvisibility=hidden
    # Clear default rpath set by qmake
    QMAKE_LFLAGS_RPATH =
    # Set rpath relative to the location of the executable
    QMAKE_LFLAGS += "-Wl,-rpath,\'\$$ORIGIN/lib\'"

    # To profile with LTTNG, add:
    #QMAKE_CXXFLAGS += -fno-omit-frame-pointer -finstrument-functions -finstrument-functions-exclude-file-list=/usr/include
    #CONFIG += force_debug_info
}

mac {
ICON = $$PWD/icon/smoggnd.icns
#packages installed with Homebrew
    LIBS += -L/usr/local/Cellar/libusb/1.0.23/lib -lusb-1.0 \
            -L/usr/local/Cellar/fftw/3.3.8_1/lib -lfftw3 \
            -L/usr/local/Cellar/openssl@1.1/1.1.1d/lib -lssl

    INCLUDEPATH +=  "$$_PRO_FILE_PWD_/dependencies/3rdparty/fftw3" \
                    "$$PWD/dependencies/3rdparty/libusb"
}

HEADERS += \
    dependencies/obc-packet-helpers/helper.h \
    source/audio/audioindemodulator.h \
    source/audio/audioindemodulatorthread.h \
    source/audio/audiosampler.h \
    source/command/commandtracker.h \
    source/command/commandqueue.h \
    source/command/gndconnection.h \
    source/connection/packetsjsonwrapper.h \
    source/connection/uploadcontroller.h \
    source/connection/uploadworker.h \
    source/demod/newsmog1dem.h \
    source/packet/chartwindow.h \
    source/packet/decodedpacket.h \
    source/packet/filedownload.h \
    source/packet/packetdecoder.h \
    source/packet/packettablemodel.h \
    source/packet/syncpacket.h \
    source/packet/spectrumreceiver.h \
    source/predict/predictercontroller.h \
    source/predict/predicterworker.h \
    source/predict/predictmod.h \
    source/radios/ft817radio.h \
    source/radios/ft847radio.h \
    source/radios/ft991radio.h \
    source/radios/icomradio.h \
    source/radios/radio.h \
    source/radios/smogradio.h \
    source/radios/ts2000radio.h \
    source/rotators/g5500rotator.h \
    source/rotators/rotator.h \
    source/sdr/sdrthread.h \
    source/sdr/sdrworker.h \
    source/settings/settingsholder.h \
    source/settings/settingsproxy.h \
    source/utilities/common.h \
    source/utilities/devicediscovery.h \
    source/utilities/eventfilter.h \
    source/utilities/logger.h \
    source/utilities/messageproxy.h \
    source/utilities/satellitechanger.h \
    source/utilities/serialporthandler.h \
    dependencies/3rdparty/rtlsdr/rtl-sdr.h \
    dependencies/3rdparty/rtlsdr/rtl-sdr_export.h \
    dependencies/3rdparty/rtlsdr/tuner_e4k.h \
    dependencies/3rdparty/rtlsdr/tuner_fc0012.h \
    dependencies/3rdparty/rtlsdr/tuner_fc0013.h \
    dependencies/3rdparty/rtlsdr/tuner_fc2580.h \
    dependencies/3rdparty/rtlsdr/tuner_r82xx.h \
    dependencies/3rdparty/rtlsdr/convenience.h \
    dependencies/3rdparty/rtlsdr/reg_field.h \
    dependencies/3rdparty/rtlsdr/rtlsdr_i2c.h \
    dependencies/FEC-AO40/ao40-short/decode/ao40short_decode_message.h \
    dependencies/FEC-AO40/ao40-short/decode/ao40short_decode_rs.h \
    dependencies/FEC-AO40/ao40-short/decode/ao40short_spiral-vit_scalar_1280.h \
    dependencies/FEC-AO40/ao40/decode/ao40_decode_message.h \
    dependencies/FEC-AO40/ao40/decode/ao40_spiral-vit_scalar.h \
    dependencies/FEC-AO40/ao40/decode/ao40_decode_rs.h \
    dependencies/obc-packet-helpers/int24.h \
    dependencies/obc-packet-helpers/bitfield.h \
    dependencies/obc-packet-helpers/pack.h \
    dependencies/obc-packet-helpers/telemetry.h \
    dependencies/obc-packet-helpers/uplink.h \
    dependencies/obc-packet-helpers/downlink.h \
    dependencies/obc-packet-helpers/packethelper.h \
    dependencies/racoder/ra_config.h \
    dependencies/racoder/ra_decoder_gen.h \
    dependencies/racoder/ra_lfsr.h \
    source/visualization/spectogram.h

SOURCES += \
    source/main.cpp \
    source/audio/audioindemodulator.cpp \
    source/audio/audioindemodulatorthread.cpp \
    source/audio/audiosampler.cpp \
    source/command/commandtracker.cpp \
    source/command/commandqueue.cpp \
    source/command/gndconnection.cpp \
    source/connection/packetsjsonwrapper.cpp \
    source/connection/uploadcontroller.cpp \
    source/connection/uploadworker.cpp \
    source/demod/newsmog1dem.cpp \
    source/packet/chartwindow.cpp \
    source/packet/decodedpacket.cpp \
    source/packet/filedownload.cpp \
    source/packet/packetdecoder.cpp \
    source/packet/packettablemodel.cpp \
    source/packet/spectrumreceiver.cpp \
    source/packet/syncpacket.cpp \
    source/predict/predictercontroller.cpp \
    source/predict/predicterworker.cpp \
    source/predict/predictmod.cpp \
    source/radios/ft817radio.cpp \
    source/radios/ft847radio.cpp \
    source/radios/ft991radio.cpp \
    source/radios/icomradio.cpp \
    source/radios/radio.cpp \
    source/radios/smogradio.cpp \
    source/radios/ts2000radio.cpp \
    source/rotators/g5500rotator.cpp \
    source/rotators/rotator.cpp \
    source/sdr/sdrthread.cpp \
    source/sdr/sdrworker.cpp \
    source/settings/settingsholder.cpp \
    source/settings/settingsproxy.cpp \
    source/utilities/common.cpp \
    source/utilities/devicediscovery.cpp \
    source/utilities/eventfilter.cpp \
    source/utilities/logger.cpp \
    source/utilities/messageproxy.cpp \
    source/utilities/satellitechanger.cpp \
    source/utilities/serialporthandler.cpp \
    dependencies/3rdparty/rtlsdr/librtlsdr.c \
    dependencies/3rdparty/rtlsdr/tuner_e4k.c \
    dependencies/3rdparty/rtlsdr/tuner_fc0012.c \
    dependencies/3rdparty/rtlsdr/tuner_fc0013.c \
    dependencies/3rdparty/rtlsdr/tuner_fc2580.c \
    dependencies/3rdparty/rtlsdr/tuner_r82xx.c \
    dependencies/3rdparty/rtlsdr/convenience.c \
    dependencies/FEC-AO40/ao40-short/decode/ao40short_decode_message.c \
    dependencies/FEC-AO40/ao40-short/decode/ao40short_decode_rs.c \
    dependencies/FEC-AO40/ao40-short/decode/ao40short_spiral-vit_scalar_1280.c \
    dependencies/FEC-AO40/ao40/decode/ao40_decode_message.c \
    dependencies/FEC-AO40/ao40/decode/ao40_spiral-vit_scalar.c \
    dependencies/FEC-AO40/ao40/decode/ao40_decode_rs.c \
    dependencies/obc-packet-helpers/packethelper.cpp \
    dependencies/racoder/ra_config.c \
    dependencies/racoder/ra_decoder_gen.c \
    dependencies/racoder/ra_lfsr.c \
   source/visualization/spectogram.cpp
