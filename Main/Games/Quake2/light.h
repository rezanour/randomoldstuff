#pragma once

#include <GDK.h>
#include <GameObject.h>
#include <GameWorld.h>
#include <SpacePartition.h>
#include <Transform.h>

#include "QuakeProperties.h"

namespace Quake2
{
    class Light : 
        public GDK::RuntimeObject<Light>,
        public GDK::IGameObjectController
    {
    public:
        enum class Type
        {
            None
        };

        // Creation
        static GDK::GameObjectCreateParameters Create(_In_ const std::shared_ptr<GDK::IGameWorld>& gameWorld, _In_ const std::shared_ptr<QuakeProperties>& quakeProps);

        // IGameObjectController
        virtual uint32_t GetTypeID() const override;
        virtual void OnCreate(_In_ const std::weak_ptr<GDK::IGameObject>& gameObject) override;
        virtual void OnDestroy() override;
        virtual void OnUpdate() override;
        virtual void OnActivate() override;
        virtual void AppendProperties(_Inout_ std::map<std::wstring, std::wstring>& properties) const override;

    private:
        Light();

        std::weak_ptr<GDK::IGameObject> _gameObject;
    };
}
