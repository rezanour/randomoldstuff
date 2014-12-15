#pragma once

#include <Platform.h>
#include <RuntimeObject.h>
#include <GDKMath.h>
#include <GDKError.h>

#include <GraphicsDevice.h>
#include <Texture.h>
#include <Geometry.h>

#include <AudioDevice.h>
#include <AudioClip.h>

#include <FileSystem.h>

#include <Content.h>
#include <GeometryContent.h>
#include <TextureContent.h>
#include <AudioContent.h>

#include <Input.h>

#include <DeviceContext.h>
#include <GameTime.h>
#include <UpdateResult.h>

#include <VisualInfo.h>
#include <Content.h>
#include <Collision.h>
#include <CollisionPrimitives.h>
#include <CollisionHelpers.h>
#include <Transform.h>

#include <StringUtils.h>

namespace GDK
{
    void Startup();
    void Shutdown();
}
