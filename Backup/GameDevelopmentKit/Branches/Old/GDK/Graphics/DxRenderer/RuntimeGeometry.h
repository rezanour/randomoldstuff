#pragma once

namespace GDK {
namespace Graphics {

    class RuntimeGeometry : public RuntimeObject<IRuntimeGeometry>
    {
    public:
        struct Vertex
        {
            Vector3 Position;
            Vector3 Normal;
            Vector2 TexCoord;
        };

        static Microsoft::WRL::ComPtr<RuntimeGeometry> GDKAPI Create(_In_ DxGraphicsDevice* device, _In_ GDK::Content::IGeometryResource* resource);
        static Microsoft::WRL::ComPtr<RuntimeGeometry> GDKAPI CreateFromData(_In_ DxGraphicsDevice* device, _In_ size_t numFrames, _In_ size_t numVertices, _In_ const Vertex* vertices, _In_ size_t numIndices, _In_ const uint32_t* indices);
        RuntimeGeometry(_In_ const std::wstring& name);

        //
        // IRuntimeGeometry
        //
        virtual bool GDKAPI IsBound() const
        {
            return _isBound;
        }

        virtual const wchar_t* GDKAPI GetName() const
        {
            return _name.c_str();
        }

        virtual size_t GDKAPI GetNumFrames() const
        {
            return _numFrames;
        }

        virtual void GDKAPI SetCurrentFrame(_In_ size_t frame)
        {
            _currentFrame = (frame < _numFrames) ? frame : 0;
        }

        // Internal
        void Bind(_In_ DxGraphicsDevice* device);
        void Unbind(_In_ DxGraphicsDevice* device);

        void Draw(_In_ DxGraphicsDevice* device);

    private:
        bool _isBound;
        std::wstring _name;

        // Contains all frames back to back
        Microsoft::WRL::ComPtr<ID3D11Buffer> _vertices;

        size_t _numFrames;
        size_t _currentFrame;

        // Sample ib used for all frames
        Microsoft::WRL::ComPtr<ID3D11Buffer> _indices;

        // per frame
        uint32_t _numVertices;
        uint32_t _numIndices;
    };

} // Graphics
} // GDK
