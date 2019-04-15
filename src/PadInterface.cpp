#include "PadInterface.hpp"

using namespace std;

PadInterface::PadInterface(MidiInterface &midi, SysexInterface &sysex,
                           LedInterface &leds)
    : sysex(sysex), leds(leds), listener(PadInterface::handle_message) {
  midi.register_handler(&this->listener);
}

void PadInterface::register_callback(LibPushPadCallback cb, void *context) {
  this->listener.register_callback(cb, context);
}

void PadInterface::set_global_aftertouch_range(unsigned short low,
                                               unsigned short high) {}

void PadInterface::set_global_aftertouch_mode(LibPushAftertouchMode mode) {}

LibPushAftertouchMode PadInterface::get_global_aftertouch_mode() {
  LibPushAftertouchMode b;

  return b;
}

void PadInterface::set_global_pad_velocity_curve(
    byte (&entries)[LIBPUSH_PAD_VELOCITY_CURVE_ENTRIES]) {}

void PadInterface::set_pad_sensitivity(byte x, byte y,
                                       LibPushPadSensitivity sensitivity) {}

void PadInterface::set_global_pad_sensitivity(
    LibPushPadSensitivity sensitivity) {}

LibPushPadSensitivity PadInterface::get_pad_sensitivity(byte x, byte y) {
  LibPushPadSensitivity b;

  return b;
}

void PadInterface::set_pad_color(byte x, byte y, uint color_index) {}

void PadInterface::set_pad_animation(byte x, byte y, uint color_index,
                                     LibPushLedAnimation anim) {}

constexpr uint FIRST_PAD_N = 36;

unique_ptr<LibPushPadEvent> PadInterface::handle_message(byte msg_type,
                                                         midi_msg &message) {
  LibPushPadEventType event_type;
  switch (msg_type) {
  case MidiMsgType::note_on:
    event_type = LibPushPadEventType::LP_PAD_PRESSED;
    break;
  case MidiMsgType::note_off:
    event_type = LibPushPadEventType::LP_PAD_RELEASED;
    break;
  case MidiMsgType::channel_pressure:
  case MidiMsgType::aftertouch:
    event_type = LibPushPadEventType::LP_PAD_AFTERTOUCH;
    break;
  default:
    return nullptr;
  }
  int pad_number = get_midi_number(msg_type, message);
  if (pad_number < FIRST_PAD_N ||
      pad_number >=
          FIRST_PAD_N + LIBPUSH_PAD_MATRIX_DIM * LIBPUSH_PAD_MATRIX_DIM) {
    return nullptr;
  }

  unique_ptr<LibPushPadEvent> event = make_unique<LibPushPadEvent>();
  event->event_type = event_type;
  event->velocity = get_midi_value(msg_type, message);

  auto pad_coords = pad_number_to_coordinates(pad_number);
  event->x = get<0>(pad_coords);
  event->y = get<1>(pad_coords);

  return event;
}

tuple<uint, uint> PadInterface::pad_number_to_coordinates(uint n) {
  uint x = (n - FIRST_PAD_N) % LIBPUSH_PAD_MATRIX_DIM;
  uint y = (n - FIRST_PAD_N) / LIBPUSH_PAD_MATRIX_DIM;
  y = (LIBPUSH_PAD_MATRIX_DIM - 1) - y; //y goes from top to bottom
  return make_tuple(x, y);
}

void test_fn() { foo(); }
