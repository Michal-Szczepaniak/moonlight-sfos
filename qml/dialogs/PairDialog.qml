import QtQuick 2.0
import Sailfish.Silica 1.0

Page {
    id: page

    allowedOrientations: Orientation.All

    SilicaFlickable {
        anchors.fill: parent
        contentHeight: column.height + Theme.paddingLarge

        Column {
            id: column
            width: parent.width
            spacing: Theme.paddingLarge

            PageHeader {
                title: qsTr("Pair")
            }

            TextField {
                id: hostField
                width: parent.width
                placeholderText: qsTr("192.168.1.69")
                label: qsTr("Computer")
                inputMethodHints: Qt.ImhNoAutoUppercase | Qt.ImhNoPredictiveText
                EnterKey.enabled: text.trim().length > 0 && !pairing.busy
                EnterKey.onClicked: pairing.pair(text.trim())
            }

            Button {
                anchors.horizontalCenter: parent.horizontalCenter
                text: qsTr("Pair")
                enabled: !pairing.busy && hostField.text.trim().length > 0
                onClicked: pairing.pair(hostField.text.trim())
            }

            BusyIndicator {
                anchors.horizontalCenter: parent.horizontalCenter
                running: pairing.busy
                size: BusyIndicatorSize.Medium
            }

            Label {
                width: parent.width
                horizontalAlignment: Text.AlignHCenter
                font.pixelSize: Theme.fontSizeHuge
                font.letterSpacing: Theme.paddingMedium
                color: Theme.highlightColor
                text: pairing.pin
                visible: pairing.pin.length > 0
            }

            Label {
                x: Theme.horizontalPageMargin
                width: parent.width - 2 * Theme.horizontalPageMargin
                wrapMode: Text.Wrap
                horizontalAlignment: Text.AlignHCenter
                color: Theme.highlightColor
                text: pairing.status
                visible: pairing.status.length > 0
            }
        }
    }
}
