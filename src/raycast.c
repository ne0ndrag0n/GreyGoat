#include "raycast.h"
#include "framebuffer.h"
#include "config.h"
#include "maths.h"
#include "kdebug.h"
#include "string.h"
#include "goat_math.h"
#include <math.h>
#include <stdbool.h>

const u8 worldMap[ MAP_HEIGHT ][ MAP_WIDTH ] = {
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

static Vect2D_s16 position = { 96, 96 };
static s16 direction = ANGLE_0;

void gtRaycasterFrame() {
  // todo draw background here

  s16 ray = direction - 30;
  if( ray < 0 ) {
    ray = ANGLE_360 + ray;
  }

  Vect2D_s16 grid = { 0, 0 };
  Vect2D_s16 gridIndex = { 0, 0 };
  Vect2D_s16 distToNextGrid = { 0, 0 };
  Vect2D_f32 nextIntersectionDistance = { FIX32( 0.0 ), FIX32( 0.0 ) };
  Vect2D_f32 intersection = { FIX32( 0.0 ), FIX32( 0.0 ) };
  Vect2D_f32 distToGridBeingHit = { FIX32( 0.0 ), FIX32( 0.0 ) };

  for( u16 column = 0; column != SCREEN_X; column++ ) {
    if( ray > ANGLE_0 && ray < ANGLE_180 ) {
      // Ray faces down
      grid.x = ( position.y / R_TILE_SIZE ) * R_TILE_SIZE + R_TILE_SIZE;
      distToNextGrid.x = R_TILE_SIZE;

      intersection.x = fix32Div( FIX32( grid.x - position.y ), tanLookup[ ray ] );
      intersection.x = fix32Add( intersection.x, FIX32( position.x ) );
    } else {
      // Ray faces up
      grid.x = ( position.y / R_TILE_SIZE ) * R_TILE_SIZE;
      distToNextGrid.x = -R_TILE_SIZE;

      intersection.x = fix32Div( FIX32( grid.x - position.y ), tanLookup[ ray ] );
      intersection.x = fix32Add( intersection.x, FIX32( position.x ) );

      grid.x--;
    }

    if( ray == ANGLE_0 || ray == ANGLE_180 ) {
      distToGridBeingHit.x = FIX32( 2097151.0 );
    } else {
      nextIntersectionDistance.x = fix32Div( FIX32( R_TILE_SIZE ), tanLookup[ ray ] );
      if( nextIntersectionDistance.x < FIX32( 0.0 ) ) {
        nextIntersectionDistance.x = -nextIntersectionDistance.x;
      }

      while( true ) {
        gridIndex.x = fix32ToInt( fix32Div( intersection.x, FIX32( R_TILE_SIZE ) ) );
        gridIndex.y = grid.x / R_TILE_SIZE;

        if(
          ( gridIndex.x >= MAP_WIDTH ) ||
          ( gridIndex.y >= MAP_HEIGHT ) ||
          ( gridIndex.x < 0 ) ||
          ( gridIndex.y < 0 )
        ) {
          distToGridBeingHit.x = FIX32( 2097151.0 );
          break;
        } else if ( worldMap[ gridIndex.y ][ gridIndex.x ] ) {
          distToGridBeingHit.x = fix32Sub( intersection.x, FIX32( position.x ) );
          distToGridBeingHit.x = fix32Div( distToGridBeingHit.x, cosLookup[ ray ] );
          break;
        } else {
          intersection.x = fix32Add( intersection.x, nextIntersectionDistance.x );
          grid.x += distToNextGrid.x;
        }
      }
    }

  }

  KDebug_Alert( "System halted" );
  while( 1 );
}
