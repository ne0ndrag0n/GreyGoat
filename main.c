#include <types.h>
#include <vdp_bg.h>
#include <tools.h>
#include <stdio.h>
#include "framebuffer.h"

int main() {
  VDP_resetScreen();

  VDP_drawText( "Initing...", 0, 0 );
  gtInitFramebuffer();

  while(1) {
    gtClearFramebuffer();

    // Draw 10,000 pixels on screen before blitting
    u8 color = random() % 16;

    for( u16 x = 0; x != 320; x++ ) {
      u8 lineLength = random() % 224;

      u16 start = 112 - ( lineLength / 2 );
      u16 end = 112 + ( lineLength / 2 );
      for( u16 y = start; y != end; y++ ) {
        gtFramebufferPlot( x, y, color );
      }
    }

    gtFramebufferUpdate();
  }

  return 0;
}
