#include "precomp.h"
#include "gamescreen.h"
#include "graphics.h"
#include "gameobject.h"

std::unique_ptr<GameScreen> GameScreen::Create()
{
    return std::unique_ptr<GameScreen>(new GameScreen());
}

GameScreen::GameScreen()
{
    _gameObjects.push_back(GameObject::Create(this));
    _gameObjects.push_back(GameObject::Create(this));
    _gameObjects.push_back(GameObject::Create(this));
    _gameObjects.push_back(GameObject::Create(this));
    _gameObjects.push_back(GameObject::Create(this));
    _gameObjects.push_back(GameObject::Create(this));
    _gameObjects.push_back(GameObject::Create(this));

    auto gameObject(GameObject::Create(this));
    gameObject->SetVisible(true);

    _gameObjects.push_back(gameObject);
}

_Use_decl_annotations_
void GameScreen::Update(float elapsedTime)
{
    UNREFERENCED_PARAMETER(elapsedTime);
}

void GameScreen::Draw()
{
    for (auto gameObject : _gameObjects)
    {
        if (gameObject->IsVisible())
        {
        }
    }
}
