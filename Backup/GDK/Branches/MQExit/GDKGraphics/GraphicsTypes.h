#pragma once

#include <d3d11.h>
#include <stde\com_ptr.h>

#include <GDK\ContentTag.h>
#include <GDK\ContentManager.h>
#include <GDK\ObjectModel\GameObject.h>
#include <GDK\ObjectModel\ObjectComponent.h>
#include <stde\ref_counted.h>

// D3D types
typedef stde::com_ptr<IDXGISwapChain>           SwapChainPtr;
typedef stde::com_ptr<ID3D11Device>             DevicePtr;
typedef stde::com_ptr<ID3D11DeviceContext>      ContextPtr;
typedef stde::com_ptr<ID3D11Texture2D>          Texture2DPtr;
typedef stde::com_ptr<ID3D11RenderTargetView>   RenderTargetPtr;
typedef stde::com_ptr<ID3D11DepthStencilView>   DepthStencilPtr;
typedef stde::com_ptr<ID3D11ShaderResourceView> ShaderResourcePtr;
typedef stde::com_ptr<ID3D11Buffer>             BufferPtr;
typedef stde::com_ptr<ID3D11VertexShader>       D3DVertexShaderPtr;
typedef stde::com_ptr<ID3D11PixelShader>        D3DPixelShaderPtr;
typedef stde::com_ptr<ID3D11InputLayout>        InputLayoutPtr;

// GDK types
typedef stde::ref_counted_ptr<GDK::IContentManager> IContentManagerPtr;
typedef stde::ref_counted_ptr<GDK::IContentTag>     IContentTagPtr;
typedef stde::ref_counted_ptr<GDK::IGameObject>     IGameObjectPtr;
typedef stde::ref_counted_ptr<GDK::IObjectComponent> IObjectComponentPtr;
typedef stde::ref_counted_ptr<GDK::IPropertyBag>    IPropertyBagPtr;

