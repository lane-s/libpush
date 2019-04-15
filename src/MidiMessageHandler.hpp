#pragma once
#include "MidiMsg.hpp"

/// An interface for midi message handlers like SysexInterface, PadInterface, etc
class MidiMessageHandler {
public:
  /// \param message The incoming MIDI message
  /// \effects Determined by implementation
  virtual void handle_message(midi_msg &message) = 0;
};
