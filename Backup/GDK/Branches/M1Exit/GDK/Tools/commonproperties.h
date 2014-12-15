#pragma once

#include <Windows.h>

///////////////////////////////////////////////////////////////////////////////
// Description: The full path where the files related to the project are 
//              discovered. This value contains a trailing slash.
// Type: string
// 
const CHAR ProjectRootProperty[] = "ProjectRoot";

///////////////////////////////////////////////////////////////////////////////
// Description: The full path where project specific raw resources will be 
//              imported. This value contains a trailing slash.
// Type: string
// 
const CHAR ContentRepositoryRootProperty[] = "ContentRepositoryRoot";

///////////////////////////////////////////////////////////////////////////////
// Description: The full path where shared raw resources will be imported. 
//              This value contains a trailing slash.
// Type: string
// 
const CHAR SharedContentRepositoryRootProperty[] = "SharedContentRepositoryRoot";

///////////////////////////////////////////////////////////////////////////////
// Description: The full commandline path where the game exe is located.
//
// Type: string
// 
const CHAR GameRunPathProperty[] = "GameRunPath";

///////////////////////////////////////////////////////////////////////////////
// Description: A collection of objects.  This interface value is assumed to 
//              be an IContentStudioPropertiesCollection type.
//
// Details: ParentIdProperty and IdProperty should be used to interpret any
//          heiarchy defined in the collection.
//
// Type: interface (IContentStudioPropertiesCollection)
// 
const CHAR DocumentObjectsProperty[] = "DocumentObjects";

///////////////////////////////////////////////////////////////////////////////
// Description: A currently selected object.  This object is considered to be
//              actively selected by the user.  This interface value is assumed to 
//              be an IContentStudioProperties type.
//
// Details: ParentIdProperty and IdProperty should be used to interpret any
//          additional metadata if needed.
//
// Type: interface (IContentStudioProperties)
// 
const CHAR SelectedObjectProperty[] = "SelectedObject";

///////////////////////////////////////////////////////////////////////////////
// Description: The identifier of any object that represents a parent 
//              identifier of another object.  This value is assumed to
//              be unique.
//
// Type: integer (64 bit)
// 
const CHAR ParentIdProperty[] = "ParentId";

///////////////////////////////////////////////////////////////////////////////
// Description: The identifier of any object.  This value is assumed to
//              be unique.
//
// Type: integer (64 bit)
// 
const CHAR IdProperty[] = "Id";

///////////////////////////////////////////////////////////////////////////////
// Description: The name of any object.  This value is assumed to
//              be human readable and printable to user interface code.
//
// Type: string
// 
const CHAR NameProperty[] = "Name";

///////////////////////////////////////////////////////////////////////////////
// Description: The format of any object.  This value is assumed to
//              be different depending on the type of object.
//
// Type: integer
// 
const CHAR FormatProperty[] = "Format";

///////////////////////////////////////////////////////////////////////////////
// Description: The type of any object.
//
// Type: integer
// 
const CHAR TypeProperty[] = "Type";

///////////////////////////////////////////////////////////////////////////////
// Description: The subtype of any object.
//
// Type: integer
// 
const CHAR SubTypeProperty[] = "SubType";

///////////////////////////////////////////////////////////////////////////////
// Description: The width of any object.
//
// Type: float
// 
const CHAR WidthProperty[] = "Width";

///////////////////////////////////////////////////////////////////////////////
// Description: The height of any object.
//
// Type: float
// 
const CHAR HeightProperty[] = "Height";

///////////////////////////////////////////////////////////////////////////////
// Description: A collection of object identifiers that are linked to a
//              a particular object. A link to another object is intended to 
//              be used as a way to attach additional metadata. Links should 
//              not be used to define parent/child relationships.
//              ParentIdProperty and IdProperty should be used for defining.
//
// Type: string
//
// Details: This value is a comma delimited list of identifiers.
//          (ex. 32324,2343,34342,11232,33435)
// 
const CHAR LinkedIdsProperty[] = "LinkedIds";

///////////////////////////////////////////////////////////////////////////////
// Description: The full path to an object where the file is located on disk
//
// Type: string
// 
const CHAR FullPathProperty[] = "FullPath";

///////////////////////////////////////////////////////////////////////////////
// Description: The object represents a folder and is assumed to be a parent
//              object.  This is commonly used for UI controls and visual
//              representations only.
//
// Type: integer
// 
const CHAR IsFolderProperty[] = "IsFolder";