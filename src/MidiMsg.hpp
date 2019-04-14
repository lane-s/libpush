#ifndef MIDI_MSG_TYPE_H
#define MIDI_MSG_TYPE_H

#include <vector>
#include <string>

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

uint get_midi_number(byte msg_type, midi_msg &message);
uint get_midi_value(byte msg_type, midi_msg &message);
#endif
