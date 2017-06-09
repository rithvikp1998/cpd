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
        text: "Print Quality :"
        //font.family: "Sans"
    }

    ComboBox {
        x: 160
        y: 10
        width: parent.width - 180
        height: 30
        //font.family: "Sans"
        font.pixelSize: 12
        model: ["Normal", "Low", "High"]
    }

    Text {
        x: 20
        y: 55
        text: "Color Correction :"
        //font.family: "Sans"
    }

    ComboBox {
        x: 160
        y: 50
        width: parent.width - 180
        height: 30
        //font.family: "Sans"
        font.pixelSize: 12
        model: ["Off", "Auto", "Display", "CMYK", "Black and White", "2-Color Draft", "Vivid"]
    }

    Text {
        x: 20
        y: 95
        text: "Brightness : "
        //font.family: "Sans"
    }

    TextField {
        x: 160
        y: 95
        width: parent.width - 180
        height: 24
        //font.family: "Sans"
        font.pixelSize: 12
    }

    Text {
        x: 20
        y: 135
        text: "Contrast : "
        //font.family: "Sans"
    }

    TextField {
        x: 160
        y: 135
        width: parent.width - 180
        height: 24
        //font.family: "Sans"
        font.pixelSize: 12
    }

    /* Options taken from Image Viewer's print dialog on GNOME 3 */
    /* Handles the position of image with respect to the paper */

    Text {
        text: "Image Position"
        font.bold: true
        x: 20
        y: 180
        //font.family: "Sans"
        font.pixelSize: 14
    }

    Text {
        x: 20
        y: 212
        text: "Left : "
        //font.family: "Sans"
    }

    TextField {
        x: 70
        y: 215
        width: parent.width/3
        height: 24
        //font.family: "Sans"
        font.pixelSize: 12
    }

    Text {
        x: parent.width/2
        y: 212
        text: "Top : "
        //font.family: "Sans"
    }

    TextField {
        x: parent.width/2 + 60
        y: 215
        width: parent.width/3
        height: 24
        //font.family: "Sans"
        font.pixelSize: 12
    }

    Text {
        x: 20
        y: 252
        text: "Center :"
        //font.family: "Sans"
    }

    ComboBox {
        x: 90
        y: 250
        width: parent.width - 110
        height: 30
        //font.family: "Sans"
        font.pixelSize: 12
        model: ["None", "Horizontal", "Vertical", "Both"]
    }

    /* Handles the size of image */

    Text {
        text: "Image Size"
        font.bold: true
        x: 20
        y: 300
        //font.family: "Sans"
        font.pixelSize: 14
    }

    Text {
        x: 20
        y: 332
        text: "Width : "
        //font.family: "Sans"
    }

    TextField {
        x: 80
        y: 335
        width: parent.width/3 - 10
        height: 24
        //font.family: "Sans"
        font.pixelSize: 12
    }

    Text {
        x: parent.width/2
        y: 332
        text: "Height : "
        //font.family: "Sans"
    }

    TextField {
        x: parent.width/2 + 70
        y: 335
        width: parent.width/3 - 10
        height: 24
        //font.family: "Sans"
        font.pixelSize: 12
    }

    Text {
        x: 20
        y: 372
        text: "Units :"
        //font.family: "Sans"
    }

    ComboBox {
        x: 80
        y: 370
        width: parent.width - 100
        height: 30
        //font.family: "Sans"
        font.pixelSize: 12
        model: ["Millimeter", "Centimeter", "Inches"]
    }

    Text {
        x: 20
        y: 410
        text: "Scale : "
        //font.family: "Sans"
    }

    Slider{
        id: image_scaling_slider
        x: 80
        y: 410
        width: parent.width - 160
        height: 20
    }

    Text {
        x: parent.width - 80
        y: 410
        text: "100%"
        //font.family: "Sans"
    }
}
