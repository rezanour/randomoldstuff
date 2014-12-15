#include "StdAfx.h"
#include "BaseShader.h"

using namespace Lucid3D;

uint32 BaseShader::s_nextId = 0;

BaseShader::BaseShader()
    : _id(s_nextId++)
{
}

BaseShader::~BaseShader()
{
}

uint32 BaseShader::GetSupportedQuality(_In_ uint32 materialId) const
{
    QualityMap::const_iterator it = _supportedMaterials.find(materialId);
    if (it != _supportedMaterials.end())
    {
        return it->second;
    }

    return 0;
}

void BaseShader::AddSupportedMaterial(_In_ uint32 materialId, _In_ uint32 quality)
{
    _supportedMaterials[materialId] = quality;
}

void BaseShader::SetShaderCode(_In_ stde::com_ptr<ID3DBlob> spCode)
{
    _spCode = spCode;
}
