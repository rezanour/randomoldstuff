#ifndef _MAP_H_
#define _MAP_H_

//
// The map module(s) contains all code related to loading and processing a map in the game.
// This includes owning the bsp tree, processing object movement and collision detection.
// The map also can answer queries like line of sight or other tests against the map data.
//

BOOL MapLoad(const char* name);
void MapDestroy();

void MapUpdate();

#endif // _MAP_H_