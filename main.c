#include <types.h>
#include <vdp_bg.h>
#include <stdio.h>
#include "framebuffer.h"

int main() {
  VDP_resetScreen();

  VDP_drawText( "Initing...", 0, 0 );
  gtInitFramebuffer();

  VDP_drawText( "Copying...", 0, 1 );
  gtFramebufferCopy();

  VDP_drawText( "GreyGoat", 0, 2 );
  while(1);

  return 0;
}
