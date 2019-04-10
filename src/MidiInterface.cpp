#include "MidiInterface.hpp"

using namespace std;
using Port = MidiInterface::Port;

const std::string PUSH2_LIVE_PORT_NAME = "Ableton Push 2 Live Port";
const std::string PUSH2_USER_PORT_NAME = "Ableton Push 2 User Port";
std::vector<unsigned char> SYSEX_PREFIX = {0xF0, 0x00, 0x21, 0x1D, 0x01, 0x01};
unsigned char SYSEX_SUFFIX = 0xF7;

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

MidiInterface::MidiInterface() {
}

void MidiInterface::connect(Port port) {
  if(this->midi_in || this->midi_out) {
    throw runtime_error("Can't connect to Push midi port if already connected");
  }

  this->midi_in = new RtMidiIn();
  this->midi_out = new RtMidiOut();

  int in_port = MidiInterface::find_port(this->midi_in, port);
  int out_port = MidiInterface::find_port(this->midi_out, port);

  if (in_port == -1 || out_port == -1) {
    throw runtime_error("Can't find Push midi inputs and outputs for chosen port");
  }

  this->midi_in->openPort(in_port);
  this->midi_out->openPort(out_port);

  // TODO Remove
  // Send a test sysex message
  vector<unsigned char> message(SYSEX_PREFIX);
  message.push_back(MiscSysex::set_display_brightness);
  message.insert(message.end(), {0x00, 0x00}); //Turn off display
  message.push_back(SYSEX_SUFFIX);
  this->midi_out->sendMessage(&message);
  cout << "Brightness message sent" << endl;
  for (int i : message) {
    cout << hex << i;
    cout << " ";
  }
  cout << endl;
}

int MidiInterface::find_port(RtMidi* rtmidi, Port port) {
  unsigned int port_count = rtmidi->getPortCount();
  string port_name;
  for(unsigned int i = 0; i < port_count; ++i) {
    port_name = rtmidi->getPortName(i);
    if ((port_name == PUSH2_LIVE_PORT_NAME && port == Port::LIVE) ||
        (port_name == PUSH2_USER_PORT_NAME && port == Port::USER)) {
      return i;
    }
  }

  return -1;
}

void MidiInterface::disconnect() {
  if(!this->midi_in || !this->midi_out) {
    throw runtime_error("Can't disconnect from Push midi port unless already connected");
  }

  delete this->midi_in;
  delete this->midi_out;
}

MidiInterface::~MidiInterface() {
  if(this->midi_in && this->midi_out) {
    this->disconnect();
  }
}
