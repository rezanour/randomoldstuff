#include "Precomp.h"

using namespace VRES;

//=======================================================

class VRESApp : public Application
{
public:
    VRESApp();

    bool Initialize();
    bool Tick(float elapsedSeconds) override;

private:
    bool CreateTestScene();
    void HandleMovement(float elapsedSeconds);

private:
    std::shared_ptr<HMD> _hmd;
    std::unique_ptr<Renderer> _renderer;
    std::shared_ptr<Scene> _scene;
    std::shared_ptr<Model> _model;
    Transform _camera;

    // Rotating light
    std::shared_ptr<Light> _rotatingLight;

    // Head-mounted light
    std::shared_ptr<Light> _headLight;

    // Normal map management
    static const wchar_t* s_NormalMaps[];
    uint32_t _currentNormalMap;
};

//=======================================================

const wchar_t* VRESApp::s_NormalMaps[] = 
{
    nullptr,
    L"circle_grid.png",
    L"domes_normals.jpg",
    L"normal_4.png",
    L"normal3.jpg",
    L"normalmap.png",
    L"stone_normals.jpg"
};

VRESApp::VRESApp() :
    Application(L"VRESApp"), _currentNormalMap(0)
{
}

bool VRESApp::Initialize()
{
//#define ENABLE_VR

#if defined(ENABLE_VR)
    _hmd = HMD::Create(true /* Render to headset */);
#else
    _hmd = HMD::Create(false /* Render to headset */);
#endif
    if (!_hmd)
    {
        return false;
    }

    Point location;
    Size size;
    _hmd->GetWindowDimensions(&location, &size);

#if !defined(ENABLE_VR)
    _hmd->EnableHeadtracking(false);
    location = Point(100, 100);
#endif

    if (!CreateAppWindow(location, size))
    {
        return false;
    }

    RendererConfig config = {};
    config.Window = Window();
    config.Resolution = size;
    config.CreateDebugDevice = false;

    _renderer = Renderer::Create(config);
    if (!_renderer)
    {
        return false;
    }

    if (!_hmd->Bind(_renderer.get()))
    {
        return false;
    }

    return CreateTestScene();
}

bool VRESApp::CreateTestScene()
{
    _scene = Scene::Create();
    if (!_scene)
    {
        return false;
    }

    _model = _renderer->CreateCube(XMFLOAT3(0.5, 0.5, 0.5));
    if (!_model)
    {
        return false;
    }

    _model->SetMaterial(_renderer->GetMaterial("diffuse"));

    ComPtr<ID3D11ShaderResourceView> texture = _renderer->LoadTexture(L"image.jpg");
    _model->SetTexture(TextureType::Diffuse, texture);

    _scene->Insert(_model);

    _rotatingLight.reset(new PointLight(XMFLOAT3(1.0f, 1.0f, 1.0f), 2.0f));
    _rotatingLight->Transform().SetPosition(XMFLOAT3(0, 0, -1.0f));
    _scene->Insert(_rotatingLight);

    _headLight.reset(new PointLight(XMFLOAT3(1.0f, 1.0f, 1.0f), 2.0f));
    //_scene->Insert(_headLight);

    _camera.SetPosition(XMFLOAT3(0.0f, 0.0f, -2.0f));

    return true;
}

