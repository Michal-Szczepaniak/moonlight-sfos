import QtQuick 2.0
import Sailfish.Silica 1.0

Dialog {

    SilicaFlickable {
        anchors.fill: parent
        flickableDirection: Flickable.VerticalFlick
        contentHeight: column.height

        Column {
            id: column
            width: parent.width

            DialogHeader {
                title: qsTr("Pair")
            }
        }
    }
}
