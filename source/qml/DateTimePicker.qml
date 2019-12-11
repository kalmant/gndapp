
import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Controls 1.4 as Controls1

Column {
    spacing: 10
    bottomPadding: 10

    function getDateTime() {
        var sel = calendar.selectedDate
        var ret = new Date(Date.UTC(sel.getFullYear(), sel.getMonth(), sel.getDate(),
                           hourSpin.value, minuteSpin.value, secondSpin.value, 0))
        // The values currently selected are converted to UTC, since those values are UTC-based
        return ret.valueOf()
        // returns time utc timestamp (including milliseconds: 000, at the very end)
    }
    function reset() {
        var nd = new Date()
        var modifiedDate = new Date(nd.getUTCFullYear(), nd.getUTCMonth(), nd.getUTCDate())
        calendar.minimumDate = modifiedDate
        calendar.selectedDate = modifiedDate
        calendar.maximumDate = new Date(2038,0,18)
        hourSpin.value = nd.getUTCHours()
        minuteSpin.value = nd.getUTCMinutes()
        secondSpin.value = nd.getUTCSeconds()
    }

    function setMaxDateDiffDays(days) {
        if ((days !== parseInt(days)) || days < 0) {
            console.error("Invalid days parameter in maximumDateDiffDays: "+days)
        } else {
            var min = calendar.minimumDate
            var max = new Date(Date.UTC(min.getUTCFullYear(), min.getUTCMonth(), min.getUTCDate() + days,
                                        min.getUTCHours(), min.getUTCMinutes(),
                                        min.getUTCSeconds(), 0))
            calendar.maximumDate = max
        }
    }

    Controls1.Calendar {
        id: calendar
    }

    Grid {
        width: calendar.width
        columns: 2
        spacing: 10
        verticalItemAlignment: Grid.AlignVCenter

        Label { text: qsTr("Hour") }
        SpinBox {
            id: hourSpin
            value: 0
            from: 0
            to: 23
        }
        Label { text: qsTr("Minute") }
        SpinBox {
            id: minuteSpin
            value: 0
            from: 0
            to: 59
        }
        Label { text: qsTr("Second") }
        SpinBox{
            id: secondSpin
            value: 0
            from: 0
            to: 59
        }
    }
}
