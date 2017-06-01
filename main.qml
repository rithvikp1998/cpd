import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.4

Rectangle {
    visible: true
    width: 640
    height: 480

    /* This Rectangle is for the sidebar */

    Rectangle {
        height: parent.height
        width: 150
        color: "#EEEEEE"

        ListModel {
            id: sidebar_model

            ListElement {
                name: "General"
                value: "general.qml"    // value points to the page it loads when clicked
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
            ListElement {
                name: "Preview"
                value: "preview.qml"
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
                    font.family: "Sans"
                    x: 10
                }

                MouseArea{
                    anchors.fill: parent
                    hoverEnabled: true
                    onClicked: { page_loader.source = value }
                    onEntered: { parent.color = "#BDBDBD" }
                    onExited:  { parent.color = "#EEEEEE" }
                }
            }
        }
    }

    /* This Rectangle is for the main view */

    Rectangle{
        id:main_view
        x: 150; width: parent.width - 150
        height: parent.height

        Loader{
            id: page_loader
            source: "general.qml"
            width: parent.width
            height: parent.height
        }
    }
}
