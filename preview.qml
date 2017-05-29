import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.4

Rectangle{

    width: parent.width
    height: parent.height

    Image {
        id: preview
        x: 20
        y: 20
        width: parent.width - 40
        height: parent.height - 40
        source: "image://preview/pdf"
    }
}
