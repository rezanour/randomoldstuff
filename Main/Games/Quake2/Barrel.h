#pragma once

#include <GDK.h>
#include <GameObject.h>
#include <GameWorld.h>
#include <SpacePartition.h>
#include <Transform.h>

#include "QuakeProperties.h"

namespace Quake2
{
    class Barrel : 
        public GDK::RuntimeObject<Barrel>,
        public GDK::IGameObjectController
    {
    public:
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
        Barrel();

        std::weak_ptr<GDK::IGameObject> _gameObject;
    };
}
