#ifndef QMLBRIDGE_H
#define QMLBRIDGE_H

#include <atomic>
#include <QObject>
#include <QString>
#include <QStringList>
#include <thread>
#include <vector>

#include <hlaudio/hlaudio.h>
#include <hlcontrol/hlcontrol.h>

namespace hula
{
    /**
     * Class for communicating between QML and C++.
     * This is designed to be added as a QML type and used in QML.
     */
    class QMLBridge : public QObject {
            Q_OBJECT

        private:
            Transport *transport;
            HulaRingBuffer *rb;

            std::vector<std::thread> visThreads;
            std::atomic<bool> endVis;

        public:
            explicit QMLBridge(QObject *parent = nullptr);
            virtual ~QMLBridge();

            Q_INVOKABLE void setActiveInputDevice(QString QDeviceName);
            Q_INVOKABLE void setActiveOutputDevice(QString QDeviceName);
            Q_INVOKABLE QString getInputDevices();
            Q_INVOKABLE QString getOutputDevices();

            Q_INVOKABLE QString getTransportState() const;
            Q_INVOKABLE bool record();
            Q_INVOKABLE bool stop();
            Q_INVOKABLE bool play();
            Q_INVOKABLE bool pause();
            Q_INVOKABLE void discard();

            Q_INVOKABLE void saveFile(QString dir);
            Q_INVOKABLE void cleanTempFiles();
            Q_INVOKABLE bool wannaClose();

            void startVisThread();
            void stopVisThread();
            static void updateVisualizer(QMLBridge* _this);
            static void reverseBits(size_t x, int n);

            Q_INVOKABLE void launchUpdateProcess();

        signals:

            /**
             * Signal emmitted when the Transport changes states.
             * Keeps the UI's state machine on the same page.
             */
            void stateChanged();

            /**
             * Signal emitted when the visualizer needs to update
             */
            void visData(std::vector<qreal> dataIn);
    };
}

#endif // QMLBRIDGE_H