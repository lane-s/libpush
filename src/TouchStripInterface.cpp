#include "TouchStripInterface.hpp"

using namespace std;

TouchStripInterface::TouchStripInterface(MidiInterface &midi,
                                         SysexInterface &sysex)
    : sysex(sysex), listener(TouchStripInterface::handle_message) {
  midi.register_handler(&this->listener);
  sysex.register_command_with_reply(
      TouchStripSysex::GET_TOUCH_STRIP_CONFIGURATION);
}

void TouchStripInterface::register_callback(LibPushTouchStripCallback cb,
                                            void *context) {
  this->listener.register_callback(cb, context);
}

void TouchStripInterface::set_config(LibPushTouchStripConfig cfg) {
  midi_msg args;
  byte cfg_byte = cfg.controlled_by_host;
  cfg_byte |= (cfg.led_point << 3);
  cfg_byte |= (cfg.bar_starts_at_center << 4);
  cfg_byte |= (cfg.autoreturn << 5);
  cfg_byte |= (cfg.autoreturn_to_center << 6);
  args.push_back(cfg_byte);
  sysex.sysex_call(TouchStripSysex::SET_TOUCH_STRIP_CONFIGURATION, args);
}

LibPushTouchStripConfig TouchStripInterface::get_config() {
  midi_msg args;
  midi_msg reply =
      sysex.sysex_call(TouchStripSysex::GET_TOUCH_STRIP_CONFIGURATION, args);

  LibPushTouchStripConfig cfg;
  cfg.controlled_by_host = 0x01 & reply[0];
  cfg.led_point = 0x01 & (reply[0] >> 3);
  cfg.bar_starts_at_center = 0x01 & (reply[0] >> 4);
  cfg.autoreturn = 0x01 & (reply[0] >> 5);
  cfg.autoreturn_to_center = 0x01 & (reply[0] >> 6);
  return cfg;
}

constexpr uint LEDS_PER_BYTE = 2;
void TouchStripInterface::set_leds(
    byte (&brightness)[LIBPUSH_TOUCH_STRIP_LEDS]) {
  midi_msg args;
  for (int i = 0; i < LIBPUSH_TOUCH_STRIP_LEDS; i += LEDS_PER_BYTE) {
    args.push_back((brightness[i] & 0x7) | ((brightness[i + 1] & 0x7) << 3));
  }
  sysex.sysex_call(TouchStripSysex::SET_TOUCH_STRIP_LEDS, args);
}

constexpr uint TOUCH_STRIP_NN = 12;
constexpr uint TOUCH_STRIP_CC = 1;

unique_ptr<LibPushTouchStripEvent>
TouchStripInterface::handle_message(byte msg_type, midi_msg &message) {
  if (msg_type != MidiMsgType::cc && msg_type != MidiMsgType::pitch_bend &&
      msg_type != MidiMsgType::note_on) {
    return nullptr;
  }

  if (msg_type == MidiMsgType::note_on &&
      get_midi_number(msg_type, message) != TOUCH_STRIP_NN) {
    return nullptr;
  }

  if (msg_type == MidiMsgType::cc &&
      get_midi_number(msg_type, message) != TOUCH_STRIP_CC) {
    return nullptr;
  }

  unique_ptr<LibPushTouchStripEvent> event =
      make_unique<LibPushTouchStripEvent>();
  uint val = get_midi_value(msg_type, message);

  switch (msg_type) {
  case MidiMsgType::note_on:
    event->event_type =
        val ? LibPushTouchStripEventType::LP_TOUCH_STRIP_PRESSED
            : LibPushTouchStripEventType::LP_TOUCH_STRIP_RELEASED;
    event->position = 0.0;
    break;
  case MidiMsgType::cc:
    event->event_type = LibPushTouchStripEventType::LP_TOUCH_STRIP_MOVED;
    event->position = (val - 64.0) / 64.0; //(0-128) -> (-1.0 - 1.0)
    break;
  case MidiMsgType::pitch_bend:
    event->event_type = LibPushTouchStripEventType::LP_TOUCH_STRIP_MOVED;
    // The least significant bit of the pitch value is the 7th bit of the second byte
    uint lsb = message[1] >> 6;
    uint complete_val = lsb | (val << 1);
    event->position = (complete_val - 128.0) / 128.0; //(0-256) -> (-1.0 - 1.0)
    break;
  }
  return event;
}
