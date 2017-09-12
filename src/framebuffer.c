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

static u32 tiles[ 8961 ];

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
  for( int i = 0; i != 8960; i++ ) {
    tiles[ i ] = 0x00000000;
  }
}

void gtFramebufferUpdate() {
  VDP_loadTileData( tiles, TILE_USERINDEX, 1120, TRUE );
}

void gtFramebufferPlot( u16 x, u16 y, u32 index ) {
  u16 segment = ( ( ( y / 8 ) * 40 ) + ( x / 8 ) ) * 8;

  u16 rowIndex = segment + ( y % 8 );
  u8 columnIndex = ( 7 - ( x % 8 ) ) * 4;

  tiles[ rowIndex ] &= ~( 0xF << columnIndex );
  tiles[ rowIndex ] |= ( index << columnIndex );
}
