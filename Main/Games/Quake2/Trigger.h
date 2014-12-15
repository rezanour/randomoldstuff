#pragma once

#include <GDK.h>
#include <GameObject.h>
#include "QuakeProperties.h"

namespace Quake2
{
    class Trigger : 
        public GDK::RuntimeObject<Trigger>,
        public GDK::IGameObjectController
    {
    public:
        enum class Type
        {
            Once,
            Relay,
            Always,
            Multiple,
            Timer
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
        Trigger(_In_ Type type, _In_ const std::wstring& target, _In_ const float delay);

        std::weak_ptr<GDK::IGameObject> _gameObject;
        Type _type;
        std::vector<std::shared_ptr<GDK::IGameObject>> _targets;
        std::wstring _target;
        float _delay;
        float _elapsedTime;
        bool _fired;
        bool _expired;
    };
}
