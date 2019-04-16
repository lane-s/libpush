#include "PadInterface.hpp"

using namespace std;

PadInterface::PadInterface(MidiInterface &midi, SysexInterface &sysex,
                           LedInterface &leds)
    : sysex(sysex), leds(leds), listener(PadInterface::handle_message) {
  midi.register_handler(&this->listener);
  sysex.register_command_with_reply(PadSysex::GET_AFTERTOUCH_MODE);
  sysex.register_command_with_reply(PadSysex::GET_SELECTED_PAD_SETTINGS);
}

void PadInterface::register_callback(LibPushPadCallback cb, void *context) {
  this->listener.register_callback(cb, context);
}

void PadInterface::set_global_aftertouch_range(unsigned short low,
                                               unsigned short high) {
  midi_msg args({0, 0, 0, 0, (byte)(low & 0x7F), (byte)(low >> 7),
                 (byte)(high & 0x7F), (byte)(high >> 7)});
  this->sysex.sysex_call(PadSysex::SET_PAD_PARAMETERS, args);
}

void PadInterface::set_global_aftertouch_mode(LibPushAftertouchMode mode) {
  byte mode_byte = static_cast<byte>(mode);
  midi_msg args({mode_byte});
  this->sysex.sysex_call(PadSysex::SET_AFTERTOUCH_MODE, args);
}

LibPushAftertouchMode PadInterface::get_global_aftertouch_mode() {
  LibPushAftertouchMode mode;
  midi_msg args;
  midi_msg reply = this->sysex.sysex_call(PadSysex::GET_AFTERTOUCH_MODE, args);
  return static_cast<LibPushAftertouchMode>(reply[0]);
}

constexpr byte CURVE_STEP = LIBPUSH_PAD_VELOCITY_CURVE_ENTRIES / 16;
void PadInterface::set_global_pad_velocity_curve(
    byte (&entries)[LIBPUSH_PAD_VELOCITY_CURVE_ENTRIES]) {
  for (byte i = 0; i < LIBPUSH_PAD_VELOCITY_CURVE_ENTRIES; i += CURVE_STEP) {
    midi_msg args({i});
    for (byte j = i; j < i + CURVE_STEP; ++j) {
      args.push_back(entries[j]);
    }
    sysex.sysex_call(PadSysex::SET_PAD_VELOCITY_CURVE_ENTRY, args);
  }
}

void PadInterface::set_pad_sensitivity(byte x, byte y,
                                       LibPushPadSensitivity sensitivity) {
  x += 1;
  y = LIBPUSH_PAD_MATRIX_DIM - y;
  midi_msg args({y, x, static_cast<byte>(sensitivity)});
  sysex.sysex_call(PadSysex::SELECT_PAD_SETTINGS, args);
}

void PadInterface::set_global_pad_sensitivity(
    LibPushPadSensitivity sensitivity) {
  midi_msg args({0, 0, static_cast<byte>(sensitivity)});
  sysex.sysex_call(PadSysex::SELECT_PAD_SETTINGS, args);
}

LibPushPadSensitivity PadInterface::get_pad_sensitivity(byte x, byte y) {
  x += 1;
  y = LIBPUSH_PAD_MATRIX_DIM - y;
  midi_msg args({y, x});
  midi_msg reply = sysex.sysex_call(PadSysex::GET_SELECTED_PAD_SETTINGS, args);
  return static_cast<LibPushPadSensitivity>(reply[2]);
}

void PadInterface::set_pad_color(byte x, byte y, uint color_index) {
  uint n = pad_coordinates_to_number(x, y);
  LibPushLedAnimation anim;
  anim.type = LibPushLedAnimationType::LP_NO_TRANSITION;
  anim.duration = LibPushLedAnimationDuration::LP_24TH;
  this->leds.set_led_color(MidiMsgType::note_on, n, anim, color_index);
}

void PadInterface::set_global_pad_color(uint color_index) {
  for (byte x = 0; x < LIBPUSH_PAD_MATRIX_DIM; ++x) {
    for (byte y = 0; y < LIBPUSH_PAD_MATRIX_DIM; ++y) {
      this->set_pad_color(x, y, color_index);
    }
  }
}

void PadInterface::set_pad_animation(byte x, byte y, uint color_index,
                                     LibPushLedAnimation anim) {
  uint n = pad_coordinates_to_number(x, y);
  this->leds.set_led_color(MidiMsgType::note_on, n, anim, color_index);
}

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

uint PadInterface::pad_coordinates_to_number(byte x, byte y) {
  return (LIBPUSH_PAD_MATRIX_DIM - 1 - y) * LIBPUSH_PAD_MATRIX_DIM + x +
         FIRST_PAD_N;
}
