#include "MidiInterface.hpp"

using namespace std;

using Port = MidiInterface::Port;
using LedSys = MidiInterface::LedSysex;
using PadSys = MidiInterface::PadSysex;
using TouchSys = MidiInterface::TouchStripSysex;
using PedalSys = MidiInterface::PedalSysex;
using MiscSys = MidiInterface::MiscSysex;

using byteVec = vector<unsigned char>;

const string PUSH2_LIVE_PORT_NAME = "Ableton Push 2 Live Port";
const string PUSH2_USER_PORT_NAME = "Ableton Push 2 User Port";
byteVec SYSEX_PREFIX = {0xF0, 0x00, 0x21, 0x1D, 0x01, 0x01};
unsigned char SYSEX_SUFFIX = 0xF7;

// Set of sysex commands that provide a reply
unordered_set<unsigned char> MidiInterface::commands_with_reply = {
  LedSys::get_led_color_palette_entry,
  LedSys::get_led_brightness,
  LedSys::get_led_white_balance,
  PadSys::get_aftertouch_mode,
  PadSys::get_pad_velocity_curve_entry,
  PadSys::get_selected_pad_settings,
  PedalSys::sample_pedal_data,
  MiscSys::get_display_brightness,
  MiscSys::set_midi_mode,
};

MidiInterface::MidiInterface() : midi_in(NULL), midi_out(NULL) {
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
  this->sysex_call(MiscSys::set_display_brightness, {0x7F, 0x01});
  cout << "Brightness message sent" << endl;
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

byteVec MidiInterface::sysex_call(unsigned char command, byteVec args) {
  if(!this->midi_in || !this->midi_out) {
    throw runtime_error("Can't make sysex call when disconnected");
  }

  byteVec message(SYSEX_PREFIX);
  message.push_back(command);
  message.insert(message.end(), args.begin(), args.end());
  message.push_back(SYSEX_SUFFIX);
  this->midi_out->sendMessage(&message);

  for (int i : message) {
    cout << hex << i;
    cout << " ";
  }
  cout << endl;

  byteVec reply;

  if (commands_with_reply.count(command)) {
    cout << "Waiting for command reply" << endl;
    // TODO Block until reply is received by midi_in
  }

  return reply;
}

MidiInterface::~MidiInterface() {
  if(this->midi_in && this->midi_out) {
    this->disconnect();
  }
}
