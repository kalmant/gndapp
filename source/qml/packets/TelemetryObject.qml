
import QtQuick 2.9
import QtQuick.Controls 2.2

QtObject {
    property string group
    property string description
    property string content: String(value) + (unit ? (" " + unit) : "")
    property var value
    property var min
    property var max
    property string unit
    property bool ok: (typeof(min) === "undefined" || value >= min) && (typeof(max) === "undefined" || value <= max)
}
