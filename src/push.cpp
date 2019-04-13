#include "push.hpp"

using namespace std;
using Pixel = unsigned short int;

bool libpush_connect(LibPushPort port) {
  try {
    midi_interface = new MidiInterface();
    midi_interface->connect(port);
  } catch (exception &ex) {
    cout << ex.what() << endl;
    return false;
  }

  try {
    display_interface = new DisplayInterface();
    display_interface->connect();
  } catch (exception &ex) {
    cerr << ex.what() << endl;
    return false;
  }

  return true;
}

bool libpush_disconnect() {
  if (!display_interface || !midi_interface) {
    return false;
  }

  try {
    display_interface->disconnect();
    midi_interface->disconnect();
    return true;
  } catch (exception &ex) {
    cerr << ex.what() << endl;
    return false;
  }
}

void libpush_draw_frame(
    Pixel (&buffer)[LIBPUSH_DISPLAY_HEIGHT][LIBPUSH_DISPLAY_WIDTH]) {
  try {
    display_interface->draw_frame(buffer);
  } catch (exception &ex) {
    cerr << ex.what() << endl;
  }
}

const string CALLBACK_ERROR_MSG = "Ensure libpush_connect is successful before "
                                  "attempting to register callbacks";

void libpush_register_pad_callback(LibPushPadCallback cb, void *user_data) {
  if (!display_interface || !midi_interface) {
    cerr << CALLBACK_ERROR_MSG << endl;
    return;
  }
  midi_interface->pad_listener.register_callback(cb, user_data);
}

void libpush_register_button_callback(LibPushButtonCallback cb,
                                      void *user_data) {
  if (!display_interface || !midi_interface) {
    cerr << CALLBACK_ERROR_MSG << endl;
    return;
  }
  midi_interface->button_listener.register_callback(cb, user_data);
}

void libpush_register_encoder_callback(LibPushEncoderCallback cb,
                                       void *user_data) {
  if (!display_interface || !midi_interface) {
    cerr << CALLBACK_ERROR_MSG << endl;
    return;
  }
  midi_interface->encoder_listener.register_callback(cb, user_data);
}

void libpush_register_touch_strip_callback(LibPushTouchStripCallback cb,
                                           void *user_data) {
  if (!display_interface || !midi_interface) {
    cerr << CALLBACK_ERROR_MSG << endl;
    return;
  }
  midi_interface->touch_strip_listener.register_callback(cb, user_data);
}
