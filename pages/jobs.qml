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
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.0

Rectangle {

    property bool active_only: false
    
    signal cancelJob(int jobIndex, bool activeOnly)

    /* Acts as the header row to indicate the column values */
    Rectangle {
        width: parent.width
        height: location_heading.contentHeight // Since "Location" is the biggest word in this row,
                                               // it will be the first to get wrapped
        Text {
            x: 20
            y: 10
            width: parent.width / 3 - 20
            text: "Printer"
            font.bold: true
            wrapMode: Text.Wrap
        }

        Text {
            id: location_heading
            x: parent.width / 3 + 20
            y: 10
            width: parent.width / 3 - 20
            text: "User"
            font.bold: true
            wrapMode: Text.Wrap
        }

        Text {
            x: 2 * parent.width / 3 + 20
            y: 10
            width: parent.width / 3 - 20
            text: "Status"
            font.bold: true
            wrapMode: Text.Wrap
        }
    }

    /* Fills the rows in the table */
    ListView {
        id: jobs_view
        width: parent.width
        height: parent.height - 200
        model: jobsList
        y: location_heading.contentHeight + 20

        delegate: Rectangle {
            width: parent.width
            height: Math.max(printer_text.contentHeight, location_text.contentHeight,
                             status_text.contentHeight) + 10
            color: (model.index % 2 == 0) ? "#EEEEEE" : "white"
            property variant stringList: jobsList[index].split('%')

            Menu {
                id: menu

                MenuItem {
                    text: "Cancel"
                    onTriggered: cancelJob(index, active_only)
                }
            }

            MouseArea {
                anchors.fill: parent
                acceptedButtons: Qt.RightButton
                hoverEnabled: true

                onEntered: { parent.color = "#BDBDBD" }
                onExited:  { parent.color = (model.index % 2 == 0) ? "#EEEEEE" : "white" }
                onClicked: {
                    menu.x = mouseX
                    menu.y = mouseY
                    menu.open()
                }
            }

            Text {
                id: printer_text
                x: 20
                anchors.verticalCenter: parent.verticalCenter
                width: parent.width / 3 - 20
                text: stringList[0]
                wrapMode: Text.Wrap
            }

            Text {
                id: location_text
                x: parent.width / 3 + 20
                anchors.verticalCenter: parent.verticalCenter
                width: parent.width / 3 - 20
                text: stringList[1]
                wrapMode: Text.Wrap
            }

            Text {
                id: status_text
                x: 2*parent.width / 3 + 20
                anchors.verticalCenter: parent.verticalCenter
                width: parent.width / 3 - 20
                text: stringList[2]
                wrapMode: Text.Wrap
            }
        }
    }

    /* Select the delay between clicking on the print button and starting the job on the printer */
    Text {
        x: 20
        y: parent.height - 147
        text: "Start Job : "
    }

    ComboBox {
        id: start_job_combobox
        x: 120
        y: parent.height - 155
        width: (start_job_combobox.currentIndex == 1) ? parent.width - 280 : parent.width - 180
        height: 40
        font.pixelSize: 12
        model: jobHoldOptionsList
    }

    /* Save the job details in a file inorder to review or reuse them later */
    Text {
        x: 20
        y: parent.height - 97
        text: "Save Job : "
    }

    Switch {
        id: save_job_switch
        x: 105
        y: parent.height - 110
    }

    Button {
        x: 200
        y: parent.height - 110
        height: 40
        text: "Browse"
        visible: (save_job_switch.checked) ? true : false
        onClicked: { file_dialog.open() }
    }

    Text {
        text: "Location : "
        x: 20
        y: parent.height - 50
        visible: (save_job_switch.checked) ? true : false
    }

    Text {
        id: save_job_location
        text: "None"
        x: 120
        y: parent.height - 50
        visible: (save_job_switch.checked) ? true : false
    }

    FileDialog {
        id: file_dialog
        title: "Please choose a folder"
        folder: shortcuts.home
        selectFolder: true
        onAccepted: {
            save_job_location.text = file_dialog.fileUrl.toString().substring(7)
            file_dialog.close()
        }
        onRejected: {
            file_dialog.close()
        }
    }
}
