#include <types.h>
#include <vdp_bg.h>
#include <tools.h>
#include <stdio.h>
#include "framebuffer.h"
#include "raycast.h"

int main() {
  VDP_resetScreen();

  VDP_drawText( "Initing...", 0, 0 );
  gtInitFramebuffer();

  while(1) {
    gtClearFramebuffer();

    gtRaycasterFrame();

    gtFramebufferUpdate();
  }

  return 0;
}
