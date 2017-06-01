import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.4

Rectangle{

    width: parent.width
    height: parent.height

    Rectangle{
        width: parent.width-80
        height: parent.height-80
        border.width: 1
        border.color: "black"
        x: 40
        y: 20

        Image {
            id: preview
            anchors.centerIn: parent
            width: parent.width - 10
            height: parent.height - 10
            source: "image://preview/pdf"
        }
    }

    Button { // Displays the previous page in the doument
        id: preview_previous_page_button
        x: 40
        y: parent.height - 40
        height: 20
        width: 20
        text: '<'
    }

    Slider{ // To set the zoom level of the preview image. I should add pan functionality.
        id: preview_zoom_slider
        x: 80
        y: parent.height - 40
        width: parent.width - 160
        height: 20
    }

    Button {  // Displays the next page in the doument
        id: preview_next_page_button
        x: parent.width - 60
        y: parent.height - 40
        height: 20
        width: 20
        text: '>'
    }
}
