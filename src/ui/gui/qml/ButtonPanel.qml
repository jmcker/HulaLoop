import QtQuick 2.10
import QtQuick.Window 2.0

import QtQuick.Controls 2.3
import QtQuick.Dialogs 1.0
import QtQuick.Layouts 1.3

import Qt.labs.platform 1.0

import "../fonts/Icon.js" as MDFont

Rectangle {

    id: buttonPanel

    width: parent.width
    height: transportLayout.height
    color: "lightgrey"

    Timer {
        id: countDownTimer
        objectName: "countDownTimer"

        interval: 1000
        running: false
        repeat: true
        onTriggered: {
            if (textCountdown.time == 0) {
                textCountdown.text = 0
                countDownTimer.stop()
                qmlbridge.record()
                recordingTimer.start()
            } else {
                textCountdown.text = textCountdown.time
                textCountdown.time--
            }
        }
    }

    Timer {
        id: recordingTimer
        objectName: "recordingTimer"

        interval: 1000
        running: false
        repeat: true
        onTriggered: {
            // Since the timer starts at 0, go to endTime - 1
            if (textCountdown.time >= textCountdown.time2 - 1) {
                textCountdown.text = textCountdown.time + 1
                qmlbridge.stop()
                recordingTimer.stop()
            } else {
                textCountdown.text = textCountdown.time + 1
                textCountdown.time++
            }
        }
    }

    RowLayout {
        id: transportLayout
        width: buttonPanel.width

        Item {
            Layout.preferredWidth: 10
        }

        RowLayout {
            id: rowLayout

            spacing: Math.round(buttonPanel.width * 0.0075)

            RoundButton {
                id: recordBtn
                objectName: "recordBtn"

                display: AbstractButton.TextOnly
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter

                background: Rectangle {
                    opacity: enabled ? 1 : 0.15
                    color: recordBtn.pressed ? "grey" : "darkgrey"
                    radius: recordBtn.width / 2
                }

                contentItem: Text {
                    font.family: "Material Design Icons"
                    font.pixelSize: Math.ceil(buttonPanel.width * 0.02)
                    text: MDFont.Icon.record
                    color: "red"

                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }

                onClicked: {

                    let success = qmlbridge.record()

                    if(qmlbridge.getTransportState() === "Recording") // TODO: Check record call success
                    {
                        // Update stop button
                        stopBtn.enabled = true;

                        // Update play/pause button
                        playpauseBtn.enabled = true;
                        playpauseBtn.contentItem.text = MDFont.Icon.pause;
                        playpauseBtn.contentItem.color = "white";

                        // Update self
                        enabled = false;
                    }
                }
            }

            RoundButton {
                id: stopBtn
                objectName: "stopBtn"
                enabled: false
                property bool isStopped: false

                display: AbstractButton.TextOnly
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter

                background: Rectangle {
                    opacity: enabled ? 1 : 0.15
                    color: stopBtn.pressed ? "grey" : "darkgrey"
                    radius: stopBtn.width / 2
                }

                contentItem: Text {
                    font.family: "Material Design Icons"
                    font.pixelSize: Math.ceil(buttonPanel.width * 0.02)
                    text: MDFont.Icon.stop
                    color: "black"

                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }

                onClicked: {
                    let success = qmlbridge.stop()

                    if(qmlbridge.getTransportState() === "Stopped")
                    {
                        enabled = false;

                        recordBtn.enabled = false;
                        isStopped = true;

                        playpauseBtn.enabled = true;
                        playpauseBtn.contentItem.text = MDFont.Icon.play;
                        playpauseBtn.contentItem.color = "green";

                        exportBtn.enabled = true;
                    }
                }
            }

            RoundButton {
                id: playpauseBtn
                objectName: "playpauseBtn"
                enabled: false

                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                display: AbstractButton.TextOnly

                contentItem: Text {
                    objectName: "play_icon"
                    font.family: "Material Design Icons"
                    font.pixelSize: Math.ceil(buttonPanel.width * 0.02)
                    text: MDFont.Icon.pause
                    color: "white"

                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }

                background: Rectangle {
                    opacity: enabled ? 1 : 0.15
                    color: playpauseBtn.pressed ? "grey" : "darkgrey"
                    radius: playpauseBtn.width / 2
                }

                onClicked: {

                    let success;

                    if(contentItem.text == MDFont.Icon.pause)
                    {
                        success = qmlbridge.pause();

                        if(qmlbridge.getTransportState() === "Paused")
                        {
                            contentItem.text = MDFont.Icon.play;
                            contentItem.color = "green";

                            if(!stopBtn.isStopped)
                            {
                                stopBtn.enabled = true;
                                recordBtn.enabled = true;
                            }
                        }
                    }
                    else
                    {
                        success = qmlbridge.play();

                        if(qmlbridge.getTransportState() === "Playing")
                        {
                            contentItem.text = MDFont.Icon.pause;
                            contentItem.color = "white";

                            stopBtn.enabled = false; // TODO: Determine final state of playback button
                            recordBtn.enabled = false;
                        }
                    }

                }
            }

            RoundButton {
                id: timerBtn
                objectName: "timerBtn"

                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                display: AbstractButton.TextOnly

                contentItem: Text {
                    font.family: "Material Design Icons"
                    font.pixelSize: Math.ceil(buttonPanel.width * 0.02)
                    text: MDFont.Icon.timer
                    color: "white"

                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }

                background: Rectangle {
                    opacity: enabled ? 1 : 0.15
                    color: timerBtn.pressed ? "grey" : "darkgrey"
                    radius: timerBtn.width / 2
                }

                onClicked: timerPopup.open()
            }

            RoundButton {
                id: exportBtn
                objectName: "exportBtn"
                enabled: false

                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                display: AbstractButton.TextOnly

                contentItem: Text {
                    font.family: "Material Design Icons"
                    font.pixelSize: Math.ceil(buttonPanel.width * 0.02)
                    text: MDFont.Icon.export
                    color: "white"
                    //transform: Rotation {angle: 270}

                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }

                background: Rectangle {
                    opacity: enabled ? 1 : 0.15
                    color: exportBtn.pressed ? "grey" : "darkgrey"
                    radius: exportBtn.width / 2
                }

                onClicked:saveDialog.open()
            }

            RoundButton {
                id: checkUpdateBtn
                objectName: "checkUpdateBtn"

                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                display: AbstractButton.TextOnly

                contentItem: Text {
                    font.family: "Material Design Icons"
                    font.pixelSize: Math.ceil(buttonPanel.width * 0.02)
                    text: MDFont.Icon.download
                    color: "white"

                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }

                background: Rectangle {
                    opacity: enabled ? 1 : 0.15
                    color: timerBtn.pressed ? "grey" : "darkgrey"
                    radius: timerBtn.width / 2
                }

                onClicked: qmlbridge.launchUpdateProcess()
            }

        }

        FileDialog {
            id: saveDialog
            objectName: "saveDialog"
            fileMode: FileDialog.SaveFile
            nameFilters: saveDialog.nameFilters
            folder: StandardPaths.writableLocation(StandardPaths.DocumentsLocation)
            onAccepted: {
                qmlbridge.saveFile(saveDialog.currentFile);
//                console.log(saveDialog.fileUrl.toString());
            }
        }

        RowLayout {
            id: timeLabelLayout

            Label {
                id: timerLabel

                color: "black"
                font.pixelSize: Math.ceil(buttonPanel.width * 0.02)
                text: qsTr("Timer:")
            }

            Text {
                id: textCountdown
                objectName: "textCountdown"

                color: "black"
                font.pixelSize: Math.ceil(buttonPanel.width * 0.02)
                text: "0"

                property int time: getTime()
                property int time2: getTime2()
                function getTime() {
                    var d = delayInput.text
                    var h = parseInt(d.substring(0, 2))
                    var m = parseInt(d.substring(3, 5))
                    var s = parseInt(d.substring(6, 8))
                    var timeRem = h * 60 * 60 + m * 60 + s
                    return timeRem
                }
                function getTime2() {
                    var d = recordTimeInput.text
                    var h = parseInt(d.substring(0, 2))
                    var m = parseInt(d.substring(3, 5))
                    var s = parseInt(d.substring(6, 8))
                    var timeRem = h * 60 * 60 + m * 60 + s
                    return timeRem
                }
            }
        }

        GridLayout {
            Layout.alignment: Qt.AlignRight
            rows: 2
            columns: 2
            Label {
                id: inputDeviceLabel

                color: "black"
                text: qsTr("Input Device:")
            }
            ComboBox {
                id: iDeviceInfoLabel
                Layout.preferredWidth: Math.max(Math.round(window.width * 0.2), 320)
                model: ListModel {
                    id: iDeviceItems
                    Component.onCompleted: {
                        var idevices = qmlbridge.getInputDevices().split(',')
                        var i
                        for (i = 0; i < idevices.length; i++) {
                            append({
                                       "text": idevices[i]
                                   })
                        }
                    }
                }
                onActivated: {
                    console.log("Audio device has been changed to: " + iDeviceInfoLabel.currentText);
                    qmlbridge.setActiveInputDevice(iDeviceInfoLabel.currentText);
                }

                onPressedChanged: {
                    model.clear();
                    var idevices = qmlbridge.getInputDevices().split(',')
                        var i
                        for (i = 0; i < idevices.length; i++) {
                            model.append({
                                       "text": idevices[i]
                                   })
                        }
                }
                currentIndex: 0
            }
            Label {
                id: outputDeviceLabel

                color: "black"
                text: qsTr("Output Device:")
            }
            ComboBox {
                id: oDeviceInfoLabel
                Layout.preferredWidth: Math.max(Math.round(window.width * 0.2), 320)
                model: ListModel {
                    id: oDeviceItems
                    Component.onCompleted: {
                        var odevices = qmlbridge.getOutputDevices().split(',')
                        var i
                        for (i = 0; i < odevices.length; i++) {
                            append({
                                       "text": odevices[i]
                                   })
                        }
                    }
                }
                onActivated: {
                    console.log("Audio device has been changed to: " + oDeviceInfoLabel.currentText);
                    qmlbridge.setActiveOutputDevice(oDeviceInfoLabel.currentText);
                }

                onPressedChanged: {
                    model.clear();
                    var odevices = qmlbridge.getOutputDevices().split(',')
                        var i
                        for (i = 0; i < odevices.length; i++) {
                            model.append({
                                       "text": odevices[i]
                                   })
                        }
                }
                currentIndex: 0
            }
        }

        Item {
            Layout.preferredWidth: 10
        }
    }

    Popup {
        id: timerPopup
        objectName: "timerPopup"

        x: Math.round((window.width - width) / 2)
        y: Math.round((window.height - height) / 2)

        modal: true
        focus: true
        closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside

        ColumnLayout {
            spacing: Math.round(buttonPanel.height * 0.15)

            GridLayout {
                id: gridLayout
                Layout.alignment: Qt.AlignTop
                rows: 3
                columns: 2

                Label {
                    font.family: "Roboto"
                    font.pixelSize: Math.ceil(buttonPanel.width * 0.02)
                    text: qsTr("Delay Recording (hh:mm:ss)")
                    color: "white"

                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }

                TextInput {
                    id: delayInput
                    objectName: "delayInput"

                    text: "00:00:00"
                    inputMask: "00:00:00"
                    color: "white"
                }

                Label {
                    font.family: "Roboto"
                    font.pixelSize: Math.ceil(buttonPanel.width * 0.02)
                    text: qsTr("Record Duration (hh:mm:ss)")
                    color: "white"

                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }

                TextInput {
                    id: recordTimeInput
                    objectName: "recordTimeInput"

                    text: "00:00:00"
                    inputMask: "00:00:00"
                    color: "white"
                }
            }

            RowLayout {
                Layout.alignment: Qt.AlignCenter
                spacing: Math.round(buttonPanel.width * 0.05)
                width: gridLayout.width / 2

                Button {
                    Layout.alignment: Qt.AlignLeft
                    id: cancelBtn
                    onClicked: {
                        timerPopup.close()
                    }
                    Layout.preferredWidth: Math.round(buttonPanel.width * 0.15)
                    contentItem: Text {
                        font.family: "Roboto"
                        font.pixelSize: Math.ceil(buttonPanel.width * 0.02)
                        text: qsTr("CANCEL")
                        color: "white"

                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                }

                Button {
                    Layout.alignment: Qt.AlignRight
                    id: okBtn
                    onClicked: {
                        countDownTimer.start()
                        timerPopup.close()
                    }

                    Layout.preferredWidth: Math.round(buttonPanel.width * 0.15)
                    contentItem: Text {
                        font.family: "Roboto"
                        font.pixelSize: Math.ceil(buttonPanel.width * 0.02)
                        text: qsTr("OK")
                        color: "white"

                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                }
            }
        }
    }
}
