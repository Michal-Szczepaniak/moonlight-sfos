import QtQuick 2.0
import Sailfish.Silica 1.0

Page {
    id: page

    allowedOrientations: Orientation.All

    SilicaListView {
        anchors.fill: parent
        model: pairing.hosts

        header: PageHeader {
            title: qsTr("Computers")
        }

        PullDownMenu {
            MenuItem {
                text: qsTr("Pair")
                onClicked: pageStack.push(Qt.resolvedUrl("../dialogs/PairDialog.qml"))
            }

            MenuItem {
                text: qsTr("Settings")
                onClicked: pageStack.push(Qt.resolvedUrl("Settings.qml"))
            }

            MenuItem {
                text: qsTr("About")
                onClicked: pageStack.push(Qt.resolvedUrl("About.qml"))
            }
        }

        delegate: ListItem {
            contentHeight: Theme.itemSizeSmall
            width: ListView.view.width

            onClicked: {
                pairing.loadApps(modelData)
                pageStack.push(Qt.resolvedUrl("Apps.qml"), { "host": modelData })
            }

            Label {
                text: modelData
                anchors.verticalCenter: parent.verticalCenter
                x: Theme.horizontalPageMargin
                width: parent.width - 2 * Theme.horizontalPageMargin
                color: highlighted ? Theme.highlightColor : Theme.primaryColor
            }
        }

        ViewPlaceholder {
            enabled: pairing.hosts.length === 0
            text: qsTr("No computers")
        }
    }
}
