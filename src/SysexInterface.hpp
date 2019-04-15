#pragma once
#include "MidiMsg.hpp"
#include "RtMidi.h"
#include "push.h"
#include <future>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <unordered_map>
#include <unordered_set>

/// Responsible for sending and handling sysex MIDI messages
class SysexInterface {
public:
  enum PadSysex : byte {
    SET_PAD_PARAMETERS = 0x1B,
    SET_AFTERTOUCH_MODE = 0x1E,
    GET_AFTERTOUCH_MODE = 0x1F,
    SET_PAD_VELOCITY_CURVE_ENTRY = 0x20,
    GET_PAD_VELOCITY_CURVE_ENTRY = 0x21,
    SELECT_PAD_SETTINGS = 0x28,
    GET_SELECTED_PAD_SETTINGS = 0x29
  };

  enum TouchStripSysex : byte {
    SET_TOUCH_STRIP_CONFIGURATION = 0x17,
    GET_TOUCH_STRIP_CONFIGURATION = 0x18,
    SET_TOUCH_STRIP_LEDS = 0x19
  };

  enum PedalSysex : byte {
    SAMPLE_PEDAL_DATA = 0x0C,
    CONFIGURE_PEDAL = 0x30,
    SET_PEDAL_CURVE_LIMITS = 0x31,
    SET_PEDAL_CURVE_ENTRIES = 0x32
  };

  enum MiscSysex : byte {
    SET_DISPLAY_BRIGHTNESS = 0x08,
    GET_DISPLAY_BRIGHTNESS = 0x09,
    SET_MIDI_MODE = 0x0A,
    REQUEST_STATISTICS = 0x1A,
  };

  SysexInterface(std::unique_ptr<RtMidiOut> &midi_out);

  /// Send a sysex command to Push
  ///
  /// \param command The command code (defined in the _Sysex enums)
  /// \param args The argument bytes for the command
  /// \returns The command's reply if it has one
  /// \effects Sends the sysex command to Push and blocks until a reply is received
  midi_msg sysex_call(byte command, midi_msg args);

  /// Register a command that expects a reply
  ///
  /// \params The command byte code
  /// \effects Listens for and returns a reply for the given command type when a sysex_call is made for that type
  void register_command_with_reply(byte command);

private:
  std::unique_ptr<RtMidiOut> &midi_out;

  /// Stores a message queue for each type of command to hold the command's replies
  std::unordered_map<byte, std::queue<midi_msg>> sysex_reply_queues;
  std::mutex reply_queues_lock;
  std::unordered_set<byte> commands_with_reply;

  /// Blocks until a reply is received for a sysex command
  ///
  /// \param command The command code that is waiting for a reply
  /// \returns The data bytes of the command's reply
  /// \effects Creates a thread to poll for a reply, blocks until it is received
  midi_msg get_sysex_reply(byte command);

  /// Polls for a reply for a sysex command
  ///
  /// \param command The command code that is waiting for a reply
  /// \param p A promise to hold the reply's data bytes
  /// \param self A pointer to the MidiInterface object that is requesting the polling
  static void poll_for_sysex_reply(byte command, std::promise<midi_msg> p,
                                   SysexInterface *self);

  void handle_sysex_message(midi_msg &message);

  // MidiInterface needs to call handle_sysex_message,
  // but that method shouldn't be public
  friend class MidiInterface;
};
