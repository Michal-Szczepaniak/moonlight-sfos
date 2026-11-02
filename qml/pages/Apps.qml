import QtQuick 2.0
import Sailfish.Silica 1.0

Page {
    id: page

    property string host

    allowedOrientations: Orientation.All

    SilicaListView {
        anchors.fill: parent
        model: pairing.apps

        header: PageHeader {
            title: host
        }

        PullDownMenu {
            MenuItem {
                text: qsTr("Refresh")
                enabled: !pairing.appsBusy
                onClicked: pairing.loadApps(host)
            }
        }

        delegate: Item {
            width: page.width
            height: Theme.itemSizeSmall

            Label {
                text: modelData
                anchors.verticalCenter: parent.verticalCenter
                x: Theme.horizontalPageMargin
                width: parent.width - 2 * Theme.horizontalPageMargin
                color: Theme.primaryColor
                truncationMode: TruncationMode.Fade
            }
        }

        ViewPlaceholder {
            enabled: pairing.apps.length === 0 && !pairing.appsBusy
            text: pairing.appsStatus.length > 0 ? qsTr("Could not load programs") : qsTr("No programs")
            hintText: pairing.appsStatus
        }
    }

    BusyIndicator {
        anchors.centerIn: parent
        running: pairing.appsBusy
        size: BusyIndicatorSize.Large
    }
}
