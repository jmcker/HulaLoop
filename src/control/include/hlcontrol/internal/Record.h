#ifndef HL_RECORD_H
#define HL_RECORD_H

#include <thread>

#include <hlaudio/hlaudio.h>

namespace hula
{
    /**
     * Class for Recording audio and abstracting OS specific stuff
     */
    class Record {

        private:
            Controller *controller;
            HulaRingBuffer *rb;

            std::thread recordThread;
            std::atomic<bool> endRecord;

            std::vector<std::string> exportPaths;

        public:
            Record(Controller *control);
            ~Record();

            void recorder();

            std::vector<std::string> getExportPaths();
            void clearExportPaths();

            void start();
            void stop();
    };
}

#endif // END HL_RECORD_H