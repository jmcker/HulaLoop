#include "Device.h"

/**
 * Constructs an instance of the Device object
 *
 * @param id ID of the audio device
 * @param name Name of the audio device
 * @param t DeviceType of the audio device
 */
Device::Device(uint32_t *id, string name, DeviceType t)
{
    this->deviceID = id;
    this->deviceName = name;
    this->type = t;
}

/**
 * Set the ID of a system audio device
 *
 * @param id Pointer to ID value due to Windows return values
 */
void Device::setID(uint32_t *id)
{
    this->deviceID = id;
}

/**
 * Get the ID of a system audio device
 *
 * @return id Pointer to a integer
 */
uint32_t *Device::getID()
{
    return this->deviceID;
}

/**
 * Get the name of the system audio device
 *
 * @return name String representing the name of the device
 */
string Device::getName()
{
    return deviceName;
}

/**
 * Set the name of the system audio device
 *
 * @param name String representing the name of the device
 */
void Device::setName(string name)
{
    this->deviceName = name;
}

/**
 * Get the type of system audio device
 *
 * @return type DeviceType enum value
 */
DeviceType Device::getType()
{
    return type;
}

/**
 * Set the type of system audio device
 *
 * @param type DeviceType enum value
 */
void Device::setType(DeviceType t)
{
    this->type = t;
}

/**
 * Deconstructs the device instance
 */
Device::~Device()
{
    delete deviceID;

    deviceName = "";
}