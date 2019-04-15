#include "LedInterface.hpp"

using namespace std;

LedInterface::LedInterface(MidiInterface &midi, SysexInterface &sysex)
    : sysex(sysex), midi(midi) {
  sysex.register_command_with_reply(LedSysex::GET_LED_COLOR_PALETTE_ENTRY);
  sysex.register_command_with_reply(LedSysex::GET_LED_BRIGHTNESS);
  sysex.register_command_with_reply(LedSysex::GET_LED_WHITE_BALANCE);
}

void LedInterface::set_led_color_palette_entry(byte color_index,
                                               LibPushLedColor color) {
  midi_msg args({color_index});

  args.push_back(color.r & 0x7F);
  args.push_back(color.r >> 7);
  args.push_back(color.g & 0x7F);
  args.push_back(color.g >> 7);
  args.push_back(color.b & 0x7F);
  args.push_back(color.b >> 7);
  args.push_back(color.w & 0x7F);
  args.push_back(color.w >> 7);

  sysex.sysex_call(LedSysex::SET_LED_COLOR_PALETTE_ENTRY, args);
}

LibPushLedColor LedInterface::get_led_color_palette_entry(byte color_index) {
  LibPushLedColor color;
  midi_msg args({color_index});
  midi_msg reply =
      sysex.sysex_call(LedSysex::GET_LED_COLOR_PALETTE_ENTRY, args);
  // Color data starts at second byte of reply args
  color.r = (reply[1] & 0x7F) | (reply[2] << 7);
  color.g = (reply[3] & 0x7F) | (reply[4] << 7);
  color.b = (reply[5] & 0x7F) | (reply[6] << 7);
  color.w = (reply[7] & 0x7F) | (reply[8] << 7);
  return color;
}

void LedInterface::reapply_color_palette() {
  midi_msg args;
  sysex.sysex_call(LedSysex::REAPPLY_COLOR_PALETTE, args);
}

void LedInterface::set_global_led_brightness(byte brightness) {
  midi_msg args({brightness});
  sysex.sysex_call(LedSysex::SET_LED_BRIGHTNESS, args);
}

byte LedInterface::get_global_led_brightness() {
  midi_msg args;
  midi_msg reply = sysex.sysex_call(LedSysex::GET_LED_BRIGHTNESS, args);
  return reply[0];
}

void LedInterface::set_led_pwm_freq(int freq) {
  midi_msg args;
  // Calculate correction factor from frequency according to formula
  // found in push interface documentation
  uint correction_factor = (5000000 / freq) - 42752;
  correction_factor &= 0x1FFFFF; // Keep only the first 21 bits

  uint low_seven = correction_factor & 0x7F;
  uint mid_seven = (correction_factor & 0x3F80) >> 7;
  uint high_seven = (correction_factor & 0x1FC000) >> 14;

  args.push_back(low_seven);
  args.push_back(mid_seven);
  args.push_back(high_seven);
  sysex.sysex_call(LedSysex::SET_LED_PWM_FREQ_CORRECTION, args);
}

void LedInterface::set_led_white_balance(LedColorGroup color_group,
                                         unsigned short balance_factor) {
  midi_msg args({color_group});
  balance_factor &= 0x7FF; // Keep only the first 11 bits
  args.push_back(balance_factor & 0x7F);
  args.push_back(balance_factor >> 7);
  sysex.sysex_call(LedSysex::SET_LED_WHITE_BALANCE, args);
}

unsigned short LedInterface::get_led_white_balance(LedColorGroup color_group) {
  midi_msg args({color_group});
  midi_msg reply = sysex.sysex_call(LedSysex::GET_LED_WHITE_BALANCE, args);
  return (reply[0] | (reply[1] << 7));
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
