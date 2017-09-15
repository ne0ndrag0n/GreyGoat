#include "raycast.h"
#include "framebuffer.h"
#include "config.h"
#include "maths.h"
#include "kdebug.h"
#include <math.h>
#include <string.h>

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
static fix32 posX = FIX32( 22.0 );
static fix32 posY = FIX32( 12.0 );
static fix32 dirX = FIX32( -1.0 );
static fix32 dirY = FIX32( 0.0 );
static fix32 planeX = FIX32( 0.0 );
static fix32 planeY = FIX32( 0.66 );

void gtRaycasterFrame() {
  for( u16 x = 0; x != 320; x++ ) {
    char buf[ 35 ];

    uintToStr( x, buf, 1 );
    KDebug_Alert( buf );

    // 2 * x / double(w) - 1
    fix32 cameraX = fix32Mul( FIX32( 2.0 ), FIX32( x ) );
    cameraX = fix32Div( cameraX, FIX32( 320.0 ) );
    cameraX = fix32Sub( cameraX, FIX32( 1.0 ) );

    fix32ToStr( cameraX, buf, 3 );
    KDebug_Alert( buf );

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

    KDebug_Alert( "x^2" );
    fix32ToStr( rayDirXSquared, buf, 3 );
    KDebug_Alert( buf );

    KDebug_Alert( "y^2" );
    fix32ToStr( rayDirYSquared, buf, 3 );
    KDebug_Alert( buf );

    // 1 + (rayDirY * rayDirY) / (rayDirX * rayDirX)
    fix32 sqrtX = fix32Div( rayDirYSquared, rayDirXSquared );
    sqrtX = fix32Add( sqrtX, FIX32( 1.0 ) );

    // 1 + (rayDirX * rayDirX) / (rayDirY * rayDirY)
    fix32 sqrtY = fix32Div( rayDirXSquared, rayDirYSquared );
    sqrtY = fix32Add( sqrtY, FIX32( 1.0 ) );

    KDebug_Alert( "made it here" );

    // Narrowing conversions!
    fix32 deltaDistX = fix16ToFix32( fix16Sqrt( fix32ToFix16( sqrtX ) ) );
    fix32 deltaDistY = fix16ToFix32( fix16Sqrt( fix32ToFix16( sqrtY ) ) );
    fix32 perpWallDist;

    s8 stepX;
    s8 stepY;

    u8 hit = 0;
    u8 side;
    if( fix32ToInt( rayDirX ) < 0 ) {
      stepX = -1;

      // (rayPosX - mapX) * deltaDistX
      sideDistX = fix32Sub( rayPosX, FIX32( mapX ) );
      sideDistX = fix32Mul( sideDistX, deltaDistX );
    } else {
      stepX = 1;

      // (mapX + 1.0 - rayPosX) * deltaDistX
      sideDistX = fix32Add( FIX32( mapX ), FIX32( 1.0 ) );
      sideDistX = fix32Sub( sideDistX, rayPosX );
      sideDistX = fix32Mul( sideDistX, deltaDistX );
    }

    if( fix32ToInt( rayDirY ) < 0 ) {
      stepY = -1;

      // (rayPosY - mapY) * deltaDistY
      sideDistY = fix32Sub( rayPosY, FIX32( mapY ) );
      sideDistY = fix32Mul( sideDistY, deltaDistY );
    } else {
      stepY = 1;

      // (mapY + 1.0 - rayPosY) * deltaDistY
      sideDistY = fix32Add( FIX32( mapY ), FIX32( 1.0 ) );
      sideDistY = fix32Sub( sideDistY, rayPosY );
      sideDistY = fix32Mul( sideDistY, deltaDistY );
    }

    while( hit == 0 ) {
      if( fix32ToInt( sideDistX ) < fix32ToInt( sideDistY ) ) {
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

    if( side == 0 ) {
      // (mapX - rayPosX + (1 - stepX) / 2) / rayDirX

      perpWallDist = fix32Sub( FIX32( 1.0 ), FIX32( stepX ) );
      perpWallDist = fix32Div( perpWallDist, FIX32( 2.0 ) );
      perpWallDist = fix32Add( perpWallDist, FIX32( mapX ) );
      perpWallDist = fix32Sub( perpWallDist, rayPosX );
      perpWallDist = fix32Div( perpWallDist, rayDirX );
    } else {
      // (mapY - rayPosY + (1 - stepY) / 2) / rayDirY;

      perpWallDist = fix32Sub( FIX32( 1.0 ), FIX32( stepY ) );
      perpWallDist = fix32Div( perpWallDist, FIX32( 2.0 ) );
      perpWallDist = fix32Add( perpWallDist, FIX32( mapY ) );
      perpWallDist = fix32Sub( perpWallDist, rayPosY );
      perpWallDist = fix32Div( perpWallDist, rayDirY );
    }

    u16 lineHeight = fix32ToInt( fix32Div( FIX32( 224.0 ), perpWallDist ) );
    u16 drawStart = -lineHeight / 2 + 224 / 2;
    if( drawStart < 0 ) {
      drawStart = 0;
    }
    u16 drawEnd = lineHeight / 2 + 224 / 2;
    if( drawEnd >= 224 ) {
      drawEnd = 223;
    }

    u8 colourIndex = worldMap[ mapX ][ mapY ];
    if( side == 0 ) {
      colourIndex += 8;
    }

    for( u16 y = drawStart; y != drawEnd; y++ ) {
      gtFramebufferPlot( x, y, colourIndex );
    }
  }
}
