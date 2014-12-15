#include "Precomp.h"
#include "Transform.h"
#include "Screen.h"
#include "Input.h"
#include "AITestScreen.h"
#include "AssetLoading.h"
#include "GraphicsScene.h"
#include "GraphicsSystem.h"
#include "AudioSystem.h"
#include "Texture.h"
#include "Geometry.h"
#include "light.h"
#include "SpriteFont.h"
#include "Geometry.h"
#include "Model.h"
#include "Contexts.h"
#include "Bsp.h"
#include "ScreenSystem.h"
#include "MainMenuScreen.h"

#include <DirectXCollision.h>
#include <DirectXColors.h>

#include "Game\Entity.h"
#include "Game\Weapon.h"
#include "Game\Behaviors.h"
#include "Game\Doors.h"
#include "Game\GameWorld.h"
#include "Game\DebugDrawing.h"
#include "Game\Inputhelper.h"

const wchar_t* AIScene = L"w3d_map1.txt";

_Use_decl_annotations_
std::shared_ptr<AITestScreen> AITestScreen::Create(const std::shared_ptr<IGraphicsSystem>& graphics, const std::shared_ptr<IAudioSystem>& audio)
{
    return std::shared_ptr<AITestScreen>(new AITestScreen(graphics, audio));
}

_Use_decl_annotations_
AITestScreen::AITestScreen(const std::shared_ptr<IGraphicsSystem>& graphics, const std::shared_ptr<IAudioSystem>& audio)
{
    auto config = graphics->GetConfig();

    _worldLoadTimer = 0.0f;

    _frameCounterElapsedTime = 0.0f;
    _frames = 0;

    // Load test sound
    auto assetLoader = AssetLoader::Create();
    _sound = assetLoader->LoadSoundFromFile(audio, L"pc_up.wav");

    uint8_t tpixel[] = { 0, 0, 31, 100 };
    _transparent = graphics->CreateTexture(1, 1, DXGI_FORMAT_R8G8B8A8_UNORM, tpixel, sizeof(tpixel), false);

    // Load sprite fonts
    _spriteFont18pt = assetLoader->LoadSpriteFontFromFile(graphics, L"font18.spritefont");
    _spriteFont14pt = assetLoader->LoadSpriteFontFromFile(graphics, L"font14.spritefont");
    _spriteFont36pt = assetLoader->LoadSpriteFontFromFile(graphics, L"menufont.36pt.spritefont");
    _spriteFont48pt = assetLoader->LoadSpriteFontFromFile(graphics, L"menufont.48pt.spritefont");

    _crosshair = assetLoader->LoadTextureFromFile(graphics, L"crosshairs.png", false, nullptr);

    // Place camera high enough to observe the behaviors
    _observeCamera.SetPosition(Vector3(453,4,200));
    _observeCamera.SetScale(Vector3(1,1,1));
    _useObserveCamera = false;
    _zoomLevel = 40;

    _drawTargetStats = false;

    _drawHud = true;

    _projection = Matrix::CreatePerspectiveFieldOfView(XMConvertToRadians(60), config.Width / static_cast<float>(config.Height), 0.1f, 10000.0f);
}

bool AITestScreen::SupportsFocus() const
{
    return true;
}

