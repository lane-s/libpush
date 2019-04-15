#include "push.hpp"

using namespace std;
using Pixel = unsigned short int;

PushInterface *push;
const string NOT_CONNECTED_MSG = "Please ensure libpush_connect is successful "
                                 "before using other parts of the API";

PushInterface::PushInterface(LibPushPort port)
    : sysex(midi), display(sysex), leds(midi, sysex), misc(sysex),
      pedals(midi, sysex), encoders(midi), pads(midi, sysex, leds) {
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

void libpush_set_display_brightness(unsigned char brightness) {
  if (!push) {
    cerr << NOT_CONNECTED_MSG << endl;
    return;
  }
  push->display.set_brightness(brightness);
}

unsigned char libpush_get_display_brightness() {
  if (!push) {
    cerr << NOT_CONNECTED_MSG << endl;
    return 0;
  }
  return push->display.get_brightness();
}

void libpush_register_pad_callback(LibPushPadCallback cb, void *context) {
  if (!push) {
    cerr << NOT_CONNECTED_MSG << endl;
    return;
  }
  push->pads.register_callback(cb, context);
}

void libpush_register_button_callback(LibPushButtonCallback cb, void *context) {
  if (!push) {
    cerr << NOT_CONNECTED_MSG << endl;
    return;
  }
  push->midi.button_listener.register_callback(cb, context);
}

void libpush_register_encoder_callback(LibPushEncoderCallback cb,
                                       void *context) {
  if (!push) {
    cerr << NOT_CONNECTED_MSG << endl;
    return;
  }
  push->encoders.register_callback(cb, context);
}

void libpush_register_touch_strip_callback(LibPushTouchStripCallback cb,
                                           void *context) {
  if (!push) {
    cerr << NOT_CONNECTED_MSG << endl;
    return;
  }
  push->midi.touch_strip_listener.register_callback(cb, context);
}

void libpush_register_pedal_callback(LibPushPedalCallback cb, void *context) {
  if (!push) {
    cerr << NOT_CONNECTED_MSG << endl;
    return;
  }
  push->pedals.register_callback(cb, context);
}

LibPushPedalSampleData libpush_sample_pedals(unsigned char sample_size) {
  if (!push) {
    cerr << NOT_CONNECTED_MSG << endl;
    LibPushPedalSampleData d;
    return d;
  }
  return push->pedals.sample_pedals(sample_size);
}

void libpush_set_pedal_curve_limits(LibPushPedalContact contact,
                                    unsigned short heel_down,
                                    unsigned short toe_down) {
  if (!push) {
    cerr << NOT_CONNECTED_MSG << endl;
    return;
  }
  return push->pedals.set_pedal_curve_limits(contact, heel_down, toe_down);
}

void libpush_set_pedal_curve_entries(
    LibPushPedalContact contact,
    unsigned char (&entries)[LIBPUSH_PEDAL_CURVE_ENTRIES]) {
  if (!push) {
    cerr << NOT_CONNECTED_MSG << endl;
    return;
  }
  return push->pedals.set_pedal_curve_entries(contact, entries);
}

void libpush_set_led_color_palette_entry(unsigned char color_index,
                                         LibPushLedColor color) {
  if (!push) {
    cerr << NOT_CONNECTED_MSG << endl;
    return;
  }
  push->leds.set_led_color_palette_entry(color_index, color);
}

LibPushLedColor libpush_get_led_color_palette_entry(unsigned char color_index) {
  if (!push) {
    cerr << NOT_CONNECTED_MSG << endl;
    LibPushLedColor c;
    return c;
  }
  return push->leds.get_led_color_palette_entry(color_index);
}

void libpush_reapply_color_palette() {
  if (!push) {
    cerr << NOT_CONNECTED_MSG << endl;
    return;
  }
  push->leds.reapply_color_palette();
}

void libpush_set_global_led_brightness(unsigned char brightness) {
  if (!push) {
    cerr << NOT_CONNECTED_MSG << endl;
    return;
  }
  push->leds.set_global_led_brightness(brightness);
}

unsigned char libpush_get_global_led_brightness() {
  if (!push) {
    cerr << NOT_CONNECTED_MSG << endl;
    return 0;
  }
  return push->leds.get_global_led_brightness();
}

void libpush_set_led_pwm_freq(int freq) {
  if (!push) {
    cerr << NOT_CONNECTED_MSG << endl;
    return;
  }
  return push->leds.set_led_pwm_freq(freq);
}

void libpush_set_midi_mode(LibPushMidiMode mode) {
  if (!push) {
    cerr << NOT_CONNECTED_MSG << endl;
    return;
  }
  push->misc.set_midi_mode(mode);
}

LibPushStats libpush_get_statistics(unsigned char run_id) {
  if (!push) {
    cerr << NOT_CONNECTED_MSG << endl;
    LibPushStats s;
    return s;
  }
  return push->misc.get_statistics(run_id);
}
