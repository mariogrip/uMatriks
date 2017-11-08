import QtQuick 2.4
import Ubuntu.Components 1.3
import Ubuntu.Components.Themes 1.3
import Qt.labs.settings 1.0

Page {
    id: roomView
    visible: false

    header: PageHeader {
        id: pageHeader

        title: i18n.tr("Room")
        trailingActionBar {
            numberOfSlots: 2
            visible: currentRoom.isCallSupported()
            actions: [
                Action {
                    id: actionCallVideo
                    iconName: "camcorder"
                    onTriggered: {
                        console.log("video call")
                        roomList.initCallPage(currentRoom);
                        roomList.placeVideoCall();
                    }
                },
                Action {
                    id: actionCallVoice
                    iconName: "call-start"
                    onTriggered: {
                        console.log("voice call")
                        roomList.initCallPage(currentRoom);
                        roomList.placeVoiceCall();
                    }
                }
            ]
        }
    }

    property var currentRoom
    property var completion

    function setRoom(room) {
        console.log("RoomView setting room: "+ room.displayName)
	      pageHeader.title = room.displayName
        currentRoom = room
        chat.setRoom(room)
    }

    function setConnection(conn) {
        chat.setConnection(conn)
    }

    function sendLine(line) {
        chat.sendLine(line)
        textEntry.text = ''
    }

    ChatRoom {
        id: chat
        anchors.bottom: textRect.top
        anchors.right: parent.right
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.margins: {
            bottom: 20
        }

        color: uMatriks.theme.palette.normal.background
    }

    Rectangle {
        id: textRect
        anchors.right: parent.right
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        color: uMatriks.theme.palette.normal.background

        TextField {
            id: textEntry
            focus: true
            anchors.right: parent.right
            anchors.left: parent.left
            anchors.bottom: parent.bottom
            anchors.margins: 10

            placeholderText: qsTr("Say something...")
            onAccepted: sendLine(text)

            Component.onCompleted: {
                textRect.height = height + (anchors.margins * 2);
            }
        }
    }
}
