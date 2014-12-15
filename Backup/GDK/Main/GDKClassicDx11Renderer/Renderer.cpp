#include "StdAfx.h"

using GDK::Renderer;
using GDK::IRendererScene;

GDK_METHOD_(stde::ref_ptr<Renderer>) Renderer::Create(_In_ HWND hwnd)
{
    assert(hwnd);

    return stde::ref_ptr<Renderer>(new Renderer(hwnd));
}

Renderer::Renderer(_In_ HWND hwnd)
    : _hwnd(hwnd)
{
    assert(_hwnd && "Must provide valid hwnd handle");
}

Renderer::~Renderer()
{
    _hwnd = nullptr;
}

GDK_IMETHODIMP_(void) Renderer::AddScene(_In_ const stde::ref_ptr<IRendererScene>& scene)
{
    UNREFERENCED_PARAMETER(scene);
}

GDK_IMETHODIMP_(void) Renderer::GetScenes(_Inout_ std::vector<stde::ref_ptr<IRendererScene>>& scenes) const
{
    UNREFERENCED_PARAMETER(scenes);
}

GDK_IMETHODIMP_(void) Renderer::RenderAll()
{
}

GDK_IMETHODIMP_(stde::ref_ptr<GDK::IMemoryStats>) Renderer::GetMemoryStats()
{
    stde::ref_ptr<IMemoryStats> stats;
    stats.reset(dynamic_cast<GDK::IMemoryStats*>(this));
    return stats;
}
