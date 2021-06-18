import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.3
import io.qt.Backend 1.0
import r2f.SqlContactModel 1.0

Page
{
    id: root
    Connections
    {
        target: backend
        function onStatusChanged_signal( newStatus )
        {
            ti.append( addMsg( qsTr( "connection status changed:" ) + newStatus ) );
            if ( currentStatus !== true )
            {
                btn_connect.enabled = true;
            }
        }
        function onSomeMessage_signal( msg )
        {
            ti.append( addMsg( qsTr( "server message: " ) + msg ) );
        }
        function onSystemMessage_signal( msg )
        {
            ti.append( addMsg( qsTr( "system message: " ) + msg) );
        }
        function onWeAreConnected_signal()
        {
            singup.enabled = true;
            singin.enabled = true;
            btn_send.enabled = true;
            btn_connect.enabled = false;
        }
        function onServerStoped_signal()
        {
            btn_send.enabled = false;
            singup.enabled = false;
            singin.enabled = false;
            btn_connect.enabled = true;
            btn_disconnect.enabled = false;
        }
        function onInSystem_signal()
        {
            contscts_show_buton.enabled = true;
            btn_disconnect.enabled = true;
        }
    }

    header: ChatToolBar
    {
        Label
        {
            text: qsTr( "Start page" )
            font.pixelSize: 20
            anchors.centerIn: parent
        }
    }
            ColumnLayout
            {
                Layout.preferredWidth: root.width
                id: chatpannel
                Layout.fillWidth: true
                Layout.fillHeight: true

                LayoutSection
                {
                    Layout.leftMargin: 15
                    Layout.rightMargin: 15
                    Layout.alignment: Qt.AlignHCenter
                    height: status.height + 15
                    color: backend.currentStatus ? "#CAF5CF" : "#EA9FA9"
                    Text
                    {
                        id: status
                        anchors.centerIn: parent
                        text: backend.currentStatus ? qsTr( "CONNECTED" ) : qsTr( "DISCONNECTED" )
                        font.weight: Font.Bold
                    }
                }
                RowLayout
                {
                    Layout.alignment: Qt.AlignHCenter
                    Layout.leftMargin: 15
                    Layout.rightMargin: 15
                    id: connectdisconnect
                    BetterButton
                    {
                        id: btn_connect
                        Layout.fillWidth: true
                        text: qsTr( "Connect" )
                        color: enabled ? this.down ? "#78C37F" : "#87DB8D" : "#CED0D4"
                        border.color: "#78C37F"
                        onClicked:
                        {
                            ti.append( addMsg( qsTr( "system message: try to connect to server" ) ) );
                            backend.connectClicked( servername.text, portnumber.text );
                        }
                    }
                    Rectangle{
                        id:rec_server
                        Layout.fillWidth: true
                        height: servernameinputarea.height + 20
                        width: 50
                        Text
                        {
                             id:servernameinputarea
                             text: qsTr( "server:" )
                             anchors.centerIn: rec_server
                             leftPadding: 5
                             rightPadding: 5
                             font.pixelSize: defpixelSize
                         }
                    }
                    Rectangle {
                        Layout.fillWidth: true
                        height: servername.height + 20
                        radius: 5
                        width: 100
                        color: "#F4F2F5"
                        border.width: 1
                        TextInput
                        {
                            id: servername
                            anchors.verticalCenter: parent.verticalCenter
                            leftPadding: 3
                            rightPadding: 3
                            width: parent.width
                            font.pixelSize: defpixelSize
                            clip: true
                        }
                    }
                    Rectangle
                    {
                        id: rec_port
                        Layout.fillWidth: true
                        height: port.height + 20
                        width: 30
                        Text
                        {
                            id:port
                            text: qsTr( "port:" )
                            font.pixelSize: defpixelSize
                            anchors.centerIn: rec_port
                        }
                    }
                    Rectangle
                    {
                        Layout.fillWidth: true
                        height: portnumber.height + 20
                        radius: 5
                        width: 100
                        color: "#F4F2F5"
                        border.color: "gray"
                        border.width: 1

                        TextInput
                        {
                            id: portnumber
                            anchors.verticalCenter: parent.verticalCenter
                            leftPadding: 10
                            rightPadding: 10
                            width: parent.width
                            font.pixelSize: defpixelSize
                            clip: true
                        }
                    }
                    BetterButton
                    {
                        id: btn_disconnect
                        Layout.fillWidth: true
                        enabled: !btn_connect.enabled
                        text: qsTr ( "Disconnect" )
                        color: enabled ? this.down ? "#DB7A74" : "#FF7E79" : "#CED0D4"
                        onClicked:
                        {
                            ti.append( addMsg( qsTr( "Button \"Disconnect\" from server clicked" ) ) );
                            backend.disconnectClicked_slot();
                            contscts_show_buton.enabled = false;
                            singup.enabled = false;
                            singin.enabled = false;
                            btn_send.enabled = false;
                            btn_connect.enabled = true;
                        }
                    }
                }
                RowLayout
                {
                    Layout.alignment: Qt.AlignHCenter
                    Layout.leftMargin: 15
                    Layout.rightMargin: 15
                    id: enterusernamenew
                    Rectangle
                    {
                        id: rec_username
                        Layout.fillWidth: true
                        height: usertext.height+20
                        width: 50
                        Text
                        {
                                id: usertext
                                text: qsTr( "username:" )
                                font.pixelSize: defpixelSize
                                leftPadding: 5
                                rightPadding: 5
                                anchors.centerIn: rec_username

                        }
                    }
                    Rectangle
                    {
                        Layout.fillWidth: true
                        height: username.height + 20
                        radius: 5
                        color: "#F4F2F5"
                        border.color: "gray"
                        border.width: 1
                        width: 100
                        TextInput
                        {
                            id: username
                            anchors.verticalCenter: parent.verticalCenter
                            leftPadding: 10
                            rightPadding: 10
                            width: parent.width
                            font.pixelSize: defpixelSize
                            clip: true
                        }
                    }
                }
                RowLayout
                {
                    Layout.alignment: Qt.AlignHCenter
                    Layout.leftMargin: 15
                    Layout.rightMargin: 15
                    id: enterpasswordnew
                    Rectangle
                    {
                        id: rec_pass
                        Layout.fillWidth: true
                        height: passwordtext.height + 20
                        width: 50
                        Text
                        {
                            id: passwordtext
                            text: qsTr( "password:" )
                            font.pixelSize: defpixelSize
                            anchors.centerIn: rec_pass
                        }
                    }
                    Rectangle
                    {
                        Layout.fillWidth: true
                        height: password.height + 20
                        radius: 5
                        color: "#F4F2F5"
                        border.color: "gray"
                        border.width: 1                        
                        width:100
                        TextInput
                        {
                            id: password
                            anchors.verticalCenter: parent.verticalCenter
                            leftPadding: 10
                            rightPadding: 10
                            width: parent.width
                            font.pixelSize: defpixelSize
                            clip: true
                        }
                    }
                }
                RowLayout
                {
                    id:buttonloginsignup
                    Layout.leftMargin: 15
                    Layout.rightMargin: 15
                    Layout.topMargin: 5
                    Layout.bottomMargin: 5
                    Layout.fillWidth: true
                    width: parent.width
                    BetterButton
                    {
                        id: singin
                        Layout.fillWidth: true
                        Text
                        {
                            id: authorization_text
                            text: qsTr( "authorization" )
                            anchors.centerIn: parent

                        }
                        color: enabled ? "#86a4e5" : "#CED0D4"
                        onClicked: {
                            ti.append( addMsg( qsTr( "system message: trying to singin" ) ) );
                            backend.authorizationClicked( username.text, password.text );
                        }
                    }
                    BetterButton
                    {
                        id: singup
                        Layout.fillWidth: true
                        Text
                        {
                            text: qsTr( "registration" )
                            anchors.centerIn: parent
                        }
                        color: enabled ? "#86a4e5" : "#CED0D4"
                        onClicked:
                        {
                            ti.append( addMsg( qsTr( "system message: trying to singup" ) ) );
                            backend.registerClicked( username.text, password.text );
                        }
                    }
                }

                BetterButton
                {
                    Layout.leftMargin: 15
                    Layout.rightMargin: 15
                    Layout.topMargin: 5
                    id: contscts_show_buton
                    Layout.fillWidth: true
                    Text
                    {
                        text: qsTr( "Show contacts" )
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.horizontalCenter: parent.horizontalCenter
                    }
                    color: enabled ? "#86a4e5" : "#CED0D4"
                    onClicked:
                    {
                        ti.append( addMsg( qsTr( "Show contacts" ) ) );
                        root.StackView.view.push( "qrc:/qml/ContactPage.qml" );
                    }
                }
                RowLayout
                {
                    id: messangearea
                    Layout.leftMargin: 15
                    Layout.rightMargin: 15
                    Layout.topMargin: 5
                    Layout.bottomMargin: 5
                    Rectangle
                    {
                        Layout.fillWidth: true
                        height: btn_send.height
                        color: "#F4F2F5"
                        border.color: "gray"
                        border.width: 1
                        radius: 5
                        TextInput
                        {
                            id: msgToSend
                            anchors.verticalCenter: parent.verticalCenter
                            leftPadding: 10
                            rightPadding: 10
                            width: parent.width
                            font.pixelSize: defpixelSize
                            clip: true
                        }
                    }
                    BetterButton
                    {
                        id: btn_send
                        text: qsTr( "Send to server" )
                        color: enabled ? "#86a4e5" : "#CED0D4"
                        onClicked:
                        {
                           ti.append( addMsg( qsTr( "Sending message to server:" + msgToSend.text ) ) );
                            backend.sendDirectlyToServerClicked( msgToSend.text );
                        }
                   }
                }
                RowLayout
                {
                    width: 450
                    height: 200
                    id: chatarean
                    Layout.leftMargin: 15
                    Layout.rightMargin: 15
                    Layout.topMargin: 10
                    Layout.bottomMargin: 10
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    Rectangle
                    {
                        id: back
                        border.width: 1
                        radius: 5
                        height: chatarean.height
                        width: chatarean.width
                        anchors.leftMargin:10
                        ScrollView
                        {
                            id: scrollView
                            width: back.width
                            height: back.height
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            TextArea
                            {
                                id: ti
                                readOnly: true
                                selectByMouse : true
                                font.pixelSize: defpixelSize
                                wrapMode: TextInput.WrapAnywhere
                            }
                            ScrollBar.vertical: ScrollBar {}
                        }
                    }
                }
            }

            Component.onCompleted:
            {
                ti.text = addMsg( qsTr( "R2F-Messanger-Client started\n- - - - - -" ), false );
                contscts_show_buton.enabled = false;
                singup.enabled = false;
                singin.enabled = false;
                btn_send.enabled = false;
            }

            function addMsg( someText )
            {
                return "[" + currentTime() + "] " + someText;
            }
                function currentTime()
                {
                    var now = new Date();
                    var nowString = ( "0" + now.getHours() ).slice( -2 ) + ":"
                            + ( "0" + now.getMinutes() ).slice( -2 ) + ":"
                            + ( "0" + now.getSeconds() ).slice( -2 );
                    return nowString;
                }
}
