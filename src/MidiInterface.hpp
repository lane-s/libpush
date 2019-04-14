#include "MidiMessageHandlerFns.hpp"
#include "MidiMessageListener.hpp"
#include "MidiMsg.hpp"
#include "RtMidi.h"
#include "push.h"
#include <future>
#include <iostream>
#include <memory>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <unordered_map>
#include <unordered_set>

/// A convenient interface to all Push functions that are accessed via Midi
///
/// Sysex messages are used for most of Push's functions.
/// Push also sends messages back to the host when the user
/// interacts with pads, buttons, or the touch strip.
/// These can be received by registering callback functions with this class.
class MidiInterface {
public:
  enum LedSysex : byte {
    set_led_color_palette_entry = 0x03,
    get_led_color_palette_entry = 0x04,
    reapply_color_palette = 0x05,
    set_led_brightness = 0x06,
    get_led_brightness = 0x07,
    set_led_pwm_freq_correction = 0x0B,
    set_led_white_balance = 0x14,
    get_led_white_balance = 0x15
  };

  enum PadSysex : byte {
    set_pad_parameters = 0x1B,
    set_aftertouch_mode = 0x1E,
    get_aftertouch_mode = 0x1F,
    set_pad_velocity_curve_entry = 0x20,
    get_pad_velocity_curve_entry = 0x21,
    select_pad_settings = 0x28,
    get_selected_pad_settings = 0x29
  };

  enum TouchStripSysex : byte {
    set_touch_strip_configuration = 0x17,
    get_touch_strip_configuration = 0x18,
    set_touch_strip_leds = 0x19
  };

  enum PedalSysex : byte {
    sample_pedal_data = 0x0C,
    configure_pedal = 0x30,
    set_pedal_curve_limits = 0x31,
    set_pedal_curve_entries = 0x32
  };

  enum MiscSysex : byte {
    set_display_brightness = 0x08,
    get_display_brightness = 0x09,
    set_midi_mode = 0x0A,
    request_statistics = 0x1A,
  };

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

  /// Send a sysex command to Push
  ///
  /// \param command The command code (defined in the _Sysex enums)
  /// \param args The argument bytes for the command
  /// \returns The command's reply if it has one
  /// \effects Sends the sysex command to Push and blocks until a reply is received
  midi_msg sysex_call(byte command, midi_msg args);

private:
  std::unique_ptr<RtMidiIn> midi_in;
  std::unique_ptr<RtMidiOut> midi_out;

  /// Stores a message queue for each type of command to hold the command's replies
  std::unordered_map<byte, std::queue<midi_msg>> sysex_reply_queues;
  std::mutex reply_queues_lock;

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
                                   MidiInterface *self);

  void handle_sysex_message(midi_msg &message);
};
