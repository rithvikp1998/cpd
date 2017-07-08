/* TODO
 * 1. Add functions to buttons
 */

import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.4

Rectangle {

    signal nextPageButtonClicked()
    signal prevPageButtonClicked()
    signal zoomSliderValueChanged()

    Button { // Displays the previous page in the doument
        id: preview_previous_page_button
        x: 20
        anchors.verticalCenter: parent.verticalCenter
        height: 40
        width: 40
        text: "\u25C0"

        onClicked: prevPageButtonClicked()
    }

    Slider { // To set the zoom level of the preview image.
        id: preview_zoom_slider
        x: 80
        anchors.verticalCenter: parent.verticalCenter
        width: parent.width - 160
        height: 40
        to: 3
        from: 1
        value: 1
        stepSize: 0.5
        property real previousScaleValue: 1

        onValueChanged: zoomSliderValueChanged()
    }

    Button {  // Displays the next page in the doument
        id: preview_next_page_button
        x: parent.width - 60
        anchors.verticalCenter: parent.verticalCenter
        height: 40
        width: 40
        text: "\u25B6"

        onClicked: nextPageButtonClicked()
    }
}
