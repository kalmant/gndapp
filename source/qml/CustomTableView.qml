
// Copyright (c) 2019 Timur Kristóf
// Licensed to you under the terms of the MIT license.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

import QtQuick 2.9
import QtQuick.Controls 2.2

Frame {
    id: frame

    property var headerModel
    property alias model: listView.model
    property alias flickableDirection: listView.flickableDirection
    property alias boundsBehavior: listView.boundsBehavior
    property alias currentIndex: listView.currentIndex
    property alias verticalLayoutDirection: listView.verticalLayoutDirection
    property alias keyNavigationEnabled: listView.keyNavigationEnabled
    readonly property int headerHeight: fontMetrics.height + 2 * cellPadding + borderWidth

    property int cellPadding: 3
    property int minCellWidth: 50
    property int borderWidth: 1

    signal doubleClicked

    padding: 0

    FontMetrics {
        id: fontMetrics
    }

    ScrollView {
        id: scrollView
        width: frame.width
        height: frame.height
        padding: borderWidth

        ListView {
            id: listView
            width: scrollView.width - 2 * scrollView.padding
            height: scrollView.height - 2 * scrollView.padding
            contentWidth: headerItem ? headerItem.width : width
            flickableDirection: Flickable.AutoFlickIfNeeded
            boundsBehavior: Flickable.StopAtBounds
            clip: true
            currentIndex: -1
            focus: true

            headerPositioning: ListView.OverlayHeader
            header: Rectangle {
                id: headerRect
                color: palette.button
                width: Math.max(scrollView.width - 2 * scrollView.padding, headerRow.width)
                height: headerHeight
                z: 10

                Rectangle {
                    height: borderWidth
                    width: parent.width
                    y: parent.height - height
                    color: palette.mid
                }

                MouseArea {
                    property int dragStartMargin: 10
                    property int dragStartX
                    property int dragStartWidth
                    property var model

                    anchors.fill: parent
                    preventStealing: true

                    onPressed: {
                        var found = false
                        var total = 0

                        for (var i = 0; i < headerModel.count; i++) {
                            var m = headerModel.get(i)
                            total += m.width

                            if (mouse.x <= (total + dragStartMargin) && mouse.x >= (total - dragStartMargin)) {
                                found = true
                                model = m
                                break
                            }
                        }

                        if (!found) {
                            model = null
                            mouse.accepted = false
                            return
                        }

                        dragStartX = mouse.x
                        dragStartWidth = model.width
                    }
                    onPositionChanged: {
                        if (pressed) {
                            var diff = mouse.x - dragStartX
                            model.width = Math.max(dragStartWidth + diff, minCellWidth)
                        }
                    }
                }

                Row {
                    id: headerRow

                    Repeater {
                        model: headerModel

                        Rectangle {
                            width: model.width
                            height: fontMetrics.height + 2 * cellPadding
                            color: palette.button

                            Label {
                                id: headerCellText
                                text: model.title
                                width: parent.width - cellPadding * 2
                                x: cellPadding + parent.border.width
                                y: cellPadding
                                clip: true
                            }

                            Rectangle {
                                width: borderWidth
                                height: parent.height
                                color: palette.mid
                                visible: model.index
                            }

                            MouseArea {
                                width: 20
                                x: parent.width - width / 2
                                height: parent.height
                                cursorShape: Qt.SizeHorCursor
                                onPressed: mouse.accepted = false
                            }
                        }
                    }
                }
            }
            delegate: Rectangle {
                id: delegateRoot
                width: listView.contentWidth
                height: fontMetrics.height + 2 * cellPadding
                color: ListView.isCurrentItem ? palette.highlight : (model.index % 2) ? palette.base : palette.alternateBase
                z: 1

                property var rowModel: model

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        listView.forceActiveFocus()
                        listView.currentIndex = model.index
                    }
                    onDoubleClicked: frame.doubleClicked()
                }

                Row {
                    Repeater {
                        model: headerModel

                        Item {
                            width: model.width
                            height: fontMetrics.height + 2 * cellPadding
                            clip: true

                            Label {
                                x: cellPadding
                                y: cellPadding
                                text: delegateRoot.rowModel[model.role]
                                color: ListView.isCurrentItem ? palette.hightlightedText : palette.windowText
                            }
                        }
                    }
                }
            }
        }
    }
}
