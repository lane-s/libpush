#pragma once
#include "MidiInterface.hpp"
#include "MidiMessageHandler.hpp"
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
class SysexInterface : public MidiMessageHandler {
public:
  SysexInterface(MidiInterface &midi);

  /// Send a sysex command to Push
  ///
  /// \param command The command code (defined in the _Sysex enums)
  /// \param args The argument bytes for the command
  /// \returns The command's reply if it has one
  /// \effects Sends the sysex command to Push and blocks until a reply is received
  midi_msg sysex_call(byte command, midi_msg &args);

  /// Register a command that expects a reply
  ///
  /// \params The command byte code
  /// \effects Listens for and returns a reply for the given command type when a sysex_call is made for that type
  void register_command_with_reply(byte command);

private:
  MidiInterface &midi;

  /// Stores a message queue for each type of command to hold the command's replies
  std::unordered_map<byte, std::queue<midi_msg>> sysex_reply_queues;
  std::mutex reply_queues_lock;
  std::unordered_set<byte> commands_with_reply;

  /// Blocks until a reply is received for a sysex command
  ///
  /// \param command The command code that is waiting for a reply
  /// \returns The data bytes (arguments) of the command's reply
  /// \effects Creates a thread to poll for a reply, blocks until it is received
  midi_msg get_sysex_reply(byte command);

  /// Polls for a reply for a sysex command
  ///
  /// \param command The command code that is waiting for a reply
  /// \param p A promise to hold the reply's data bytes
  /// \param self A pointer to the MidiInterface object that is requesting the polling
  static void poll_for_sysex_reply(byte command, std::promise<midi_msg> p,
                                   SysexInterface *self);

  void handle_message(midi_msg &message) override;

  // MidiInterface needs to call handle_sysex_message,
  // but that method shouldn't be public
  friend class MidiInterface;
};
