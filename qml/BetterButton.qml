import QtQuick 2.15
import QtQuick.Controls 2.3

Button
{
    id: control

    implicitWidth: 150
    implicitHeight: 40

    leftPadding: padding
    topPadding: padding + 5
    rightPadding: padding
    bottomPadding: padding + 5    

    property alias color: back.color
    property alias border: back.border

    contentItem: Text
    {
        text: control.text
        font.pixelSize: defpixelSize
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        anchors.fill: parent
    }

    background: Rectangle
    {
        id: back
        radius: 5
        border.width: control.activeFocus ? 4 : 0
        border.color: control.activeFocus ? "blue" : "transparent"
    }

    Keys.onPressed: {
            if (event.key === Qt.Key_Return || event.key === Qt.Key_Enter) {
                control.clicked()
                event.accepted = true
            }
        }
}
