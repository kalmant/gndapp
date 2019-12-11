import QtQuick 2.9
import QtQuick.Layouts 1.3

Text {
    textFormat: TextEdit.PlainText
    font.pixelSize: 12
    font.family: defaultMonotypeFontFamily


    Component.onCompleted: {
        Layout.minimumWidth = Qt.binding(function() {
            return (Math.floor(paintedWidth/5)+1)*5
        })
    }
}
