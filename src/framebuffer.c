#include "framebuffer.h"
#include "goat_debug.h"
#include <vdp_tile.h>
#include <vdp_pal.h>
#include <kdebug.h>
#include <stdio.h>

const u16 stdPalette[ 16 ] = {
  0x0000,0x0800,0x0080,0x0880,0x0008,0x0808,0x0088,0x0CCC,
  0x0888,0x0E00,0x00E0,0x0EE0,0x000E,0x0E0E,0x00EE,0x0EEE
};

static u8 tiles[ 35841 ];

void gtInitFramebuffer() {
  VDP_setPalette( PAL0, stdPalette );

  gtClearFramebuffer();
  gtFramebufferUpdate();

  VDP_fillTileMapRectInc(
    PLAN_A,
    TILE_ATTR_FULL(
      PAL0,
      FALSE,
      FALSE,
      FALSE,
      TILE_USERINDEX
    ),
    0,
    0,
    40,
    28
  );
}

void gtClearFramebuffer() {
  for( u16 i = 0; i != 35840; i++ ) {
    tiles[ i ] = 0x00;
  }
}

void gtFramebufferUpdate() {
  VDP_loadTileData( ( u32* ) tiles, TILE_USERINDEX, 1120, TRUE );
}

void gtFramebufferPlot( u16 x, u16 y, u8 index ) {
  if( !( x % 2 ) ) {
    index <<= 4;
  }

  u8 cellX = x / 8;
  u8 cellY = y / 8;

  u8 posX = x % 8;
  u8 posY = y % 8;

  // Get us to the 32x32 cell needed. Then get us to the row in that cell needed. Then get us to the column.
  u16 offset = ( ( 1280 * cellY ) + ( 32 * cellX ) ) + ( posY * 4 ) + ( posX / 2 );
  tiles[ offset ] |= index;
}
