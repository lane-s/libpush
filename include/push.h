#include "exported.h"

#ifndef LIBPUSH_PUBLIC_CONSTANTS
  #define LIBPUSH_DISPLAY_HEIGHT 160
  #define LIBPUSH_DISPLAY_WIDTH 960
#endif

extern "C"
{
  EXPORTED bool libpush_disconnect();
  EXPORTED void libpush_draw_frame(unsigned short int (&pixel_buffer)[LIBPUSH_DISPLAY_HEIGHT][LIBPUSH_DISPLAY_WIDTH]);
typedef enum LibPushPort { LIVE, USER } LibPushPort;
EXPORTED bool libpush_connect(LibPushPort port);
}
