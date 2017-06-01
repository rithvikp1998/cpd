import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.4

Rectangle{
    width: parent.width
    height: parent.height

    /* Options taken from GNOME 2 print dialog advanced tab */

    Text {
        x: 20
        y: 15
        text: "Print Quality:"
        font.family: "Sans"
    }

    ComboBox {
        x: 160
        y: 10
        width: parent.width - 180
        height: 24
        font.family: "Sans"
        font.pixelSize: 12
        model: ["Normal", "Low", "High"]
    }

    Text {
        x: 20
        y: 55
        text: "Color Correction:"
        font.family: "Sans"
    }

    ComboBox {
        x: 160
        y: 50
        width: parent.width - 180
        height: 24
        font.family: "Sans"
        font.pixelSize: 12
        model: ["None", "Red", "Blue"] //Change this to real options for color corrections
    }

    Text {
        x: 20
        y: 95
        text: "Brightness: "
        font.family: "Sans"
    }

    TextField {
        x: 160
        y: 90
        width: parent.width - 180
        height: 24
        font.family: "Sans"
        font.pixelSize: 12
    }

    Text {
        x: 20
        y: 135
        text: "Contrast: "
        font.family: "Sans"
    }

    TextField {
        x: 160
        y: 130
        width: parent.width - 180
        height: 24
        font.family: "Sans"
        font.pixelSize: 12
    }

}
