#include "push.hpp"

using namespace std;
using Pixel = unsigned short int;

PushInterface *push;

PushInterface::PushInterface(LibPushPort port)
    : sysex(midi), leds(midi, sysex) {
  midi.connect(port);
  display.connect();
}

PushInterface::~PushInterface() {
  midi.disconnect();
  display.disconnect();
}

bool libpush_connect(LibPushPort port) {
  try {
    push = new PushInterface(port);
  } catch (exception &ex) {
    cerr << ex.what() << endl;
    return false;
  }

  return true;
}

bool libpush_disconnect() {
  if (!push) {
    cerr << "Disconnecting when not connected" << endl;
    return false;
  }

  try {
    delete push;
  } catch (exception &ex) {
    cerr << ex.what() << endl;
    return false;
  }
}

void libpush_draw_frame(
    Pixel (&buffer)[LIBPUSH_DISPLAY_HEIGHT][LIBPUSH_DISPLAY_WIDTH]) {
  try {
    push->display.draw_frame(buffer);
  } catch (exception &ex) {
    cerr << ex.what() << endl;
  }
}

const string CALLBACK_ERROR_MSG = "Ensure libpush_connect is successful before "
                                  "attempting to register callbacks";

void libpush_register_pad_callback(LibPushPadCallback cb, void *context) {
  if (!push) {
    cerr << CALLBACK_ERROR_MSG << endl;
    return;
  }
  push->midi.pad_listener.register_callback(cb, context);
}

void libpush_register_button_callback(LibPushButtonCallback cb, void *context) {
  if (!push) {
    cerr << CALLBACK_ERROR_MSG << endl;
    return;
  }
  push->midi.button_listener.register_callback(cb, context);
}

void libpush_register_encoder_callback(LibPushEncoderCallback cb,
                                       void *context) {
  if (!push) {
    cerr << CALLBACK_ERROR_MSG << endl;
    return;
  }
  push->midi.encoder_listener.register_callback(cb, context);
}

void libpush_register_touch_strip_callback(LibPushTouchStripCallback cb,
                                           void *context) {
  if (!push) {
    cerr << CALLBACK_ERROR_MSG << endl;
    return;
  }
  push->midi.touch_strip_listener.register_callback(cb, context);
}
