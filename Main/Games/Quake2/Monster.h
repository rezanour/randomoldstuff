#pragma once

#include <GDK.h>
#include <GameObject.h>
#include "QuakeProperties.h"

namespace Quake2
{
    class Monster : 
        public GDK::RuntimeObject<Monster>,
        public GDK::IGameObjectController
    {
    public:
        enum class Type
        {
            None,
            Berserk,
            Bitch,
            Boss1,
            Boss2,
            Boss3Jorg,
            Boss3Rider,
            Brain,
            Commander,
            Flipper,
            Float,
            Flyer,
            Gladiator,
            Gunner,
            Hover,
            Infantry,
            Medic,
            Mutant,
            Parasite,
            Soldier,
            SoldierLt,
            SoldierSS,
            Tank,
            InsaneGuy1,
            InsaneGuy2,
            InsaneGuy3,
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
        Monster(_In_ Type type, _In_ uint16_t health);

        std::weak_ptr<GDK::IGameObject> _gameObject;
        Type _type;
        uint16_t _health;
    };
}
