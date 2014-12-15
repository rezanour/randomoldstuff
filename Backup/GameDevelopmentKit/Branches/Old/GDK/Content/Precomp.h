#pragma once

#include <Platform.h>
#include <GDKError.h>
#include <GDKMath.h>
#include <GDKString.h>
#include <RuntimeObject.h>
#include <GDKStream.h>
#include <Resources.h>
#include <ResourceData.h>
#include <Content.h>
#include <vector>
#include <string>
#include <istream>

#ifdef WIN32
#   include "ContentWinClassic\GeometryResourceEdit.h"
#   include "ContentWinClassic\TextureResourceEdit.h"
#   include "ContentWinClassic\WorldResourceEdit.h"
#   include "ContentWinClassic\ResourceFactory.h"
#   include "ContentWinClassic\ContentArchive.h"
#else
#   include "ContentOSX/GeometryResource.h"
#   include "ContentOSX/TextureResource.h"
#   include "ContentOSX/ContentArchive.h"
#endif
