
import QtQuick 2.9
import QtQuick.Controls 2.2
import s1obc 1.0
import "subsystems/smog1" as Smog1
import "subsystems/smogp" as SmogP
import "subsystems/atl1" as Atl1

Column {
    id: statusPage

    anchors.fill: parent
    anchors.margins: 10
    spacing: 10

    Label {
        id: titleLabel

        width: parent.width
        x: loader.x
        text: loader.item.title
        font.pixelSize: Qt.application.font.pixelSize * 1.2
        font.bold: true
    }

    Row {
        width: parent.width
        height: parent.height - titleLabel.height - parent.spacing
        spacing: 20

        ButtonGroup {
            buttons: buttonsColumn.children
        }

        ScrollView {
            height: parent.height
            width: buttonsColumn.width
            clip: true

            Column {
                id: buttonsColumn

                spacing: 10

                Button {
                    text: "OBC"
                    checkable: true
                    checked: true
                    onCheckedChanged: if (checked) loader.sourceComponent = [obcComponent_smog1, obcComponent_smogp, obcComponent_atl1][mainWindow.currentSatellite]
                }
                Button {
                    text: "COM"
                    checkable: true
                    onCheckedChanged: if (checked) loader.sourceComponent = [comComponent_smog1, comComponent_smogp, comComponent_atl1][mainWindow.currentSatellite]
                }
                Button {
                    text: "PCU"
                    checkable: true
                    onCheckedChanged: if (checked) loader.sourceComponent = [pcuComponent_smog1, pcuComponent_smogp, pcuComponent_atl1][mainWindow.currentSatellite]
                }
                Button {
                    text: mainWindow.currentSatellite === 2 ? "Battery" : "TID"
                    checkable: true
                    visible: enabled
                    onCheckedChanged: if (checked) loader.sourceComponent = [tidComponent_smog1, tidComponent_smogp, battComponent_atl1][mainWindow.currentSatellite]
                }
                Button {
                    text: "Solar"
                    checkable: true
                    onCheckedChanged: if (checked) loader.sourceComponent = [solarComponent_smog1, solarComponent_smogp, solarComponent_atl1][mainWindow.currentSatellite]
                }
            }
        }

        Loader {
            id: loader

            sourceComponent: [obcComponent_smog1, obcComponent_smogp, obcComponent_atl1][mainWindow.currentSatellite]
            width: parent.width
                   - buttonsColumn.width
                   - logo.width
                   - parent.spacing * 2
            height: parent.height

            Component {
                id: obcComponent_smog1
                Smog1.ObcSubsystem {}
            }
            Component {
                id: obcComponent_smogp
                SmogP.ObcSubsystem {}
            }
            Component {
                id: obcComponent_atl1
                Atl1.ObcSubsystem {}
            }
            Component {
                id: comComponent_smog1
                Smog1.ComSubsystem {}
            }
            Component {
                id: comComponent_smogp
                SmogP.ComSubsystem {}
            }
            Component {
                id: comComponent_atl1
                Atl1.ComSubsystem {}
            }
            Component {
                id: pcuComponent_smog1
                Smog1.PcuSubsystem {}
            }
            Component {
                id: pcuComponent_smogp
                SmogP.PcuSubsystem {}
            }
            Component {
                id: pcuComponent_atl1
                Atl1.PcuSubsystem {}
            }
            Component {
                id: tidComponent_smog1
                Smog1.TidSubsystem {}
            }
            Component {
                id: tidComponent_smogp
                SmogP.TidSubsystem {}
            }
            Component {
                id: battComponent_atl1
                Atl1.BatterySubsystem {}
            }
            Component {
                id: solarComponent_smog1
                Smog1.SolarSubsystem {}
            }
            Component {
                id: solarComponent_smogp
                SmogP.SolarSubsystem {}
            }
            Component {
                id: solarComponent_atl1
                Atl1.SolarSubsystem {}
            }
        }

        Image {
            id: logo
            fillMode: Image.PreserveAspectFit
            verticalAlignment: Image.AlignTop
            horizontalAlignment: Image.AlignRight
            width: mainWindow.width < 900 ? 100 : 200
            visible: mainWindow.currentSatellite < 2
            source: ["qrc:/images/smog_1.png", "qrc:/images/smog_p.png", "qrc:/images/smog_1.png"][mainWindow.currentSatellite]
            y: -titleLabel.height - statusPage.spacing
            z: 1
        }
    }
}