_Use_decl_annotations_
void AITestScreen::Update(const UpdateContext& context, bool hasFocus)
{
    UNREFERENCED_PARAMETER(hasFocus);

    if (DismissButtonPressed(context.Input))
    {
        context.ScreenSystem->PopScreen();
        context.ScreenSystem->PushScreen(MainMenuScreen::Create(context.GraphicsSystem));
        return;
    }

    // Do low tech frame counter
    _frames++;
    _frameCounterElapsedTime += context.ElapsedTime;

    if (_frameCounterElapsedTime >= 1.0f)
    {
        _fps = _frames;
        _frames = 0;
        _frameCounterElapsedTime = 0.0f;
    }

    if (_world == nullptr)
    {
        _worldLoadTimer += context.ElapsedTime;
    }

    if(_worldLoadTimer >= 1.0f && _world == nullptr)
    {
        //CreateDebugScene(context.GraphicsSystem);

        // Load world
        _world = GameWorld::Create();
        _world->LoadScene(context, AIScene, true /* compile BSP */);
        _world->LoadEntities(context, AIScene);

        _world->ToggleEntityRendering(_world->GetPlayerControlledEntity(), _useObserveCamera);
        _observeEntity = _world->GetPlayerControlledEntity();
    }

    if (_world)
    {
        Transform controlledTransform = _observeEntity->GetTransform();
        Vector3 cameraEyePos = controlledTransform.GetPositionV();
        cameraEyePos.y = 4.0f;
        Vector3 followPos = controlledTransform.GetPositionV();
        followPos.y += _zoomLevel;
        if (_useObserveCamera)
        {
            _observeCamera.SetPosition(followPos);
            _observeCamera.SetScale(Vector3(1,1,1));
            _observeCamera.LookAt(controlledTransform.GetPositionV(), Vector3(1,0,0));
        }
        else
        {
            _observeCamera = controlledTransform;
            _observeCamera.SetPosition(cameraEyePos);
        }

        if (hasFocus)
        {
            _view = Matrix::CreateLookAt(_observeCamera.GetPositionV(), XMVectorAdd(_observeCamera.GetPositionV(), _observeCamera.GetForwardV()), _observeCamera.GetUpV());
        }

        if (context.Input->IsKeyDown('Z'))
        {
            _zoomLevel += 1.0f;
        }

        if (context.Input->IsKeyDown('X'))
        {
            _zoomLevel -= 1.0f;
        }

        if (context.Input->IsKeyJustPressed('C'))
        {
            _useObserveCamera = !_useObserveCamera;
            _world->ToggleEntityRendering(_world->GetPlayerControlledEntity(), _useObserveCamera);
        }

        if (context.Input->IsKeyJustPressed('T'))
        {
            _drawTargetStats = !_drawTargetStats;
        }

        if (context.Input->IsKeyJustPressed('H'))
        {
            _drawHud = !_drawHud;
        }

        if (context.Input->IsKeyJustPressed(VK_F1))
        {
            _observeEntity = _world->GetPlayerControlledEntity();
        }

        if (context.Input->IsKeyJustPressed('N'))
        {
            _observeEntity = _world->GetNextEntity(_observeEntity, EntityClass::Soldier);
        }

        _world->Update(context);
    }
}

_Use_decl_annotations_
void AITestScreen::Draw(const DrawContext& context)
{
    if (_world)
    {
        auto config = context.GraphicsSystem->GetConfig();
        //context.GraphicsSystem->DrawScene(_debugScene, _view, _projection);
        context.GraphicsSystem->DrawScene(_world->GetScene(), _view, _projection);

        if (_drawHud)
        {
            DrawHud(context);
        }
        else
        {
            DrawStatsPanel(context);
        }

        context.GraphicsSystem->DrawImage(_crosshair, config.Width/2 - (_crosshair->GetWidth()/2), config.Height/2 - (_crosshair->GetHeight()/2));
    }
    else
    {
        auto config = context.GraphicsSystem->GetConfig();

        const wchar_t* st = L"Loading...";
        _spriteFont48pt->Draw(context.GraphicsSystem, st, 
            (config.Width/2)-_spriteFont48pt->GetStringLength(st)/2,config.Height/2 - _spriteFont48pt->GetLineSpacing()/2);
    }
}

