#include "precomp.h"

// Material Parsing (.MTL) Notes
// Spec Url http://people.sc.fsu.edu/~jburkardt/data/mtl/mtl.html
//          http://paulbourke.net/dataformats/mtl/
/*
Ka r g b 
    defines the ambient color of the material to be (r,g,b). The default is (0.2,0.2,0.2)
Kd r g b 
    defines the diffuse color of the material to be (r,g,b). The default is (0.8,0.8,0.8)
Ks r g b 
    defines the specular color of the material to be (r,g,b). This color shows up in highlights. The default is (1.0,1.0,1.0)
d alpha 
    defines the transparency of the material to be alpha. The default is 1.0 (not transparent at all) Some formats use Tr instead of d
Tr alpha 
    defines the transparency of the material to be alpha. The default is 1.0 (not transparent at all). Some formats use d instead of Tr
Ns s 
    defines the shininess of the material to be s. The default is 0.0
illum n 
    denotes the illumination model used by the material.
    illum = 1 indicates a flat material with no specular highlights, so the value of Ks is not used.
    illum = 2 denotes the presence of specular highlights, and so a specification for Ks is required.
map_Ka 
    filename names a file containing a texture map, which should just be an ASCII dump of RGB values
*/

// find next newline and replace with a null terminator. Return pointer to next char
static bool getline(_Inout_ char* p, _Out_ char** pNext)
{
    *pNext = p;
    while (**pNext != '\n' && **pNext != 0)
    {
        ++(*pNext);
    }

    if (**pNext == '\n')
    {
        // remove \r too if it's there
        if (*(*pNext - 1) == '\r')
        {
            *(*pNext - 1) = 0;
        }

        **pNext = 0;
        ++(*pNext);
    }

    // were there any characters past p?
    return (*pNext - p >= 1);
}

// efficient inplace split
static void split(_Inout_ char* p, _Out_writes_(*count) char** subs, _Inout_ uint32_t* count, _In_ const char delim)
{
    char* x = p;
    uint32_t num = 0;
    uint32_t max = *count;
    subs[num++] = p;
    while (num < max && *x != 0)
    {
        if (*x == delim)
        {
            *x = 0;
            subs[num++] = x + 1;
        }

        ++x;
    }
    *count = num;
}

static void trimstart(_Inout_ char* p, const char* end)
{
    while (*p != 0 && p < end && isspace(*p))
    {
        ++p;
    }
}

typedef struct
{
    char MaterialName[128];
    char DiffuseTexturePath[256];
    int MaterialId;
    int startVertexIndex;
    int numVertices;
}ObjMaterial;

void LoadMaterialFileDataFromOBJMTL(_In_z_ const char* path, _Out_ ObjMaterial* materialData, _In_ uint32_t numMaterials)
{
    FileHandle inputFile(CreateFileA(path, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr));
    if (!inputFile.IsValid())
    {
        Error("Failed to open file: %s", path);
    }

    DWORD size = GetFileSize(inputFile.Get(), nullptr);
    std::unique_ptr<char[]> buffer(new char[size]);

    DWORD read;
    CHECKTRUE(ReadFile(inputFile.Get(), buffer.get(), size, &read, nullptr));

    inputFile.Close();

    char* p = buffer.get();
    char* next = nullptr;

    int indexToMaterial = -1;

    while (getline(p, &next))
    {
        trimstart(p, buffer.get() + size);

        if (strncmp(p, "newmtl ", 7) == 0)
        {
            char materialName[128];
            sscanf_s(p, "%*s %s", materialName, (uint32_t)_countof(materialName));

            // reset index
            indexToMaterial = -1;

            for (uint32_t i = 0; i < numMaterials; i++)
            {
                if (_strcmpi(materialName, materialData[i].MaterialName) == 0)
                {
                    indexToMaterial = i;
                    break;
                }
            }

            if (indexToMaterial == -1)
            {
                Error("Material '%s' was not found in material map", materialName);
            }
        }
        else if (strncmp(p, "map_Kd ", 7) == 0)
        {
            sscanf_s(p, "%*s %s", materialData[indexToMaterial].DiffuseTexturePath, (uint32_t)_countof(materialData[indexToMaterial].DiffuseTexturePath));
        }

        p = next;
    }
}

