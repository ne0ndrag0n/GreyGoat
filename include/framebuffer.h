/**
 * Software framebuffer for the Sega Megadrive
 */
#ifndef FRAMEBUFFER_SMD
#define FRAMEBUFFER_SMD

#include <types.h>

void gtInitFramebuffer();
void gtClearFramebuffer();
void gtFramebufferCopy();
void gtFramebufferBlit();

void gtFramebufferPlot(
  unsigned char x,
  unsigned char y,
  unsigned char index 
);

#endif
