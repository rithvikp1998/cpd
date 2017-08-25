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
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.4

Rectangle {
    visible: true

    signal printButtonClicked(string printerName, string backendName) // Connects to printDocument(QString, QString) in QQmlWidget
    signal cancelButtonClicked()                    // Connects to cpdQuit() in QQmlWidget
    signal setJobsList(bool activeOnly)             // Connects to setJobsList(bool) in QQmlWidget
    signal setJobsHoldOptions(string printerName)   // Connects to setJobsHoldOptions(QString) in QQmlWidget
    signal setAdvancedOptions(string printerName)   // Connects to setAdvancedOptions(QString) in QQmlWidget
    signal resolutionValueChanged(string resolutionValue, string printerName)   // Connects to setResolutionSetting(QString, QString) in QQmlWidget
    signal cancelJob(int jobIndex, bool activeOnly) // Connects to cancelJob(int, bool) in QQmlWidget

    /* Change the following properties to match the test printer you are using */
    property string printer_name: "Xerox_Placeholder"
    property string backend_name: "CUPS"
    property bool active_only: false

    /* This function calls the appropriate signal corresponding the item loaded in page_loader */
    function emitSignal(pageRequested) {
        pageRequested = pageRequested.toString()
        if(pageRequested === "Jobs") {
            setJobsList(active_only);
            setJobsHoldOptions(printer_name);
        }
        if(pageRequested === "Advanced")
            setAdvancedOptions(printer_name);
        return;
    }

    /* This Rectangle is for the sidebar. value points to the page it loads when clicked */

    Rectangle {
        height: parent.height
        width: 150
        color: "#EEEEEE"

        ListModel {
            id: sidebar_model

            ListElement {
                name: "General"
                value: "general.qml"
            }
            ListElement {
                name: "Page Setup"
                value: "page_setup.qml"
            }
            ListElement {
                name: "Options"
                value: "options.qml"
            }
            ListElement {
                name: "Jobs"
                value: "jobs.qml"
            }
            ListElement {
                name: "Advanced"
                value: "advanced.qml"
            }
        }

        ListView {
            width: parent.width
            height: parent.height
            model: sidebar_model
            delegate: Rectangle { // Every button in the sidebar is made using a Rectangle with a MouseArea
                width: parent.width
                height: 40
                color: "#EEEEEE"

                Text {
                    anchors.verticalCenter: parent.verticalCenter
                    text: name
                    x: 10
                }

                MouseArea {
                    anchors.fill: parent
                    hoverEnabled: true
                    onClicked: {
                        emitSignal(name)
                        page_loader.source = value
                    }
                    onEntered: { parent.color = "#BDBDBD" }
                    onExited:  { parent.color = "#EEEEEE" }
                }
            }
        }

        /* Print button in the sidebar */
        Rectangle {
            x: 10
            y: parent.height - 120
            width: parent.width - 20
            height: 40
            color: "#E91E63"

            Text {
                anchors.centerIn: parent
                text: "Print"
                color: "white"
            }

            MouseArea {
                anchors.fill: parent
                hoverEnabled: true
                onEntered: { parent.color = "#D81B60" }
                onExited:  { parent.color = "#E91E63" }
                onClicked: printButtonClicked(printer_name, backend_name)
            }
        }

        /* Cancel button in the sidebar */
        Rectangle {
            x: 10
            y: parent.height - 60
            width: parent.width - 20
            height: 40
            color: "#CFD8DC"

            Text {
                anchors.centerIn: parent
                text: "Cancel"
                color: "black"
            }

            MouseArea {
                anchors.fill: parent
                hoverEnabled: true
                onEntered: { parent.color = "#B0BEC5" }
                onExited:  { parent.color = "#CFD8DC" }
                onClicked: { cancelButtonClicked() }
            }
        }
    }
    /* Sidebar ends here */

    /* This Rectangle is for the main view */

    Rectangle {
        id:main_view
        x: 150; width: parent.width - 150
        height: parent.height

        Loader {
            id: page_loader
            source: "general.qml"
            width: parent.width
            height: parent.height
        }

        /* Acts as a slot for the signals coming from jobs.qml and advanced.qml and forwards them
           to cpd.cpp */
        Connections {
            target: page_loader.item
            onResolutionValueChanged: resolutionValueChanged(resolutionValue, printerName)
            onCancelJob: cancelJob(jobIndex, 0)
        }
    }

    /* Main view ends here */
}
