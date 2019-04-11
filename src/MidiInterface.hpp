#include "RtMidi.h"
#include <iostream>
#include <string>
#include <unordered_set>

using namespace std;

class MidiInterface {
public:
  enum Port {
    LIVE,
    USER
  };

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

  MidiInterface();
  ~MidiInterface();

  void connect(Port port);
  void disconnect();
  vector<unsigned char> sysex_call(unsigned char command, vector<unsigned char> args);
private:
  RtMidiIn* midi_in = NULL;
  RtMidiOut* midi_out = NULL;

  static unordered_set<unsigned char> commands_with_reply;
  static int find_port(RtMidi* rtmidi, Port port);

  void check_connection();
};
