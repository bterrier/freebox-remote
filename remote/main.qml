import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

ApplicationWindow {
    visible: true
    width: 640
    height: 480
    title: qsTr("Hello World")
    ColumnLayout {
        Label {
            text: "State: " + fh.stateString(fh.state);
        }

        Button {
            text: "POWER"
            onClicked: fh.send();
        }
    }
}
