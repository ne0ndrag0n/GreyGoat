/**
 * The raycaster engine
 */
#ifndef SMD_RAYCASTER
#define SMD_RAYCASTER

#include <types.h>

#define ScreenX 320
#define ScreenY 200
#define TileSize 64
#define PlayerHeight 32
#define AngleIncrement FIX32( 0.188 )

#define mapWidth 24
#define mapHeight 24

// why the fuck isn't this in maths.h for sgdk
typedef struct {
  fix32 x;
  fix32 y;
} Vect2D_f32;

void gtRaycasterFrame();

#endif
