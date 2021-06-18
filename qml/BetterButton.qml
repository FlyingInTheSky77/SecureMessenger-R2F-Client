import QtQuick 2.15
import QtQuick.Controls 2.3

Button
{
    id: control
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
    }

    background: Rectangle
    {
        id: back
        radius: 5
    }
}
