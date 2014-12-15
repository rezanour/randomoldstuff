#include <Precomp.h>
#include "Screen.h"
#include "GraphicsTestScreen.h"
#include "MainMenuScreen.h"
#include "AssetLoading.h"
#include "GraphicsSystem.h"
#include "GraphicsScene.h"
#include "Geometry.h"
#include "Model.h"
#include "Light.h"
#include "Texture.h"
#include "Contexts.h"
#include "Input.h"
#include "ScreenSystem.h"
#include "Game\Inputhelper.h"

#define RAND_BETWEEN(mn, mx) (rand() % ((mx)-(mn)) + (mn))

_Use_decl_annotations_
std::shared_ptr<GraphicsTestScreen> GraphicsTestScreen::Create(const std::shared_ptr<IGraphicsSystem>& graphics)
{
    return std::shared_ptr<GraphicsTestScreen>(new GraphicsTestScreen(graphics));
}

_Use_decl_annotations_
GraphicsTestScreen::GraphicsTestScreen(const std::shared_ptr<IGraphicsSystem>& graphics)
{
    auto assetLoader = AssetLoader::Create();

    //_scene = assetLoader->LoadSceneFromFile(graphics, L"w3d_map1.txt");
    //_scene = assetLoader->LoadSceneFromFile(graphics, L"scene1.txt");
    _scene = assetLoader->LoadSceneFromFile(graphics, L"lighting_test.txt");

    auto& config = graphics->GetConfig();

    //_camera = Transform(XMFLOAT3(455.0f, 3.5f, 230.0f), XMFLOAT4(0, 0, 0, 1), XMFLOAT3(1, 1, 1));
    //_camera = Transform(XMFLOAT3(0.0f, 0.25f, 0.0f), XMFLOAT4(0, 0, 0, 1), XMFLOAT3(1, 1, 1));
    _camera = Transform(XMFLOAT3(0.0f, 4.0f, 0.0f), XMFLOAT4(0, 0, 0, 1), XMFLOAT3(1, 1, 1));

    XMStoreFloat4x4(&_view, XMMatrixLookAtLH(_camera.GetPositionV(), XMVectorAdd(_camera.GetPositionV(), _camera.GetForwardV()), _camera.GetUpV()));

    XMStoreFloat4x4(&_projection, XMMatrixPerspectiveFovLH(XMConvertToRadians(60), config.Width / static_cast<float>(config.Height), 0.1f, 10000.0f));

#if 0
    for (uint32_t i = 0; i < 1000; ++i)
    {
        static const int32_t area = 30;

        auto light = graphics->CreateLight(LightType::Point, XMFLOAT3(RAND_BETWEEN(0, 256) / 256.0f, RAND_BETWEEN(0, 256) / 256.0f, RAND_BETWEEN(0, 256) / 256.0f), (float)RAND_BETWEEN(1, 3));
        light->SetPosition(XMFLOAT3((float)RAND_BETWEEN(-area, area), (float)RAND_BETWEEN(-1, 1), (float)RAND_BETWEEN(-area, area)));
        _scene->AddLight(light);
    }
#endif
}

bool GraphicsTestScreen::SupportsFocus() const
{
    return true;
}

_Use_decl_annotations_
void GraphicsTestScreen::Update(const UpdateContext& context, bool hasFocus)
{
    if (hasFocus)
    {
        if (DismissButtonPressed(context.Input))
        {
            context.ScreenSystem->PopScreen();
            context.ScreenSystem->PushScreen(MainMenuScreen::Create(context.GraphicsSystem));
            return;
        }

        static const float CameraSpeed = 0.4f;
        static const float CameraRotationSpeed = 0.03f;

        XMVECTOR right = _camera.GetRightV();
        XMVECTOR forward = XMVector3Normalize(XMVector3Cross(right, XMVectorSet(0, 1, 0, 0)));

        // Movement
        XMVECTOR movement = XMVectorZero();
        if (context.Input->IsKeyDown('W'))
        {
            movement = XMVectorAdd(movement, forward);
        }
        if (context.Input->IsKeyDown('A'))
        {
            movement = XMVectorAdd(movement, XMVectorNegate(right));
        }
        if (context.Input->IsKeyDown('S'))
        {
            movement = XMVectorAdd(movement, XMVectorNegate(forward));
        }
        if (context.Input->IsKeyDown('D'))
        {
            movement = XMVectorAdd(movement, right);
        }

        _camera.Move(XMVectorScale(XMVector3Normalize(movement), CameraSpeed));

        // Rotation
        float yaw = 0.0f;
        float pitch = 0.0f;
        if (context.Input->IsKeyDown(VK_UP))
        {
            pitch -= 1.0f;
        }
        if (context.Input->IsKeyDown(VK_DOWN))
        {
            pitch += 1.0f;
        }
        if (context.Input->IsKeyDown(VK_LEFT))
        {
            yaw -= 1.0f;
        }
        if (context.Input->IsKeyDown(VK_RIGHT))
        {
            yaw += 1.0f;
        }

        _camera.Rotate(_camera.GetRightV(), pitch * CameraRotationSpeed);
        _camera.Rotate(_camera.GetUpV(), yaw * CameraRotationSpeed);

        // fix up vector to avoid rolling
        _camera.AlignUp(XMVectorSet(0, 1, 0, 0));

        // Head tracking (if available)
        XMVECTOR headOrientation = context.GraphicsSystem->GetHeadOrientation();
        XMVECTOR bodyOrientation = _camera.GetOrientationV();
        XMVECTOR totalOrientation = XMQuaternionNormalize(XMQuaternionMultiply(headOrientation, bodyOrientation));

        // Compute viewCamera transform, but only update camera itself partially
        Transform viewCamera = _camera;
        viewCamera.SetOrientation(totalOrientation);
        XMStoreFloat4x4(&_view, XMMatrixLookToLH(viewCamera.GetPositionV(), viewCamera.GetForwardV(), viewCamera.GetUpV()));

        auto debug = _scene->GetDebug();

        debug->DrawBox(XMFLOAT3(460, 0, 230), XMFLOAT3(480, 10, 260), XMFLOAT4(0.25f, 0.0f, 0.0f, 0.25f));
    }
}

_Use_decl_annotations_
void GraphicsTestScreen::Draw(const DrawContext& context)
{
    context.GraphicsSystem->DrawScene(_scene, _view, _projection);
}
