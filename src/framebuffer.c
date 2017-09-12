#include "framebuffer.h"
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
}

void gtClearFramebuffer() {
  for( int i = 0; i != 8960; i++ ) {
    tiles[ i ] = 0x00000000;
  }
}

void gtFramebufferCopy() {
  VDP_loadTileData( tiles, TILE_USERINDEX, 1120, TRUE );
}

void gtFramebufferBlit() {
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

void gtFramebufferPlot( unsigned char x, unsigned char y, unsigned char index ) {
  // Find 8x8 segment (each block on-screen from top left to bottom right occurs in multiples of 8)
  unsigned char cellX = x / 8;
  unsigned char cellY = y / 8;

  unsigned char remX = x % 8;
  unsigned char remY = y % 8;

  unsigned char segment = ( cellX + ( cellY * 40 ) ) * 8;

  u32 selected = tiles[ segment + remY ];
}
