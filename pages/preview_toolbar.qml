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

    signal nextPageButtonClicked()              // Connects to showNextPage in QCpdPreviewWidget
    signal prevPageButtonClicked()              // Connects to showPrevPage in QCpdPreviewWidget
    signal zoomSliderValueChanged(real value)   // Connects to setZoom in QCpdPreviewWidget

    /* Show the previous page in the document */
    Button {
        id: preview_previous_page_button
        x: 20
        anchors.verticalCenter: parent.verticalCenter
        height: 40
        width: 40
        text: "\u25C0" // Denotes "<" symbol

        onClicked: prevPageButtonClicked()
    }

    /* Set the zoom level of the preview image */
    Slider {
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

        onValueChanged: zoomSliderValueChanged(preview_zoom_slider.value)
    }

    /* Show the next page in the document */
    Button {
        id: preview_next_page_button
        x: parent.width - 60
        anchors.verticalCenter: parent.verticalCenter
        height: 40
        width: 40
        text: "\u25B6" // Denotes ">" symbol

        onClicked: nextPageButtonClicked()
    }
}
