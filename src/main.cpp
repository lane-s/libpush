#include "push.h"
#include <array>
#include <chrono>
#include <iostream>
#include <thread>
#include <vector>

using namespace std;
using Pixel = unsigned short int;

void pad_callback(LibPushPadEvent event, void* context) {
  if (event.event_type == LibPushPadEventType::LP_PAD_PRESSED) {
    cout << dec << "Pad at (" << event.x << ", " << event.y << ") pressed" << endl;
  }
}

void fill_buff(
    Pixel (&pixel_buffer)[LIBPUSH_DISPLAY_HEIGHT][LIBPUSH_DISPLAY_WIDTH],
    Pixel color) {
  for (int i = 0; i < LIBPUSH_DISPLAY_HEIGHT; ++i) {
    for (int j = 0; j < LIBPUSH_DISPLAY_WIDTH; ++j) {
      pixel_buffer[i][j] = color;
    }
  }
}

void rgb_test() {
  Pixel pixel_buffer[LIBPUSH_DISPLAY_HEIGHT][LIBPUSH_DISPLAY_WIDTH];
  const int frames = 30;
  array<Pixel, 3> colors = {0x001F, 0x07E0, 0xF800};

  for (int i = 0; i < frames; ++i) {
    fill_buff(pixel_buffer, colors[i % 3]);
    libpush_draw_frame(pixel_buffer);
    this_thread::sleep_for(chrono::milliseconds(500));
  }
}

int main(int argc, char *argv[]) {
  if (libpush_connect(LibPushPort::LIVE)) {
    cout << "Successfully connected" << endl;
    libpush_register_pad_callback(&pad_callback, nullptr);
    rgb_test();
    this_thread::sleep_for(chrono::milliseconds(5000));
    libpush_disconnect();
    cout << "Disconnected" << endl;
  } else {
    cout << "Unable to connect" << endl;
  }
}
