#pragma once

class GameObjectNew : public BaseObject<GameObjectNew>, public IGameObject
{
public:
    static std::shared_ptr<GameObjectNew> CreateGameObject();

    //
    // IGameObject
    //

    virtual const Transform& GetTransform() const override;

private:
    GameObjectNew();

private:

    Transform _transform;
};