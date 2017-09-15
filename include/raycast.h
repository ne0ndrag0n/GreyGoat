/**
 * The raycaster engine
 */
#ifndef SMD_RAYCASTER
#define SMD_RAYCASTER

#include <types.h>

#define mapWidth 24
#define mapHeight 24

extern const u8 worldMap[ mapWidth ][ mapHeight ];

void gtRaycasterFrame();

#endif
