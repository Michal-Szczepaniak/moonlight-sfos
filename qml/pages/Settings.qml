import QtQuick 2.0
import Sailfish.Silica 1.0

Page {
    allowedOrientations: Orientation.All

    SilicaFlickable {
        anchors.fill: parent
        contentHeight: column.height

        Column {
            id: column
            width: parent.width
            spacing: Theme.paddingLarge

            PageHeader {
                title: qsTr("Settings")
            }

            Button {
                anchors.horizontalCenter: parent.horizontalCenter
                text: qsTr("Wipe all data and exit")
                onClicked: remorse.execute(qsTr("Wiping all data"), function() { pairing.wipe() })
            }
        }
    }

    RemorsePopup { id: remorse }
}
