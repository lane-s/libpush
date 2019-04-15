#include "LedInterface.hpp"

using namespace std;

LedInterface::LedInterface(MidiInterface &midi, SysexInterface &sysex)
    : sysex(sysex), midi(midi) {
  sysex.register_command_with_reply(LedSysex::GET_LED_COLOR_PALETTE_ENTRY);
  sysex.register_command_with_reply(LedSysex::GET_LED_BRIGHTNESS);
  sysex.register_command_with_reply(LedSysex::GET_LED_WHITE_BALANCE);
}

void LedInterface::set_led_color_palette_entry(byte color_index,
                                               LibPushLedColor color) {}

LibPushLedColor LedInterface::get_led_color_palette_entry(byte color_index) {
  LibPushLedColor b;

  return b;
}

void LedInterface::reapply_color_palette() {}

void LedInterface::set_global_led_brightness(byte brightness) {}

byte LedInterface::get_global_led_brightness() {
  byte b;

  return b;
}

void LedInterface::set_led_pwm_freq(int freq) {}

void LedInterface::set_led_white_balance(LedColorGroup color_group,
                                         unsigned short balance_factor) {}

unsigned short LedInterface::get_led_white_balance(LedColorGroup color_group) {
  return 0;
}

void LedInterface::set_led_color(MidiMsgType msg_type, uint midi_number,
                                 LibPushLedAnimation animation,
                                 uint color_index) {
  midi_msg message;
  byte animation_byte = animation.type + animation.duration;
  message.push_back(msg_type | animation_byte);
  message.push_back(midi_number);
  message.push_back(color_index);
  this->midi.send_message(message);
}
