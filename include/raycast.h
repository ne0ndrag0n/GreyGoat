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

// why the fuck isn't this in maths.h for sgdk
typedef struct {
  fix32 x;
  fix32 y;
} Vect2D_f32;

void gtRaycasterFrame();

#endif
