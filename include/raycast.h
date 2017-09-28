/**
 * The raycaster engine
 */
#ifndef SMD_RAYCASTER
#define SMD_RAYCASTER

#include <types.h>

#define SCREEN_X 320
#define SCREEN_Y 200

// Player height is half the tile size
#define R_TILE_SIZE 64

#define MAP_WIDTH 24
#define MAP_HEIGHT 24

// Lookup table indexes
#define ANGLE_60 SCREEN_X
#define ANGLE_30 (ANGLE_60/2)
#define ANGLE_15 (ANGLE_30/2)
#define ANGLE_90 (ANGLE_30*3)
#define ANGLE_180 (ANGLE_90*2)
#define ANGLE_270 (ANGLE_90*3)
#define ANGLE_360 (ANGLE_60*6)
#define ANGLE_0 0
#define ANGLE_5 (ANGLE_30/6)
#define ANGLE_10 (ANGLE_5*2)

// why the fuck isn't this in maths.h for sgdk
typedef struct {
  fix32 x;
  fix32 y;
} Vect2D_f32;

void gtRaycasterFrame();

#endif
