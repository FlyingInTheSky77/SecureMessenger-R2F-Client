import QtQuick 2.12
import QtQuick.Controls 2.12
import r2f.SqlContactModel 1.0


Page
{
    id: root

    Connections
    {
        target: backend
        function onServerStoped_signal()
        {
            pageTitle.text1 = qsTr ( " Contacts ( you are offline now ) " );
            pageTitle.color = "red";
        }
        function onWeAreConnected_signal()
        {
            pageTitle.text1 = qsTr( "Contacts" );
            pageTitle.color = "black";
        }        
        function onUpdateQMLModelView_sigal()
        {
            contactModel.refresh()
        }
    }

    SqlContactModel {
        id: contactModel
    }

    header: ChatToolBar
    {
        ToolButton
        {
            text: qsTr( "Back" )
            anchors.left: parent.left
            anchors.leftMargin: 10
            anchors.verticalCenter: parent.verticalCenter
            onClicked: root.StackView.view.pop()
        }

        Label
        {
            property alias text1: pageTitle.text
            property alias color: pageTitle.color

            id: pageTitle
            text: qsTr( "Contacts" )
            font.pixelSize: 20
            anchors.centerIn: parent
        }
    }

    ListView
    {
        id: contactModellistView
        anchors.fill: parent
        topMargin: 48
        leftMargin: 48
        bottomMargin: 48
        rightMargin: 48
        spacing: 20
        model: contactModel
        delegate: ItemDelegate
        {
            text: model.display
            width: contactModellistView.width - contactModellistView.leftMargin - contactModellistView.rightMargin
            leftPadding: 32
            onClicked:
            {
                root.StackView.view.push("qrc:/qml/ConversationPage.qml", { inConversationWith: model.display })
            }
        }
    }
}
