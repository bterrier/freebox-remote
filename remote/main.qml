import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

import dev.bterrier.remote 1.0

ApplicationWindow {
    visible: true
    width: layout.implicitWidth
    height: layout.implicitHeight + header.height
    title: qsTr("Hello World")

    header: ToolBar {

        Label {
            anchors.centerIn: parent
            text: "State: " + fh.stateString(fh.state);
        }
    }
    ColumnLayout {
        id: layout

        GridLayout {
            columns: 3


            Button {
                text: 'AV'
            }
            Button {
                Layout.column: 2
                icon.source: "qrc:/icons/power_settings_new-24px.svg"
                onClicked: fh.sendKeyboard(FoilHid.HID_KEYBOARD_POWER);
            }
            Repeater {
                model: 9


                Button {
                    text: index + 1
                }
            }

            Button {
                Layout.row: 4
                Layout.column: 1
                text: '0'
            }
            Button {
                Layout.row: 5
                icon.source: "qrc:/icons/undo-24px.svg"
            }

            Button {
                icon.source: "qrc:/icons/arrow_drop_up-24px.svg"
                onClicked: fh.sendKeyboard(FoilHid.HID_KEYBOARD_UPARROW);
            }


            Button {
                icon.source: "qrc:/icons/search-24px.svg"
            }


            Button {
                icon.source: "qrc:/icons/arrow_left-24px.svg"
                onClicked: fh.sendKeyboard(FoilHid.HID_KEYBOARD_LEFTARROW);
            }


            Button {
                text: 'OK'
                onClicked: fh.sendKeyboard(FoilHid.HID_KEYBOARD_ENTER);
            }


            Button {
                icon.source: "qrc:/icons/arrow_right-24px.svg"
                onClicked: fh.sendKeyboard(FoilHid.HID_KEYBOARD_RIGHTARROW);
            }

            Button {
                icon.source: "qrc:/icons/menu_open-24px.svg"
            }

            Button {
                icon.source: "qrc:/icons/arrow_drop_down-24px.svg"
                onClicked: fh.sendKeyboard(FoilHid.HID_KEYBOARD_DOWNARROW);
            }

            Button {
                icon.source: "qrc:/icons/info-24px.svg"
            }

            Button {
                Layout.columnSpan: 3
                Layout.fillWidth: true
                text: 'free'
                onClicked: fh.sendKeyboard(FoilHid.HID_KEYBOARD_HOME);
            }

            Button {
                icon.source: "qrc:/icons/volume_up-24px.svg"
            }

            Button {
                icon.source: "qrc:/icons/volume_off-24px.svg"
                onClicked: fh.sendConsumer(FoilHid.HID_CONSUMER_MUTE);
            }

            Button {
                icon.source: "qrc:/icons/add-24px.svg"
            }

            Button {
                icon.source: "qrc:/icons/volume_down-24px.svg"
            }

            Button {
                icon.source: "qrc:/icons/fiber_manual_record-24px.svg"
                onClicked: fh.sendConsumer(FoilHid.HID_CONSUMER_RECORD);
            }

            Button {
                icon.source: "qrc:/icons/remove-24px.svg"
            }
            Button {
                icon.source: "qrc:/icons/fast_rewind-24px.svg"
                onClicked: fh.sendConsumer(FoilHid.HID_CONSUMER_REWIND);
            }

            Button {
                icon.source: "qrc:/icons/play_arrow-24px.svg"
            }

            Button {
                icon.source: "qrc:/icons/fast_forward-24px.svg"
                onClicked: fh.sendConsumer(FoilHid.HID_CONSUMER_FAST_FORWARD);
            }
        }
    }
}
