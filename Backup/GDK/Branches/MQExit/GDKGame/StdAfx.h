#pragma once

#include <GDK\Platform.h>

#include <strsafe.h>

// STL isn't warning level 4 clean, so we lower warning level temporarily
#pragma warning(push, 3)
#pragma warning(disable: 4995)
#include <string>
#include <memory>
#include <vector>
#include <map>
#pragma warning(pop)

#include <stde\types.h>
#include <stde\non_copyable.h>
#include <stde\ref_counted.h>
#include <stde\conversion.h>

#include <CoreServices\Debug.h>
#include <CoreServices\Error.h>
#include <CoreServices\Configuration.h>
#include <CoreServices\FileStream.h>
#include <CoreServices\ContentTag.h>
#include <CoreServices\MemoryStream.h>
#include <CoreServices\StreamHelper.h>
#include <CoreServices\PropertyBag.h>

#include <GDK\ContentManager.h>
#include <GDK\RuntimeResource.h>
#include <GDK\ContentTag.h>
#include <GDK\ObjectModel\GameObject.h>
#include <GDK\ObjectModel\PropertyBag.h>
#include <GDK\ObjectModel\ObjectComponent.h>
#include <GDK\Graphics.h>

#include <DirectXTK\DirectXMath.h>

#include "GDKTypes.h"
