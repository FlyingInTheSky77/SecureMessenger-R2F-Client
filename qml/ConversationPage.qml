import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12
import r2f.SqlConversationModel 1.0
import io.qt.Backend 1.0

Page
{
    id: root
    property string inConversationWith

    Connections
    {
            target: backend
            function onSomeMessageFromAuthorInQML_signal( author, timestamp, message )
            {
                if ( inConversationWith === author )
                {
                        modelclass.sendMessageAuthor( author, "Me", message );
                }
            }
            function onNeedNameinConversationWithNow_signal()
            {
                backend.setNameinConversationWithWriteNow( inConversationWith );
            }
            function onRecipientOffline_signal( msg )
            {
                pageTitle.text1 = inConversationWith + qsTr( " (is offline now)" );
                pageTitle.color = "red";
            }
            function onServerStoped_signal()
            {
                pageTitle.text1 = qsTr ( " you are offline now  " );
                pageTitle.color = "red";
            }
            function onWeAreConnected_signal()
            {
                pageTitle.text1 = inConversationWith;
                pageTitle.color = "black";
            }
            function onContactOnline_signal()
            {
                pageTitle.text1 = inConversationWith;
                pageTitle.color = "black";
            }
    }

    header: ChatToolBar
    {
        ToolButton
        {
            text: qsTr( "Back" )
            anchors.left: parent.left
            anchors.leftMargin: 10
            anchors.verticalCenter: parent.verticalCenter
            onClicked:
            {
                root.StackView.view.pop()
                backend.liveConversationPage()
            }
        }

        Label
        {
            property alias text1: pageTitle.text
            property alias color: pageTitle.color

            id: pageTitle
            text: inConversationWith
            font.pixelSize: 20
            anchors.centerIn: parent            
        }
    }

    ColumnLayout
    {
        anchors.fill: parent
        ListView
        {
            id: listView
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.margins: pane.leftPadding + messageField.leftPadding
            displayMarginBeginning: 40
            displayMarginEnd: 40
            verticalLayoutDirection: ListView.BottomToTop
            spacing: 12
            model: SqlConversationModel
            {
                id: modelclass
                recipient: inConversationWith
            }
            delegate: Column
            {
                anchors.right: sentByMe ? listView.contentItem.right : undefined
                spacing: 6

                readonly property bool sentByMe: model.recipient !== "Me"

                Row
                {
                    id: messageRow
                    spacing: 6
                    anchors.right: sentByMe ? parent.right : undefined
                    Rectangle
                    {
                        width: Math.min( messageText.implicitWidth + 24, listView.width - messageRow.spacing )
                        height: messageText.implicitHeight + 24
                        color: sentByMe ? "lightgrey" : "steelblue"
                        Label
                        {
                            id: messageText
                            text: model.message
                            color: sentByMe ? "black" : "white"
                            anchors.fill: parent
                            anchors.margins: 12
                            wrapMode: Label.Wrap
                        }
                    }
                }

                Label
                {
                    id: timestampText
                    text: Qt.formatDateTime( model.timestamp, "d MMM hh:mm" )
                    color: "lightgrey"
                    anchors.right: sentByMe ? parent.right : undefined
                }
            }
            ScrollBar.vertical: ScrollBar {}
        }

        Pane
        {
            id: pane
            Layout.fillWidth: true
            RowLayout
            {
                width: parent.width
                TextArea
                {
                    id: messageField
                    Layout.fillWidth: true
                    placeholderText: qsTr( "Compose message" )
                    wrapMode: TextArea.Wrap
                }
                Button
                {
                    id: sendButton
                    text: qsTr( "Send" )
                    enabled: messageField.length > 0
                    onClicked:
                    {
                        listView.model.sendMessage( inConversationWith, messageField.text );
                        backend.sendMessageToRecipientClicked( inConversationWith, messageField.text );
                        messageField.text = "";
                    }
                }
            }
        }
    }

    Component.onCompleted:
    {
        backend.startConversationPage();
    }
}
