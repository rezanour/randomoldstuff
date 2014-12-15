#pragma once

#include <stde\ref_counted.h>

#include <GDK\ContentTag.h>
#include <GDK\ObjectModel\GameObject.h>
#include <GDK\Graphics.h>

typedef stde::ref_counted_ptr<GDK::IContentTag>     IContentTagPtr;
typedef stde::ref_counted_ptr<GDK::IGameObject>     IGameObjectPtr;
//typedef stde::ref_counted_ptr<GDK::IGraphicsSystem> IGraphicsSystemPtr;
typedef stde::ref_counted_ptr<GDK::IRenderer>       IRendererPtr;
typedef stde::ref_counted_ptr<GDK::IRendererScene>  IRendererScenePtr;
typedef stde::ref_counted_ptr<GDK::IRuntimeResource> IRuntimeResourcePtr;
typedef stde::ref_counted_ptr<GDK::IObjectComponent> IObjectComponentPtr;

