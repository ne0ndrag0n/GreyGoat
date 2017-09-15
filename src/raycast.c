#include "raycast.h"
#include "framebuffer.h"
#include "config.h"
#include "maths.h"
#include <math.h>

const u8 worldMap[ mapWidth ][ mapHeight ] = {
  {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,2,2,2,2,2,0,0,0,0,3,0,3,0,3,0,0,0,1},
  {1,0,0,0,0,0,2,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,2,0,0,0,2,0,0,0,0,3,0,0,0,3,0,0,0,1},
  {1,0,0,0,0,0,2,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,2,2,0,2,2,0,0,0,0,3,0,3,0,3,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,4,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,0,4,0,0,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,0,0,0,0,5,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,0,4,0,0,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,0,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,4,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};

// Software framebuffer on top of floating point emulation
// LMAO this is going to be so slow
static fix32 posX = FIX32( 22 );
static fix32 posY = FIX32( 12 );
static fix32 dirX = FIX32( -1 );
static fix32 dirY = FIX32( 0 );
static fix32 planeX = FIX32( 0 );
static fix32 planeY = FIX32( 0.66 );

void gtRaycasterFrame() {
  for( u16 x = 0; x != 320; x++ ) {

    // 2 * x / double(w) - 1
    fix32 cameraX = fix32Mul( 2, x );
    cameraX = fix32Div( cameraX, 320 );
    cameraX = fix32Sub( cameraX, 1 );

    fix32 rayPosX = posX;
    fix32 rayPosY = posY;

    // dirX + planeX * cameraX
    fix32 rayDirX = fix32Mul( planeX, cameraX );
    rayDirX = fix32Add( rayDirX, dirX );

    // dirY + planeY * cameraX
    fix32 rayDirY = fix32Mul( planeY, cameraX );
    rayDirY = fix32Add( rayDirY, dirY );

    s16 mapX = fix32ToInt( rayPosX );
    s16 mapY = fix32ToInt( rayPosY );

    fix32 sideDistX;
    fix32 sideDistY;

    fix32 rayDirXSquared = fix32Mul( rayDirX, rayDirX );
    fix32 rayDirYSquared = fix32Mul( rayDirY, rayDirY );

    // 1 + (rayDirY * rayDirY) / (rayDirX * rayDirX)
    fix32 sqrtX = fix32Div( rayDirYSquared, rayDirXSquared );
    sqrtX = fix32Add( sqrtX, 1 );

    // 1 + (rayDirX * rayDirX) / (rayDirY * rayDirY)
    fix32 sqrtY = fix32Div( rayDirXSquared, rayDirYSquared );
    sqrtY = fix32Add( sqrtY, 1 );

    // Narrowing conversions!
    fix32 deltaDistX = fix16ToFix32( fix16Sqrt( fix32ToFix16( sqrtX ) ) );
    fix32 deltaDistY = fix16ToFix32( fix16Sqrt( fix32ToFix16( sqrtY ) ) );
    fix32 perpWallDist;

    s8 stepX;
    s8 stepY;

    u8 hit = 0;
    u8 side;
    if( fix32Int( rayDirX ) < 0 ) {
      stepX = -1;

      // (rayPosX - mapX) * deltaDistX
      sideDistX = fix32Sub( rayPosX, mapX );
      sideDistX = fix32Mul( sideDistX, deltaDistX );
    } else {
      stepX = 1;

      // (mapX + 1.0 - rayPosX) * deltaDistX
      sideDistX = fix32Add( mapX, 1 );
      sideDistX = fix32Sub( sideDistX, rayPosX );
      sideDistX = fix32Mul( sideDistX, deltaDistX );
    }

    if( fix32Int( rayDirY ) < 0 ) {
      stepY = -1;

      // (rayPosY - mapY) * deltaDistY
      sideDistY = fix32Sub( rayPosY, mapY );
      sideDistY = fix32Mul( sideDistY, deltaDistY );
    } else {
      stepY = 1;

      // (mapY + 1.0 - rayPosY) * deltaDistY
      sideDistY = fix32Add( mapY, 1 );
      sideDistY = fix32Sub( sideDistY, rayPosY );
      sideDistY = fix32Mul( sideDistY, deltaDistY );
    }

    while( hit == 0 ) {
      if( fix32Int( sideDistX ) < fix32Int( sideDistY ) ) {
        sideDistX = fix32Add( sideDistX, deltaDistX );
        mapX += stepX;
        side = 0;
      } else {
        sideDistY = fix32Add( sideDistY, deltaDistY );
        mapY += stepY;
        side = 1;
      }

      if( worldMap[ mapX ][ mapY ] > 0 ) {
        hit = 1;
      }
    }

    // TODO
  }
}
