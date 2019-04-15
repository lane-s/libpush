#pragma once
#include "MidiMsg.hpp"
#include "RtMidi.h"
#include <future>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <unordered_map>
#include <unordered_set>

class SysexInterface {
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

  SysexInterface(std::unique_ptr<RtMidiOut> &midi_out);

private:
  std::unique_ptr<RtMidiOut> &midi_out;

  /// Stores a message queue for each type of command to hold the command's replies
  std::unordered_map<byte, std::queue<midi_msg>> sysex_reply_queues;
  std::mutex reply_queues_lock;

  /// Send a sysex command to Push
  ///
  /// \param command The command code (defined in the _Sysex enums)
  /// \param args The argument bytes for the command
  /// \returns The command's reply if it has one
  /// \effects Sends the sysex command to Push and blocks until a reply is received
  midi_msg sysex_call(byte command, midi_msg args);

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
