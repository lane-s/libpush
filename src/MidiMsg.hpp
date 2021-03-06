#pragma once
#include <string>
#include <vector>

enum MidiMsgType : unsigned char {
  sysex = 0xF0,
  note_off = 0x80,
  note_on = 0x90,
  aftertouch = 0xA0,
  cc = 0xB0,
  channel_pressure = 0xD0,
  pitch_bend = 0xE0,
};

using uint = unsigned int;
using byte = unsigned char;
using midi_msg = std::vector<byte>;

byte get_midi_type(midi_msg &message);
uint get_midi_number(byte msg_type, midi_msg &message);
uint get_midi_value(byte msg_type, midi_msg &message);
