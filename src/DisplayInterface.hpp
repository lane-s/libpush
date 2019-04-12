#include "push.h"
#include "libusb.h"
#include <iostream>
#include <string>

#ifndef LIBPUSH_DISPLAY_CONSTANTS
  #define DISPLAY_CONSTANTS
  #define DISPLAY_HEIGHT LIBPUSH_DISPLAY_HEIGHT
  #define DISPLAY_WIDTH LIBPUSH_DISPLAY_WIDTH
  #define DISPLAY_WIDTH_BYTES (DISPLAY_WIDTH * 2)
  #define DISPLAY_PADDING_BYTES 128
  #define ROW_LENGTH (DISPLAY_WIDTH_BYTES + DISPLAY_PADDING_BYTES)
  #define FRAME_BUFFER_LENGTH (ROW_LENGTH * (DISPLAY_HEIGHT * 2))
#endif

class DisplayInterface {
public:
  using Pixel = unsigned short int;

  DisplayInterface();
  ~DisplayInterface();

  void connect();
  void disconnect();
  void draw_frame(Pixel (&pixel_buffer)[DISPLAY_HEIGHT][DISPLAY_WIDTH]);
private:
  libusb_device_handle* push2_handle;

  static void fill_frame(Pixel (&pixel_buffer)[DISPLAY_HEIGHT][DISPLAY_WIDTH],
                         unsigned char (&frame_buffer)[FRAME_BUFFER_LENGTH]);
};
