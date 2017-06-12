/* TODO:

  1. Create a visual difference between page and container dimensions to reflect paper size

*/

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

        Flickable{
            id: flickable
            x: 20
            y: 10
            boundsBehavior: Flickable.StopAtBounds
            flickableDirection: Flickable.HorizontalAndVerticalFlick
            width: parent.width - 40
            height: parent.height - 20
            interactive: true
            clip: true

            Image {
                id: image
                anchors.centerIn: parent
                width: parent.width
                height: parent.height
                source: "image://preview/tmp/test.pdf/0"
            }
        }
    }

    Button { // Displays the previous page in the doument
        id: preview_previous_page_button
        x: 40
        y: parent.height - 50
        height: 40
        width: 40
        text: "\u25C0"

        onClicked: {
            var source = String(image.source)
            var fileNameLength = source.lastIndexOf("/")
            var fileName = source.substring(0, fileNameLength)
            var pageNumber = parseInt(source.substring(fileNameLength + 1, source.length))
            if (pageNumber - 1 >= 0)
                image.source = fileName + "/" + String(pageNumber - 1)
        }
    }

    Slider { // To set the zoom level of the preview image.
        id: preview_zoom_slider
        x: 100
        y: parent.height - 55
        width: parent.width - 200
        to: 3
        from: 1
        value: 1
        stepSize: 0.5
        property real previousScaleValue: 1

        onValueChanged: {
            flickable.resizeContent((image.width / previousScaleValue) * value , (image.height / previousScaleValue) * value, 0)
            previousScaleValue = value
        }
    }

    Button {  // Displays the next page in the doument
        id: preview_next_page_button
        x: parent.width - 80
        y: parent.height - 50
        height: 40
        width: 40
        text: "\u25B6"

        onClicked: {
            var source = String(image.source)
            var fileNameLength = source.lastIndexOf("/")
            var fileName = source.substring(0, fileNameLength)
            var pageNumber = parseInt(source.substring(fileNameLength + 1, source.length))
            if (pageNumber + 1 < preview_data.get_number_of_pages(source.substring(15,fileNameLength)))
                image.source = fileName + "/" + String(pageNumber + 1)
        }
    }
}
