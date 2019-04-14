#include "push.h"
#include <array>
#include <chrono>
#include <iostream>
#include <thread>
#include <vector>

using namespace std;
using Pixel = unsigned short int;

void pad_callback(LibPushPadEvent event, void *context) {
  if (event.event_type == LibPushPadEventType::LP_PAD_PRESSED) {
    cout << "Pad at (" << event.x << ", " << event.y << ") pressed" << endl;
  }
}

void button_callback(LibPushButtonEvent event, void *context) {
  if (event.event_type == LibPushButtonEventType::LP_BTN_PRESSED) {
    if (event.button == LibPushButton::LP_DISPLAY_TOP_BTN) {
      cout << "Top row button " << event.button_index << " pressed" << endl;
    } else if (event.button == LibPushButton::LP_DISPLAY_BOTTOM_BTN) {
      cout << "Bottom row button " << event.button_index << " pressed" << endl;
    } else if (event.button == LibPushButton::LP_SCENE_BTN) {
      cout << "Scene button " << event.button_index << " pressed" << endl;
    } else if (event.button == LibPushButton::LP_PLAY_BTN) {
      cout << "Play button pressed" << endl;
    }
  }
}

void encoder_callback(LibPushEncoderEvent event, void *context) {
  if(event.event_type == LibPushEncoderEventType::LP_ENCODER_TOUCHED) {
    cout << "Encoder " << event.index << " touched" << endl;
  }
  else if (event.event_type == LibPushEncoderEventType::LP_ENCODER_RELEASED) {
    cout << "Encoder " << event.index << " released" << endl;
  } else if (event.event_type == LibPushEncoderEventType::LP_ENCODER_MOVED) {
    cout << "Encoder " << event.index << " moved by " << event.delta << endl;
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
    libpush_register_button_callback(&button_callback, nullptr);
    libpush_register_encoder_callback(&encoder_callback, nullptr);
    rgb_test();
    this_thread::sleep_for(chrono::milliseconds(5000));
    libpush_disconnect();
    cout << "Disconnected" << endl;
  } else {
    cout << "Unable to connect" << endl;
  }
}
