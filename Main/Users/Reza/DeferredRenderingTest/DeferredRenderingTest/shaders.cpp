#include "precomp.h"
#include "shaders.h"

using namespace Microsoft::WRL;
using namespace Microsoft::WRL::Wrappers;

class CachedShaderByteCode : public RuntimeClass<RuntimeClassFlags<RuntimeClassType::ClassicCom>, ICachedShaderByteCode>
{
public:
    CachedShaderByteCode(std::unique_ptr<BYTE[]>& data, size_t size) : _size(size)
    {
        _data.swap(data);
    }

    virtual const void* GetData() const override { return _data.get(); }
    virtual size_t GetSize() const override { return _size; }

private:
    std::unique_ptr<BYTE[]> _data;
    size_t _size;
};

static std::unique_ptr<BYTE[]> LoadCompiledShader(const wchar_t* filename, DWORD* size)
{
    FileHandle file(CreateFile(filename, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr));
    if (!file.IsValid())
    {
        throw std::exception();
    }

    *size = GetFileSize(file.Get(), nullptr);

    std::unique_ptr<BYTE[]> buffer(new BYTE[*size]);

    DWORD bytesRead = 0;
    if (!ReadFile(file.Get(), buffer.get(), *size, &bytesRead, nullptr) || bytesRead != *size)
    {
        throw std::exception();
    }

    return buffer;
}

ComPtr<ID3D11VertexShader> LoadVertexShader(ID3D11Device* device, const wchar_t* filename)
{
    DWORD size = 0;
    auto buffer = LoadCompiledShader(filename, &size);

    ComPtr<ID3D11VertexShader> shader;
    if (FAILED(device->CreateVertexShader(buffer.get(), static_cast<size_t>(size), nullptr, &shader)))
    {
        throw std::exception();
    }

    // Store the compiled shader code with the shader object.
    auto code = Make<CachedShaderByteCode>(buffer, static_cast<size_t>(size));
    ComPtr<IUnknown> codeUnk(code);
    if (FAILED(shader->SetPrivateDataInterface(__uuidof(ICachedShaderByteCode), codeUnk.Get())))
    {
        throw std::exception();
    }

    return shader;
}

ComPtr<ID3D11PixelShader> LoadPixelShader(ID3D11Device* device, const wchar_t* filename)
{
    DWORD size = 0;
    auto buffer = LoadCompiledShader(filename, &size);

    ComPtr<ID3D11PixelShader> shader;
    if (FAILED(device->CreatePixelShader(buffer.get(), static_cast<size_t>(size), nullptr, &shader)))
    {
        throw std::exception();
    }

    return shader;
}
