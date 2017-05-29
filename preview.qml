import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.4

/* I have to make this responsive */

Rectangle{

    width: 640-150
    height: 480

    Text {
        id: text_area
        text: qsTr("Preview Tab")
    }

    Image {
        x: 20
        y: 40
        id: preview
        width: 640-150-40
        height: 280
        source: "image://preview/pdf"
    }
}
