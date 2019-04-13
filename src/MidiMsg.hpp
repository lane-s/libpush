#ifndef MIDI_MSG_TYPE_H
#define MIDI_MSG_TYPE_H

#include <vector>

enum MidiMsgType : unsigned char {
  sysex = 0xF0,
  note_off = 0x80,
  note_on = 0x90,
  aftertouch = 0xA0,
  cc = 0xB0,
  pitch_bend = 0xE0,
};

using byte = unsigned char;
using midi_msg = std::vector<byte>;
#endif
