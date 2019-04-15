#pragma once
#include "MidiMessageHandlerFns.hpp"
#include "MidiMessageListener.hpp"
#include "MidiMsg.hpp"
#include "RtMidi.h"
#include "SysexInterface.hpp"
#include "push.h"
#include <iostream>
#include <memory>

/// A convenient interface to all Push functions that are accessed via Midi
///
/// Sysex messages are used for most of Push's functions.
/// Push also sends messages back to the host when the user
/// interacts with pads, buttons, or the touch strip.
/// These can be received by registering callback functions with this class.
class MidiInterface {
public:
  SysexInterface sysex;
  MidiMessageListener<LibPushPadEvent> pad_listener;
  MidiMessageListener<LibPushButtonEvent> button_listener;
  MidiMessageListener<LibPushEncoderEvent> encoder_listener;
  MidiMessageListener<LibPushTouchStripEvent> touch_strip_listener;

  MidiInterface();
  ~MidiInterface();

  /// \param port The MIDI port to connect to (Live or User)
  /// \effects Connect midi_in and midi_out to Push, setup callback for incoming MIDI
  /// \requires Not already connected
  /// \throws An [std::runtime_error]() exception if a connection can't be made
  void connect(LibPushPort port);

  /// \effects Clean up the MIDI input and output
  /// \requires Currently connected
  /// \throws An [std::runtime_error]() exception if not currently connected
  void disconnect();

private:
  std::unique_ptr<RtMidiIn> midi_in;
  std::unique_ptr<RtMidiOut> midi_out;

  /// Find the given MIDI port
  ///
  /// \param rtmidi A pointer to either an RtMidiIn or RtMidiOut object
  /// \param port The port to look for (Live or User)
  /// \returns The index of the port
  /// \throws An [std::runtime_error]() exception if the port is not found
  static int find_port(RtMidi *rtmidi, LibPushPort port);

  /// Handler called when a MIDI message is received from Push
  ///
  /// \param delta Time since the last message
  /// \param message The message bytes
  /// \param this_ptr A pointer to the instance of MidiInterface that registered the callback
  /// \effects Calls appropriate handler method on the MidiInterface instance based on the message type
  static void handle_midi_input(double delta, midi_msg *message,
                                void *this_ptr);
};