_Use_decl_annotations_
void AITestScreen::DrawHud(const DrawContext& context)
{
    auto& config = context.GraphicsSystem->GetConfig();
    uint32_t yOffset = config.Height - (_spriteFont48pt->GetLineSpacing() + _spriteFont18pt->GetLineSpacing());
    uint32_t xOffset = 0;
    std::shared_ptr<Entity> playerEntity = _world->GetPlayerControlledEntity();
    uint32_t headerOffset = _spriteFont18pt->GetLineSpacing();

    DrawDebugText(context.GraphicsSystem, _spriteFont18pt, xOffset,yOffset,"Score");
    DrawDebugText(context.GraphicsSystem, _spriteFont48pt, xOffset,yOffset + headerOffset,"%d",playerEntity->GetScore());

    xOffset = (config.Width/2) - (_spriteFont48pt->GetStringLength(L"XXXX")/2);

    DrawDebugText(context.GraphicsSystem, _spriteFont18pt, xOffset,yOffset,"Health");
    DrawDebugText(context.GraphicsSystem, _spriteFont48pt, xOffset,yOffset + headerOffset,"%d%%",playerEntity->GetHealth());

    xOffset = (config.Width) - (_spriteFont48pt->GetStringLength(L"XXXX"));

    DrawDebugText(context.GraphicsSystem, _spriteFont18pt, xOffset,yOffset,"Ammo");
    DrawDebugText(context.GraphicsSystem, _spriteFont48pt, xOffset,yOffset + headerOffset,"%d",playerEntity->GetWeapon()->GetAmmoCount());

    DrawDebugText(context.GraphicsSystem, _spriteFont14pt, 0,0,"fps:%d", _fps);
}

_Use_decl_annotations_
void AITestScreen::DrawStatsPanel(const DrawContext& context)
{
    auto& config = context.GraphicsSystem->GetConfig();

    uint32_t yOffset = 0;
    uint32_t paneWidth = config.Width - 20;
    uint32_t panelHeight = 100;
    std::shared_ptr<Entity> playerEntity = _world->GetPlayerControlledEntity();

    RECT r = { config.Width - (paneWidth + 10), config.Height - (panelHeight + 10), paneWidth, panelHeight };
    DrawPanelRect(context, r);

    if (_useObserveCamera)
    {
        DrawDebugText(context.GraphicsSystem, _spriteFont14pt, config.Width - 280, r.top + yOffset, "fps: %d\npress escape to quit\npress z/x to zoom\npress c to return to player camera", _fps);
    }
    else
    {
        DrawDebugText(context.GraphicsSystem, _spriteFont14pt, config.Width - 280, r.top + yOffset, "fps: %d\npress escape to quit\npress c toggle follow camera", _fps);
    }

    // Draw player position
    Vector3 playerPos = playerEntity->GetTransform().GetPositionV();
    float playerSpeed = playerEntity->GetSpeed(SpeedType::MovementSpeed);
    DrawDebugText(context.GraphicsSystem, _spriteFont14pt, r.left, r.top + yOffset, "pos: %f,%f,%f speed: %f", playerPos.x, playerPos.y, playerPos.z, playerSpeed);

    // Draw targets based on visible entities
    if (_drawTargetStats)
    {
        std::vector<std::shared_ptr<Entity>> visibleEntities = _world->GetVisibleEntities(_world->GetPlayerControlledEntity());
        auto targetEntity = _world->GetEntityAsTarget(nullptr, _world->GetPlayerControlledEntity(), EntityClass::Any);
        yOffset = 0;
        bool targetEntityFound = false;
        for (auto entity : visibleEntities)
        {
            DrawEntityStats(context, entity, 0, yOffset, entity == targetEntity);
            yOffset += _spriteFont18pt->GetLineSpacing() + 4;
            if (!targetEntityFound)
            {
                targetEntityFound = (entity == targetEntity);
            }
        }

        if (!targetEntityFound)
        {
            DrawEntityStats(context, targetEntity, 0, yOffset, true);
        }
    }
}

_Use_decl_annotations_
void AITestScreen::DrawPanelRect(const DrawContext& context, const RECT& rect)
{
    context.GraphicsSystem->DrawImage(_transparent, rect.left, rect.top, rect.right, rect.bottom);
}

