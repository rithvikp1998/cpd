/****************************************************************************
**
**  $QT_BEGIN_LICENSE:GPL$
**
**  This program is free software: you can redistribute it and/or modify
**  it under the terms of the GNU General Public License as published by
**  the Free Software Foundation, either version 3 of the License, or
**  (at your option) any later version.
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with this program.  If not, see <http://www.gnu.org/licenses/>
**
**  $QT_END_LICENSE$
**
****************************************************************************/

import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.0

Rectangle {
    anchors.fill: parent

    signal resolutionValueChanged(string resolutionValue, string printerName)

    property string printer_name: "Xerox_Placeholder"

    ScrollBar {
        id: scroll_bar
        hoverEnabled: true
        active: hovered || pressed
        orientation: Qt.Vertical
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.bottom: parent.bottom
    }

    /* Covers the entire main view except scrollbar's width. Subtracting scroll_bar.width retains
       the scrollbar's drag function */
    MouseArea {
        height: parent.height
        width: parent.width - scroll_bar.width
        onWheel: {
            scroll_bar.position -= (wheel.angleDelta.y / 500)
            if (scroll_bar.position < 0)
                scroll_bar.position = 0
            if (scroll_bar.position > 1)
                scroll_bar.position = 1
        }
    }

    /* Acts as the main view. Only Resolution combobox works with the backend, rest are static due
       to limitations in the CUPS API. */
    Rectangle {
        x: 20
        y: 10
        width: parent.width - 40
        height: parent.height - 20

        GridLayout {
            id: grid_layout
            y: -scroll_bar.position * (height - parent.height)
            width: parent.width
            rowSpacing: 0
            columns: 2

            Label {
                text: "Quality"
                Layout.columnSpan: 2
                Layout.preferredHeight: 30
                font.bold: true
                font.pointSize: 12
            }

            Label {
                text: "Resolution (dpi)"
                Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
            }

            ComboBox {
                Layout.preferredWidth: parent.width - 180
                Layout.preferredHeight: 35
                Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                model: supportedResolutions
                onCurrentIndexChanged: resolutionValueChanged(supportedResolutions[currentIndex],
                                                              printer_name)
            }

            Label {
                text: "Toner Darkness"
                Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
            }

            ComboBox {
                Layout.preferredWidth: parent.width - 180
                Layout.preferredHeight: 40
                Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                model: ["Printer Setting", "1", "2", "3"]
            }

            Label {
                text: "Brightness"
                Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
            }

            ComboBox {
                Layout.preferredWidth: parent.width - 180
                Layout.preferredHeight: 40
                Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                model: ["Printer Setting", "-3", "-2", "-1", "0", "1", "2", "3"]
            }

            Label {
                text: "Contrast"
                Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
            }

            ComboBox {
                Layout.preferredWidth: parent.width - 180
                Layout.preferredHeight: 40
                Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                model: ["Printer Setting", "0", "1", "2", "3"]
            }

            Label {
                text: "Saturation"
                Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
            }

            ComboBox {
                Layout.preferredWidth: parent.width - 180
                Layout.preferredHeight: 40
                Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                model: ["Printer Setting", "-3", "-2", "-1", "0", "1", "2", "3"]
            }

            Label {
                text: "Enhance Fine Lines"
                Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
            }

            ComboBox {
                Layout.preferredWidth: parent.width - 180
                Layout.preferredHeight: 40
                Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                model: ["Printer Setting", "Off", "On"]
            }

            Label {
                text: "Color"
                Layout.columnSpan: 2
                Layout.preferredHeight: 30
                font.bold: true
                font.pointSize: 12
            }

            Label {
                text: "Color Correction"
                Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
            }

            ComboBox {
                Layout.preferredWidth: parent.width - 180
                Layout.preferredHeight: 40
                Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                model: ["Printer Setting", "Off", "Auto", "Manual"]
            }

            Label {
                text: "Color Saver"
                Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
            }

            ComboBox {
                Layout.preferredWidth: parent.width - 180
                Layout.preferredHeight: 40
                Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                model: ["Printer Setting", "Off", "On"]
            }

            Label {
                text: "Monochrome"
                Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
            }

            ComboBox {
                Layout.preferredWidth: parent.width - 180
                Layout.preferredHeight: 40
                Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                model: ["Printer Setting", "Off", "On"]
            }

            Label {
                text: "Color Balance"
                Layout.columnSpan: 2
                Layout.preferredHeight: 30
                font.bold: true
                font.pointSize: 12
            }

            Label {
                text: "Cyan Balance"
                Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
            }

            ComboBox {
                Layout.preferredWidth: parent.width - 180
                Layout.preferredHeight: 40
                Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                model: ["Printer Setting", "-3", "-2", "-1", "0", "1", "2", "3"]
            }

            Label {
                text: "Magenta Balance"
                Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
            }

            ComboBox {
                Layout.preferredWidth: parent.width - 180
                Layout.preferredHeight: 40
                Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                model: ["Printer Setting", "-3", "-2", "-1", "0", "1", "2", "3"]
            }

            Label {
                text: "Yellow Balance"
                Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
            }

            ComboBox {
                Layout.preferredWidth: parent.width - 180
                Layout.preferredHeight: 40
                Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                model: ["Printer Setting", "-3", "-2", "-1", "0", "1", "2", "3"]
            }

            Label {
                text: "Black Balance"
                Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
            }

            ComboBox {
                Layout.preferredWidth: parent.width - 180
                Layout.preferredHeight: 40
                Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                model: ["Printer Setting", "-3", "-2", "-1", "0", "1", "2", "3"]
            }

            Label {
                text: "Manual Color"
                Layout.columnSpan: 2
                Layout.preferredHeight: 30
                font.bold: true
                font.pointSize: 12
            }

            Label {
                text: "RGB Image"
                Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
            }

            ComboBox {
                Layout.preferredWidth: parent.width - 180
                Layout.preferredHeight: 40
                Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                model: ["Printer Setting", "Off", "Vivid", "sRGB Display", "sRGB Vivid", "Display-true Black"]
            }

            Label {
                text: "RGB Text"
                Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
            }

            ComboBox {
                Layout.preferredWidth: parent.width - 180
                Layout.preferredHeight: 40
                Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                model: ["Printer Setting", "Off", "Vivid", "sRGB Display", "sRGB Vivid", "Display-true Black"]
            }

            Label {
                text: "RGB Graphics"
                Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
            }

            ComboBox {
                Layout.preferredWidth: parent.width - 180
                Layout.preferredHeight: 40
                Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                model: ["Printer Setting", "Off", "Vivid", "sRGB Display", "sRGB Vivid", "Display-true Black"]
            }

            Label {
                text: "CMYK"
                Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
            }

            ComboBox {
                Layout.preferredWidth: parent.width - 180
                Layout.preferredHeight: 40
                Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                model: ["Printer Setting", "Off", "US CMYK", "Euro CMYK"]
            }

            Label {
                text: "Finishing"
                Layout.columnSpan: 2
                Layout.preferredHeight: 30
                font.bold: true
                font.pointSize: 12
            }

            Label {
                text: "Print Blank Pages"
                Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
            }

            ComboBox {
                Layout.preferredWidth: parent.width - 180
                Layout.preferredHeight: 40
                Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                model: ["Printer Setting", "Off", "On"]
            }

            Label {
                text: "Separator Pages"
                Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
            }

            ComboBox {
                Layout.preferredWidth: parent.width - 180
                Layout.preferredHeight: 40
                Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                model: ["Printer Setting", "None", "Between Jobs", "Between Copies", "Between Pages"]
            }

            Label {
                text: "Offset Pages"
                Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
            }

            ComboBox {
                Layout.preferredWidth: parent.width - 180
                Layout.preferredHeight: 40
                Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                model: ["Printer Setting", "None", "Between Jobs", "Between Copies"]
            }

            Label {
                text: "Staple Job"
                Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
            }

            ComboBox {
                Layout.preferredWidth: parent.width - 180
                Layout.preferredHeight: 40
                Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                model: ["Printer Setting", "Off", "On"]
            }

            Label {
                text: "Punch Hole"
                Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
            }

            ComboBox {
                Layout.preferredWidth: parent.width - 180
                Layout.preferredHeight: 40
                Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                model: ["Printer Setting", "Off", "On"]
            }

            Label {
                text: "Fold"
                Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
            }

            ComboBox {
                Layout.preferredWidth: parent.width - 180
                Layout.preferredHeight: 40
                Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                model: ["Printer Setting", "Off", "On"]
            }
        }
    }
}
