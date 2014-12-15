#include "precomp.h"

void LoadGeometryFileDataFromOBJ(_In_z_ const char* path, _Out_ GeometryFileData* fileData);

void LoadGeometryFileData(_In_z_ const char* path, _Out_ GeometryFileData* fileData)
{
    std::string p = path;
    std::transform(p.begin(), p.end(), p.begin(), ::tolower);

    if (p.rfind(".obj"))
    {
        LoadGeometryFileDataFromOBJ(path, fileData);
    }
    else
    {
        throw std::exception("Unsupported model/geometry file format");
    }
}

void FreeGeometryFileData(_In_ GeometryFileData* fileData)
{
    if (fileData)
    {
        if (fileData->MaterialTextures && fileData->NumMaterials > 0)
        {
            for (uint32_t i = 0; i < fileData->NumMaterials; ++i)
            {
                delete[] fileData->MaterialTextures[i];
            }

            delete [] fileData->MaterialTextures;
        }

        if (fileData->Vertices)
        {
            delete[] fileData->Vertices;
        }

        if (fileData->Indices)
        {
            delete[] fileData->Indices;
        }
    }
}