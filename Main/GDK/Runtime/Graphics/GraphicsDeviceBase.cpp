#include "GraphicsDeviceBase.h"
#include "Effect.h"
#include "RuntimeGeometry.h"
#include "RuntimeTexture.h"

#include "DirectX\DxGraphicsDevice.h"

namespace GDK
{
    std::shared_ptr<GraphicsDeviceBase> GraphicsDeviceBase::Create(_In_ const GraphicsDevice::CreateParameters& parameters)
    {
        std::shared_ptr<GraphicsDeviceBase> graphicsDevice;

        switch (parameters.type)
        {
#ifdef WIN32
        case GraphicsDevice::Type::DirectX:
            graphicsDevice = DxGraphicsDevice::Create(parameters);
            break;
#endif

        default:
            throw std::exception();
        }

        graphicsDevice->FinishInitialization();
        return graphicsDevice;
    }

    GraphicsDeviceBase::GraphicsDeviceBase(_In_ const GraphicsDevice::CreateParameters& parameters) :
        GraphicsDevice(parameters),
        _viewArea(static_cast<float>(parameters.backBufferWidth), static_cast<float>(parameters.backBufferHeight))
    {
        _workArea = _viewArea;
    }

    GraphicsDeviceBase::~GraphicsDeviceBase()
    {
    }

    void GraphicsDeviceBase::ClearBoundResources()
    {
        BindGeometry(nullptr);

        for (uint32_t i = 0; i < _countof(_boundTextures); ++i)
        {
            BindTexture(i, nullptr);
        }
    }

    void GraphicsDeviceBase::BindTexture(_In_ uint32_t slot, _In_ const std::shared_ptr<Texture>& texture)
    {
        if (_boundTextures[slot] != nullptr)
        {
            static_cast<RuntimeTexture*>(_boundTextures[slot].get())->Unbind(slot);
        }

        if (texture != nullptr)
        {
            _boundTextures[slot] = texture;
        }
        else
        {
            _boundTextures[slot] = _defaultTexture;
        }

        if (_boundTextures[slot] != nullptr)
        {
            static_cast<RuntimeTexture*>(_boundTextures[slot].get())->Bind(slot);
        }
    }

    void GraphicsDeviceBase::BindGeometry(_In_ const std::shared_ptr<Geometry>& geometry)
    {
        if (_boundGeometry != nullptr)
        {
            static_cast<RuntimeGeometry*>(_boundGeometry.get())->Unbind();
        }

        _boundGeometry = geometry;

        if (_boundGeometry != nullptr)
        {
            static_cast<RuntimeGeometry*>(_boundGeometry.get())->Bind();
        }
    }

    void GraphicsDeviceBase::SetViewProjection(_In_ const Matrix& view, _In_ const Matrix& projection)
    {
        _view = view;
        _projection = projection;
    }

    void GraphicsDeviceBase::Draw(_In_ const Matrix& world, _In_ uint32_t frame)
    {
        if (!_boundGeometry)
        {
            return;
        }

        if (_currentEffect != _static3DEffect)
        {
            _currentEffect = _static3DEffect;
            _currentEffect->Apply();
        }

        _currentEffect->Draw(_boundGeometry, world, frame);
    }

    void GraphicsDeviceBase::Set2DWorkArea(_In_ uint32_t width, _In_ uint32_t height)
    {
        _workArea = Vector2(static_cast<float>(width), static_cast<float>(height));
    }

    void GraphicsDeviceBase::Draw2D(_In_ const std::shared_ptr<Texture>& texture, _In_ int32_t x, _In_ int32_t y)
    {
        if (_currentEffect != _draw2DEffect)
        {
            _currentEffect = _draw2DEffect;
            _currentEffect->Apply();
        }

        auto previousBoundGeometry = _boundGeometry;
        auto previousBoundTexture = _boundTextures[0];

        BindGeometry(_quad2DGeometry);
        BindTexture(0, texture);

        int32_t width = texture->GetWidth();
        int32_t height = texture->GetHeight();
        _quadVertices[0].position = Vector2(x, y);
        _quadVertices[1].position = Vector2(x + width, y);
        _quadVertices[2].position = Vector2(x + width, y + height);
        _quadVertices[3].position = Vector2(x, y + height);

        static_cast<RuntimeGeometry*>(_quad2DGeometry.get())->Update(_quadVertices, _countof(_quadVertices) * sizeof(_quadVertices[0]));
        _currentEffect->Draw(_quad2DGeometry, Matrix::Identity(), 0);

        // re-bind previous geometry and texture if there was one
        if (previousBoundGeometry)
        {
            BindGeometry(previousBoundGeometry);
        }
        if (previousBoundTexture)
        {
            BindTexture(0, previousBoundTexture);
        }
    }

    const Matrix& GraphicsDeviceBase::GetViewMatrix() const
    {
        return _view;
    }

    const Matrix& GraphicsDeviceBase::GetProjectionMatrix() const
    {
        return _projection;
    }

    const Vector2& GraphicsDeviceBase::GetWorkArea() const
    {
        return _workArea;
    }

    const Vector2& GraphicsDeviceBase::GetViewArea() const
    {
        return _viewArea;
    }

    void GraphicsDeviceBase::FinishInitialization()
    {
        static const byte_t defaultTextureColor[] = { 160, 160, 160, 255 };
        static const uint32_t quadIndices[] = { 0, 1, 2, 2, 3, 0 };

        InitializeDevice();

        _defaultTexture = CreateDefaultTexture(1, 1, TextureFormat::R8G8B8A8, defaultTextureColor);

        _quadVertices[0] = Geometry::Vertex2D(Vector2(0, 0), Vector2(0, 0));
        _quadVertices[1] = Geometry::Vertex2D(Vector2(1, 0), Vector2(1, 0));
        _quadVertices[2] = Geometry::Vertex2D(Vector2(1, 1), Vector2(1, 1));
        _quadVertices[3] = Geometry::Vertex2D(Vector2(0, 1), Vector2(0, 1));

        _quad2DGeometry = CreateQuad2D(_countof(_quadVertices), _quadVertices, _countof(quadIndices), quadIndices);

        _static3DEffect = Effect::Create(shared_from_this(), Effect::Type::StaticGeometry3D);
        _draw2DEffect = Effect::Create(shared_from_this(), Effect::Type::Draw2D);

        _currentEffect = _static3DEffect;
        _currentEffect->Apply();
    }
}
