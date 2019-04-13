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
  enum LedSysex : unsigned char {
    set_led_color_palette_entry = 0x03,
    get_led_color_palette_entry = 0x04,
    reapply_color_palette = 0x05,
    set_led_brightness = 0x06,
    get_led_brightness = 0x07,
    set_led_pwm_freq_correction = 0x0B,
    set_led_white_balance = 0x14,
    get_led_white_balance = 0x15
  };

  enum PadSysex : unsigned char {
    set_pad_parameters = 0x1B,
    set_aftertouch_mode = 0x1E,
    get_aftertouch_mode = 0x1F,
    set_pad_velocity_curve_entry = 0x20,
    get_pad_velocity_curve_entry = 0x21,
    select_pad_settings = 0x28,
    get_selected_pad_settings = 0x29
  };

  enum TouchStripSysex : unsigned char {
    set_touch_strip_configuration = 0x17,
    get_touch_strip_configuration = 0x18,
    set_touch_strip_leds = 0x19
  };

  enum PedalSysex : unsigned char {
    sample_pedal_data = 0x0C,
    configure_pedal = 0x30,
    set_pedal_curve_limits = 0x31,
    set_pedal_curve_entries = 0x32
  };

  enum MiscSysex : unsigned char {
    set_display_brightness = 0x08,
    get_display_brightness = 0x09,
    set_midi_mode = 0x0A,
    request_statistics = 0x1A,
  };

  enum MidiMsgType {
    unknown = -1,
    sysex = 0,
    note_on = 1,
    note_off = 2,
    cc = 3
  };

  using ByteVec = std::vector<unsigned char>;

  MidiInterface();
  ~MidiInterface();

  void connect(LibPushPort port);
  void disconnect();
  ByteVec sysex_call(unsigned char command, ByteVec args);

private:
  std::unique_ptr<RtMidiIn> midi_in;
  std::unique_ptr<RtMidiOut> midi_out;

  std::unordered_map<unsigned char, std::queue<ByteVec>> sysex_reply_queues;
  std::mutex reply_queues_lock;

  static int find_port(RtMidi *rtmidi, LibPushPort port);
  static void handle_midi_input(double delta, ByteVec *message, void *this_ptr);
  static MidiMsgType get_midi_message_type(ByteVec *message);

  ByteVec get_sysex_reply(unsigned char command);
  static void poll_for_sysex_reply(unsigned char command,
                                   std::promise<ByteVec> p,
                                   MidiInterface *self);

  void handle_sysex_message(ByteVec *message);
  void handle_note_on_message(ByteVec *message);
  void handle_note_off_message(ByteVec *message);
  void handle_cc_message(ByteVec *message);

};