bool VRESApp::Tick(float elapsedSeconds)
{
    static short prevSpaceKeyState = 0;

    if (GetKeyState(VK_ESCAPE) & 0x8000)
    {
        // Quit
        return false;
    }

    short keyState = GetKeyState(VK_SPACE);
    if ((keyState & 0x8000) && ((prevSpaceKeyState & 0x8000) == 0))
    {
        _currentNormalMap = (_currentNormalMap + 1) % _countof(s_NormalMaps);
        ComPtr<ID3D11ShaderResourceView> normalmap = _renderer->LoadTexture(s_NormalMaps[_currentNormalMap]);
        _model->SetTexture(TextureType::Normal, normalmap);
    }
    prevSpaceKeyState = keyState;

    //
    // Handle rotating light
    //
    static const float RotatingSpeed = 1.0f;
    XMMATRIX rotation = XMMatrixRotationY(RotatingSpeed * elapsedSeconds);
    XMVECTOR pos = XMLoadFloat3(&_rotatingLight->Transform().Position());
    pos = XMVector3Transform(pos, rotation);
    XMFLOAT3 position;
    XMStoreFloat3(&position, pos);
    _rotatingLight->Transform().SetPosition(position);

    //
    // Handle input
    //
    HandleMovement(elapsedSeconds);

    //
    // Sync head light to head
    //
    position = _camera.Position();
    position.y += 1.0f;
    _headLight->Transform().SetPosition(position);

    _hmd->BeginFrame();

    _renderer->RenderScene(_scene, _camera, _hmd);

    _hmd->EndFrame();
    return true;
}

void VRESApp::HandleMovement(float elapsedSeconds)
{
    static const float MoveSpeed = 1.0f;
    static const float TurnSpeed = 1.0f;

    float forward = 0.0f;
    float right = 0.0f;
    float turn = 0.0f;

    if (GetKeyState('W') & 0x8000)
    {
        forward += MoveSpeed;
    }
    if (GetKeyState('A') & 0x8000)
    {
        right -= MoveSpeed;
    }
    if (GetKeyState('S') & 0x8000)
    {
        forward -= MoveSpeed;
    }
    if (GetKeyState('D') & 0x8000)
    {
        right += MoveSpeed;
    }

    if (GetKeyState(VK_LEFT) & 0x8000)
    {
        turn -= TurnSpeed;
    }
    if (GetKeyState(VK_RIGHT) & 0x8000)
    {
        turn += TurnSpeed;
    }

    XMMATRIX torsoRotation = XMMatrixRotationQuaternion(XMLoadFloat4(&_camera.Orientation())) * XMMatrixRotationY(turn * elapsedSeconds);
    torsoRotation.r[1] = XMVectorSet(0, 1, 0, 0);
    torsoRotation.r[0] = XMVector3Cross(torsoRotation.r[1], torsoRotation.r[2]);
    torsoRotation.r[2] = XMVector3Cross(torsoRotation.r[0], torsoRotation.r[1]);
    XMFLOAT4 orientation;
    XMStoreFloat4(&orientation, XMQuaternionRotationMatrix(torsoRotation));
    _camera.SetOrientation(orientation);

    float length = forward * forward + right * right;
    if (length > 0.0001f)
    {
        length = sqrt(length);
        forward = forward * elapsedSeconds / length;
        right = right * elapsedSeconds / length;
    }

    XMMATRIX headRotation = XMMatrixIdentity();
    if (_hmd->HeadtrackingEnabled())
    {
        XMFLOAT4 o = _hmd->Orientation();
        headRotation = XMMatrixRotationQuaternion(XMLoadFloat4(&o));
    }

    XMVECTOR f = XMVector3TransformNormal(XMLoadFloat3(&_camera.Forward()), headRotation);
    // Orthonormalize against straight up vector (no tilt or roll during movement)
    XMVECTOR u = XMVectorSet(0, 1, 0, 0);
    XMVECTOR r = XMVector3Cross(u, f);
    f = XMVector3Cross(r, u);
    XMVECTOR move = f * forward + r * right;
    XMFLOAT3 position;
    XMStoreFloat3(&position, XMLoadFloat3(&_camera.Position()) + move);
    _camera.SetPosition(position);
}

//=======================================================

_Use_decl_annotations_
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    if (!VRES::System::Initialize())
    {
        return -1;
    }

    {
        std::unique_ptr<VRESApp> app(new VRESApp());

        if (app->Initialize())
        {
            app->Run();
        }
    }

    VRES::System::Shutdown();

    return 0;
}
