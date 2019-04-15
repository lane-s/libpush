#pragma once
#include "MidiMsg.hpp"
#include "SysexInterface.hpp"

class MiscSysexInterface {
public:
  enum MiscSysex : byte {
    SET_MIDI_MODE = 0x0A,
    REQUEST_STATISTICS = 0x1A,
  };

  MiscSysexInterface(SysexInterface &sysex);

  void set_midi_mode(LibPushMidiMode mode);
  LibPushStats get_statistics(byte run_id);

private:
  SysexInterface &sysex;
};
