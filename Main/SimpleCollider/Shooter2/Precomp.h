#pragma once

#define NOMINMAX
#include <Windows.h>
#include <DirectXMath.h>
#include <d3d11.h>
#include <wrl.h>

#include <stddef.h>
#include <stdint.h>
#include <assert.h>

#include <memory>
#include <vector>
#include <string>
#include <exception>

//
// Collision Library
//
#include <SimpleCollider\SimpleCollider.h>

//
// DirectXTK for test app
//
#include <GeometricPrimitive.h>
#include <Model.h>
#include <Effects.h>
#include <SpriteBatch.h>
#include <SpriteFont.h>

using namespace Microsoft::WRL;
using namespace Microsoft::WRL::Wrappers;
using namespace DirectX;

//
// Local headers
//
#include "Debug.h"
#include "Renderer.h"
#include "Game.h"