_Use_decl_annotations_
void AITestScreen::DrawEntityStats(const DrawContext& context, std::shared_ptr<Entity> entity, uint32_t x, uint32_t y, bool isTarget)
{
    if (entity == nullptr)
    {
        return;
    }

    uint32_t xOffset = x;
    std::string entityClass = "unknown";
    std::shared_ptr<ITexture> pic;
    switch(entity->GetClass())
    {
    case EntityClass::Player:
        entityClass = "player";
        break;
    case EntityClass::Soldier:
        entityClass = "soldier";
        break;
    case EntityClass::SS_Soldier:
        entityClass = "SS soldier";
        break;
    case EntityClass::Dog:
        entityClass = "dog";
        break;
    case EntityClass::Door:
        entityClass = "door";
        break;
    case EntityClass::Ammo:
        entityClass = "ammo";
        break;
    case EntityClass::Health:
        entityClass = "health";
        break;
    case EntityClass::PushWall:
        entityClass = "pushwall";
        break;
    default:
        break;
    }

    DrawDebugText(context.GraphicsSystem, _spriteFont18pt, xOffset, y + 4, "(%s) %d%%, (%3.1f,%3.1f,%3.1f) %s", entityClass.c_str(), entity->GetHealth(), entity->GetTransform().GetPosition().x, entity->GetTransform().GetPosition().y, entity->GetTransform().GetPosition().z, isTarget ? "<--target" : "");
}

_Use_decl_annotations_
void AITestScreen::CreateDebugScene(const std::shared_ptr<IGraphicsSystem>& graphics)
{
    std::shared_ptr<ILight> light1 = graphics->CreateLight(LightType::Directional, Vector3(1,1,1), 0.0f);
    std::shared_ptr<ILight> light2 = graphics->CreateLight(LightType::Directional, Vector3(1,1,1), 0.0f);
    std::shared_ptr<ILight> light3 = graphics->CreateLight(LightType::Directional, Vector3(1,1,1), 0.0f);

    light1->SetPosition(Vector3(-1.0f, -1.0f, 0.5f));
    light2->SetPosition(Vector3(1.0f, -1.0f, 0.5f));
    light3->SetPosition(Vector3(0.0f, -1.0f, -0.5f));

    _debugScene = graphics->CreateScene();
    _debugScene->AddLight(light1);
    _debugScene->AddLight(light2);
    _debugScene->AddLight(light3);
}

_Use_decl_annotations_
void AITestScreen::AddBoundingBoxModelForEntity(const std::shared_ptr<IGraphicsSystem>& graphics, const std::shared_ptr<Entity>& entity)
{
    UNREFERENCED_PARAMETER(graphics);
    UNREFERENCED_PARAMETER(entity);
#if 0 // Move to using debug renderer when that comes online
    auto assetLoader = AssetLoader::Create();
    if (_bbGeometry == nullptr)
    {
        _bbGeometry = assetLoader->LoadGeometryFromOBJFile(graphics, L"wireframe_cube.obj", Transform());
    }

    if (_bbTexture == nullptr)
    {
        _bbTexture =  graphics->CreateTexture(1, 1, DXGI_FORMAT_R32G32B32A32_FLOAT, Colors::White, sizeof(Colors::White), false);
    }

    std::shared_ptr<IModel> bbModel;
    Transform bbTransform;

    bbTransform.SetPosition(entity->GetBoundingBox().Center);
    bbTransform.SetScale(entity->GetBoundingBox().Extents);

    bbModel = graphics->CreateModel();
    bbModel->SetGeometry(_bbGeometry);
    bbModel->SetTexture(TextureUsage::Diffuse, _bbTexture);
    bbModel->SetTransform(bbTransform);

    if (_debugScene != nullptr)
    {
        _debugScene->AddModel(bbModel);
    }
    else
    {
        _world->GetScene()->AddModel(bbModel);
    }
    _debugBoundingBoxModels[entity] = bbModel;
#endif
}
