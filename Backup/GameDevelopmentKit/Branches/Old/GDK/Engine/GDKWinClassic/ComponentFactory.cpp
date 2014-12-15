#include "Precomp.h"
#include <map>
#include <vector>

using namespace GDK;

typedef std::map<HMODULE, std::vector<ComponentDescription>> ComponentMap;
static ComponentMap* Components;

extern "C"
{
    size_t ComponentFinderGetNumComponents()
    {
        if (!Components)
        {
            Components = new ComponentMap;

            WIN32_FIND_DATA findData = {0};
            HANDLE findHandle = FindFirstFile(L"*.dll", &findData);
            if (findHandle)
            {
                do
                {
                    HMODULE module = LoadLibrary(findData.cFileName);
                    if (module)
                    {
                        pfnGetNumComponents getNumComponents = (pfnGetNumComponents)GetProcAddress(module, "GetNumComponents");
                        pfnGetComponents getComponents = (pfnGetComponents)GetProcAddress(module, "GetComponents");

                        if (getNumComponents && getComponents)
                        {
                            size_t numModuleComponents = getNumComponents();
                            if (numModuleComponents > 0)
                            {
                                std::vector<ComponentDescription> descriptions(numModuleComponents);
                                if (SUCCEEDED(getComponents(descriptions.data(), descriptions.size(), &numModuleComponents)))
                                {
                                    Components->insert(std::pair<HMODULE, std::vector<ComponentDescription>>(module, descriptions));
                                    module = NULL; // don't free this, we're hanging onto it.
                                }
                            }
                        }

                        if (module)
                        {
                            // this module isn't interesting, and we're not keeping the module handle around, so dump it
                            FreeLibrary(module);
                        }
                    }
                } while (FindNextFile(findHandle, &findData));

                FindClose(findHandle);
                findHandle = nullptr;
            }
        }

        size_t numComponents = 0;

        if (Components)
        {
            for (auto it = Components->begin(); it != Components->end(); ++it)
            {
                numComponents += it->second.size();
            }
        }

        return numComponents;
    }

    HRESULT ComponentFinderGetComponents(_Inout_updates_to_(maxComponents, *numComponents) ComponentDescription* components, _In_ size_t maxComponents, _Out_ size_t* numComponents)
    {
        if (!components || !numComponents)
        {
            return E_INVALIDARG;
        }

        if (maxComponents < ComponentFinderGetNumComponents())
        {
            return HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER);
        }

        size_t numFilled = 0;
        for (auto it = Components->begin(); it != Components->end(); ++it)
        {
            for (size_t i = 0; i < it->second.size(); ++i)
            {
                components[numFilled++] = it->second[i];
            }
        }

        *numComponents = numFilled;

        return S_OK;
    }

    HRESULT ComponentFinderCreateComponentFactory(_In_ const ComponentDescription& component, REFIID riid, void** factory)
    {
        assert(Components && !Components->empty());
        if (!Components || Components->empty())
        {
            return E_UNEXPECTED;
        }

        for (auto it = Components->begin(); it != Components->end(); ++it)
        {
            for (size_t i = 0; i < it->second.size(); ++i)
            {
                if (component.factoryId == it->second[i].factoryId &&
                    component.internalId == it->second[i].internalId)
                {
                    pfnCreateComponentFactory createComponentFactory = (pfnCreateComponentFactory)GetProcAddress(it->first, "CreateComponentFactory");
                    if (!createComponentFactory)
                    {
                        return E_FAIL;
                    }

                    return createComponentFactory(component, riid, factory);
                }
            }
        }

        return HRESULT_FROM_WIN32(ERROR_NOT_FOUND);
    }

    void UnloadAllComponentModules()
    {
        if (Components)
        {
            for (auto it = Components->begin(); it != Components->end(); ++it)
            {
                FreeLibrary(it->first);
            }

            delete Components;
            Components = nullptr;
        }
    }
}
