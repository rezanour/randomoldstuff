#pragma once

#include "IRefCounted.h"

namespace GDK
{
    __interface ITask;

    enum DeviceFlags
    {
        DeviceFlags_Input = 0x01,
        DeviceFlags_Output = 0x02,
        DeviceFlags_Debug = 0x04,
    };

    __interface IDevice : public IRefCounted
    {
        // Properties
        const char* GetName() const;
        DeviceFlags GetFlags() const;

        // Execution
        HRESULT OnInitialize();
        HRESULT OnMessage();
        HRESULT OnCleanup();
    };

    // Registers a device. The task in which the device is hosted is returned, and may be used to interact with the system.
    HRESULT RegisterDevice(/* message filter, */ IDevice* device, ITask** ppHostTask);
} // GDK
