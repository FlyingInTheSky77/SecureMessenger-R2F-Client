import QtQuick 2.15
import QtQuick.Controls 2.5

ApplicationWindow
{
    id: window
    width: 490
    height: 640
    visible: true
    maximumWidth: 490
    minimumWidth: 490
    maximumHeight: 640
    minimumHeight: 640

    title: qsTr( "R2F Messenger - Client" )

    readonly property int defpixelSize: 15

    StackView
    {
        id: stackView
        initialItem: HomePage{}
        anchors.fill: parent
    }
}
