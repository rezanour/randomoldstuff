#include "precomp.h"
#include "material.h"
#include "simpleobject.h"
#include "staticleveldata.h"

#define TEST_STATIC_LEVEL

GamePlay::GamePlay() :
    _cameraDirty(true),
    _cameraPosition(0, 100, -100),
    _cameraTarget(0, 0, 0)
{
    XMStoreFloat4x4(&_projection, XMMatrixPerspectiveFovLH(XMConvertToRadians(60.0f), (float)GetConfig().ScreenWidth / GetConfig().ScreenHeight, 0.1f, 1000.0f));

#if defined(TEST_STATIC_LEVEL)
    _staticLevelData.reset(GetAssetLoader().LoadStaticLevel("castle\\castle.geometry"));
#endif

    _scene.reset(new Scene());

    //
    // TEST
    //
    GameObjectSpawnInfo info = {};
    info.Type = "testcube";
    info.Position = XMFLOAT3(0, 0, 0);
    XMStoreFloat4(&info.Orientation, XMQuaternionIdentity());

    _gameObject = GameObject::Spawn(this, &info);
    _scene->AddObject(_gameObject);
}

GamePlay::~GamePlay()
{
    _staticLevelData = nullptr;
    delete _gameObject;
}

_Use_decl_annotations_
void GamePlay::Update(float deltaTime, bool hasFocus)
{
    auto& game = GetGame();
    auto& input = GetInput();

    if (hasFocus)
    {
        if (input.IsMenuButtonPressed())
        {
            game.ShowMenu(MenuId::GameMenu);
        }
        else if (input.IsMenuDismissPressed())
        {
            game.DismissMenu();
        }

        //
        // TEST
        //
        XMVECTOR forward = XMVector3Normalize(XMVectorSubtract(XMLoadFloat3(&_cameraTarget), XMLoadFloat3(&_cameraPosition)));
        XMVECTOR right = XMVector3Cross(XMVectorSet(0, 1, 0, 0), forward);
        XMVECTOR up = XMVector3Cross(forward, right);

        static const float LookSpeed = 0.01f;
        static const float MoveSpeed = 0.1f;

        XMFLOAT2 look = input.GetLookVector();
        if (look.x != 0.0f || look.y != 0.0f)
        {
            XMVECTOR rotation = XMQuaternionMultiply(XMQuaternionRotationAxis(right, -look.y * LookSpeed), XMQuaternionRotationAxis(XMVectorSet(0, 1, 0, 0), look.x * LookSpeed));
            forward = XMVector3Rotate(forward, rotation);
            right = XMVector3Cross(XMVectorSet(0, 1, 0, 0), forward);
            up = XMVector3Cross(forward, right);

            XMMATRIX rot;
            rot.r[0] = right;
            rot.r[1] = up;
            rot.r[2] = forward;
            rot.r[3] = XMVectorSet(0, 0, 0, 1);

            XMStoreFloat3(&_cameraTarget, XMVectorAdd(XMLoadFloat3(&_cameraPosition), forward));
            XMStoreFloat4(&_cameraOrientation, XMQuaternionRotationMatrix(rot));

            _cameraDirty = true;
        }

        XMFLOAT2 move = input.GetMovementVector();
        if (move.x != 0.0f || move.y != 0.0f)
        {
            XMVECTOR translation = XMVectorAdd(XMVectorScale(forward, move.y * MoveSpeed), XMVectorScale(right, move.x * MoveSpeed));
            XMStoreFloat3(&_cameraPosition, XMVectorAdd(XMLoadFloat3(&_cameraPosition), translation));
            XMStoreFloat3(&_cameraTarget, XMVectorAdd(XMLoadFloat3(&_cameraTarget), translation));

            _cameraDirty = true;
        }
    }

    //
    // TEST
    //
    _gameObject->Update(deltaTime);
}

void GamePlay::Draw()
{
    auto& graphics = GetGraphics();

    if (_cameraDirty)
    {
        XMStoreFloat4x4(&_view, XMMatrixLookAtLH(XMLoadFloat3(&_cameraPosition), XMLoadFloat3(&_cameraTarget), XMVectorSet(0, 1, 0, 0)));
        _cameraDirty = false;
    }

    XMFLOAT4X4 cameraWorld;
    XMStoreFloat4x4(&cameraWorld, XMMatrixAffineTransformation(XMVectorSet(1, 1, 1, 1), XMVectorZero(), XMLoadFloat4(&_cameraOrientation), XMLoadFloat3(&_cameraPosition)));

#if defined(TEST_STATIC_LEVEL)
    _staticLevelData->Draw(cameraWorld, _view, _projection);
#endif

    //
    // TEST
    //
    if (_gameObject != nullptr)
    {
        graphics.SetMaterial(_gameObject->GetMaterial());

        graphics.Draw3D(_view, _projection, &_gameObject, 1);
    }
}
