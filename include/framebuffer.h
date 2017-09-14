/**
 * Software framebuffer for the Sega Megadrive
 */
#ifndef SMD_FRAMEBUFFER
#define SMD_FRAMEBUFFER

#include <types.h>

void gtInitFramebuffer();
void gtClearFramebuffer();
void gtFramebufferUpdate();

void gtFramebufferPlot( u16 x, u16 y, u8 index );

#endif