void LoadGeometryFileDataFromOBJ(_In_z_ const char* path, _Out_ GeometryFileData* fileData)
{
    FileHandle inputFile(CreateFileA(path, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr));
    if (!inputFile.IsValid())
    {
        Error("Failed to open file: %s", path);
    }

    // TODO: Replace ObjVertex with common vertex structure
    typedef struct
    {
        Vector3 Position;
        Vector3 Normal;
        Vector2 TexCoord;
        uint32_t MaterialId;
    }ObjVertex;

    DWORD size = GetFileSize(inputFile.Get(), nullptr);
    std::unique_ptr<char[]> buffer(new char[size]);

    DWORD read;
    CHECKTRUE(ReadFile(inputFile.Get(), buffer.get(), size, &read, nullptr));

    inputFile.Close();

    std::vector<ObjVertex> vertices;
    std::vector<uint32_t> indices;

    char mtlLibPath[256];

    ObjMaterial materials[32];
    ZeroMemory(materials, sizeof(materials));
    int lastStartVertexIndex = 0;
    int materialId = -1;
    uint32_t numMaterials = 0;

    std::vector<Vector3> v;
    std::vector<Vector3> n;
    std::vector<Vector2> t;

    char* p = buffer.get();
    char* next = nullptr;

    while (getline(p, &next))
    {
        if (strncmp(p, "v ", 2) == 0)
        {
            Vector3 vertex;
            sscanf_s(p, "%*s %f %f %f", &vertex.x, &vertex.y, &vertex.z);
            v.push_back(vertex);
        }
        else if (strncmp(p, "vn ", 3) == 0)
        {
            Vector3 normal;
            sscanf_s(p, "%*s %f %f %f", &normal.x, &normal.y, &normal.z);
            n.push_back(normal);
        }
        else if (strncmp(p, "vt ", 3) == 0)
        {
            Vector2 texCoord;
            sscanf_s(p, "%*s %f %f", &texCoord.x, &texCoord.y);
            texCoord.x *= -1; texCoord.y *= -1;
            t.push_back(texCoord);
        }
        else if (strncmp(p, "mtllib ", 7) == 0)
        {
            // Do a quick reverse find for the last slash, so
            // a root path can be constructed for the material
            // file.
            const char* lastSlash = path + strlen(path);
            while(*lastSlash != '\\' && *lastSlash != '/' && lastSlash != path)
            {
                lastSlash--;
            }

            // clear the path string before concat operation to ensure
            // null termination
            ZeroMemory(mtlLibPath, sizeof(mtlLibPath));
            memcpy(mtlLibPath, path, (lastSlash - path) + sizeof(char));
            char mtlRelativePath[128];
            sscanf_s(p, "%*s %s", mtlRelativePath, (uint32_t)_countof(mtlRelativePath));
            strcat_s(mtlLibPath, sizeof(mtlLibPath), mtlRelativePath);
        }
        else if (strncmp(p, "usemtl ", 7) == 0)
        {
            materialId++;
            materials[materialId].MaterialId = materialId;
            materials[materialId].startVertexIndex = (uint32_t)vertices.size();
            sscanf_s(p, "%*s %s", materials[materialId].MaterialName, (uint32_t)_countof(materials[materialId].MaterialName));
            materials[materialId].numVertices = 0;
        }
        else if (strncmp(p, "f ", 2) == 0)
        {
            char* faceVertices[10] = {};
            uint32_t count = _countof(faceVertices);
            split(p, faceVertices, &count, ' ');
            ObjVertex firstFaceVertex;
            for (size_t f = 1 /*skip first*/; f < count; f++)
            {
                if (*faceVertices[f] != 0)
                {
                    ObjVertex faceVertex;
                    int vIndex = 0;
                    int tIndex = 0;
                    int nIndex = 0;
                    char* vtn[10] = {};
                    uint32_t cValues = _countof(vtn);
                    split(faceVertices[f], vtn, &cValues, '/');

                    // check for vertex index data
                    if (cValues > 0)
                    {
                        vIndex = atoi(vtn[0]) - 1;
                        faceVertex.Position.x = v[vIndex].x; faceVertex.Position.y = v[vIndex].y; faceVertex.Position.z = v[vIndex].z;
                    }

                    // check for (optional) texture coordinate data
                    if (cValues > 1)
                    {
                        tIndex = atoi(vtn[1]) - 1;
                        faceVertex.TexCoord.x = t[tIndex].x; faceVertex.TexCoord.y = t[tIndex].y;
                    }

                    // check for (optional) normals data
                    if (cValues > 2)
                    {
                        nIndex = atoi(vtn[2]) - 1;
                        faceVertex.Normal.x = n[nIndex].x; faceVertex.Normal.y = n[nIndex].y; faceVertex.Normal.z = n[nIndex].z;
                    }

                    faceVertex.MaterialId = materialId;

                    if (f > 3)
                    {
                        // triangulate vertex before adding
                        vertices.push_back(firstFaceVertex);
                        vertices.push_back(vertices[vertices.size() - 2]);
                        vertices.push_back(faceVertex);
                        if (materialId >= 0)
                        {
                            materials[materialId].numVertices += 3;
                        }
                    }
                    else
                    {
                        vertices.push_back(faceVertex);
                        if (materialId >= 0)
                        {
                            materials[materialId].numVertices++;
                        }
                    }

                    // cache the first vertex for simplified triangulation
                    if (f == 1)
                    {
                        firstFaceVertex = faceVertex;
                    }
                }
            }
        }

        p = next;
    }

    // Build indices list
    for (uint32_t i = 0; i < (uint32_t)vertices.size(); i++)
    {
        indices.push_back(i);
    }

    // Allocate geometry data
    fileData->Stride = sizeof(ObjVertex);
    fileData->Format = 0;

    fileData->NumIndices = (uint32_t)indices.size();
    fileData->Indices = new uint32_t[fileData->NumIndices];
    memcpy(fileData->Indices, indices.data(), indices.size() * sizeof(uint32_t));

    fileData->NumVertices = (uint32_t)vertices.size();
    fileData->Vertices = (uint8_t*) new ObjVertex[fileData->NumVertices];
    memcpy(fileData->Vertices, vertices.data(), vertices.size() * sizeof(ObjVertex));

    ++materialId;
    fileData->NumMaterials = materialId;

    // If we found some materials and we have a material file path, attempt to
    // load it and fill out the materials list with more information.
    if (materialId > 0 && strlen(mtlLibPath))
    {
        LoadMaterialFileDataFromOBJMTL(mtlLibPath, materials, _countof(materials));

        fileData->MaterialTextures = new char*[materialId];
        for (uint32_t i = 0; i < (uint32_t)materialId; ++i)
        {
            char hack[MAX_PATH];
            if (materials[i].DiffuseTexturePath[0] != 0)
            {
                strcpy_s(hack, "castle\\");
                strcat_s(hack, materials[i].DiffuseTexturePath);

                char* dot = strchr(hack, '.');
                *(++dot) = 'd';
                *(++dot) = 'd';
                *(++dot) = 's';
                *(++dot) = 0;
            }
            else
            {
                strcpy_s(hack, "blueprint.dds");
            }

            size_t size = strlen(hack) + 1;
            fileData->MaterialTextures[i] = new char[size];
            strcpy_s(fileData->MaterialTextures[i], size, hack);
        }
    }
}
