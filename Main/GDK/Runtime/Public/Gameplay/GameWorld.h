#pragma once

#include <Platform.h>
#include <DeviceContext.h>
#include <VisualInfo.h>

namespace GDK
{
    struct IGameObject;
    struct GameTime;

    struct IGameWorld
    {
        virtual const DeviceContext& GetDeviceContext() const = 0;
        virtual bool IsEditing() const = 0;

        virtual const std::vector<VisualInfo>& GetModel(_In_ uint32_t index) const = 0;

        virtual std::shared_ptr<IGameObject> CreateObject(_In_ const std::map<std::wstring, std::wstring>& properties) = 0;
        virtual void RemoveObject(_In_ const std::shared_ptr<IGameObject>& gameObject) = 0;

        virtual void FindObjectsByClassName(_In_ const std::wstring& className, _Inout_ std::vector<std::shared_ptr<IGameObject>>& gameObjects) const = 0;
        virtual void FindObjectsByTargetName(_In_ const std::wstring& targetName, _Inout_ std::vector<std::shared_ptr<IGameObject>>& gameObjects) const = 0;

        virtual const GameTime& GetTime() const = 0;
    };
}
