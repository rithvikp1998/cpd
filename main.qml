import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.4

ApplicationWindow {
    visible: true
    width: 640
    height: 480
    title: qsTr("Print Dialog")

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
                name: "Image Quality"
                value: "image_quality.qml"
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
            delegate: Rectangle {
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

    Rectangle{
        id:main_view
        x: 150; width: parent.width
        height: parent.height

        Loader{
            id: page_loader
            source: "general.qml"
        }
    }
}