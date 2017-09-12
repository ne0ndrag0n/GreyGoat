#include "goat_debug.h"
#include <kdebug.h>
#include <stdio.h>

void gtDebugPrintU32( char* message, u32 data ) {
  char container[ 100 ] = { 0 };

  sprintf( container, message, data );
  KDebug_Alert( container );
}
