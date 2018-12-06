#ifndef HL_OS_AUDIO_H
#define HL_OS_AUDIO_H

// System
#include <atomic>
#include <cstdlib>
#include <thread>
#include <vector>

#include "Device.h"
#include "HulaRingBuffer.h"

namespace hula
{
    /**
     * Abstract class that defines the required components
     * for OS specfic audio classes.
     */
    class OSAudio {
        private:
            void joinAndKillThreads(std::vector<std::thread> &threads);

        protected:

            /**
             * Constructor is protected since this class is abstract.
             */
            OSAudio()
            {
                this->activeInputDevice = nullptr;
                this->activeOutputDevice = nullptr;

                playbackBuffer = new HulaRingBuffer(1);

                endCapture.store(true);
                endPlay.store(true);
            };

            /**
             * The selected input device
             */
            Device *activeInputDevice;

            /**
             * The selected output device
             */
            Device *activeOutputDevice;

            /**
             * List of all added ring buffers.
             * Data received from the operating system is copied into each of these buffers.
             */
            std::vector<HulaRingBuffer *> rbs;

            /**
             * Thread for input device activities.
             */
            std::vector<std::thread> inThreads;

            /**
             * Thread for output device activities.
             */
            std::vector<std::thread> outThreads;

            /**
             * Flag to syncronize the capture thread for an instance.
             * This is used to break the capture loop when switching devices
             * or when 0 buffers are present.
             *
             * Should never be set directly. Only by setActiveXXXDevice().
             */
            std::atomic<bool> endCapture;

            /**
             * Flag to syncronize the playback thread for an instance.
             * This is used to break the playback loop when switching devices
             * or when 0 buffers are present.
             *
             * Should never be set directly. Only by setActiveXXXDevice().
             */
            std::atomic<bool> endPlay;

            /**
             * I don't really know what this is for right now
             * but I'm going to add this comment so that Doxygen
             * will quit complaining.
             * Peace. Love. Happiness. Doxygen :)
             */
            uint32_t captureBufferSize;

        public:
            HulaRingBuffer *playbackBuffer;

            virtual ~OSAudio() = 0;

            void setBufferSize(uint32_t size);

            void addBuffer(HulaRingBuffer *rb);
            void removeBuffer(HulaRingBuffer *rb);

            void startPlayback();
            void endPlayback();
            void copyToBuffers(const float *samples, ring_buffer_size_t sampleCount);
            ring_buffer_size_t playbackCopyToBuffers(const float *samples, ring_buffer_size_t sampleCount);

            /**
             * Receive the list of available record, playback and/or loopback audio devices
             * connected to the OS and return them as Device instances
             *
             * @param type DeviceType that is combination from the DeviceType enum
             * @return std::vector<Device*> A list of Device instances that carry the necessary device information
             */
            virtual std::vector<Device *> getDevices(DeviceType type) = 0;

            /**
             * Execution loop for loopback capture
             */
            virtual void capture() = 0;
            static void backgroundCapture(OSAudio *_this);

            /**
             * Execution loop for audio playback
             */
            void playback();
            static void backgroundPlayback(OSAudio *_this);

            /**
             * Verify the bit rate of set rate with the hardware device compatibility
             */
            virtual bool checkDeviceParams(Device *device) = 0;

            virtual bool setActiveInputDevice(Device *device);
            virtual bool setActiveOutputDevice(Device *device);
    };
}

#endif // END HL_OS_AUDIO_H