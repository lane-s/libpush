#include "MidiInterface.hpp"

using namespace std;

const string COMMON_PORT_NAME = "Ableton Push 2";
const vector<string> USER_PORT_STRINGS = {":1", "MIDI", "User"};

MidiInterface::MidiInterface()
    : midi_in(nullptr), midi_out(nullptr), pad_listener(pad_message_handler_fn),
      button_listener(button_message_handler_fn),
      touch_strip_listener(touch_strip_message_handler_fn) {
  this->register_handler(&pad_listener);
  this->register_handler(&button_listener);
  this->register_handler(&touch_strip_listener);
}

void MidiInterface::connect(LibPushPort port) {
  if (this->midi_in || this->midi_out) {
    throw runtime_error("Can't connect to Push midi port if already connected");
  }

  this->midi_in = make_unique<RtMidiIn>();
  this->midi_out = make_unique<RtMidiOut>();

  int in_port = MidiInterface::find_port(this->midi_in.get(), port);
  int out_port = MidiInterface::find_port(this->midi_out.get(), port);

  this->midi_in->openPort(in_port);
  this->midi_in->setCallback(&MidiInterface::handle_midi_input, this);
  this->midi_in->ignoreTypes(false, true, true); // Don't ignore sysex messages

  this->midi_out->openPort(out_port);
}

void MidiInterface::register_handler(MidiMessageHandler *handler) {
  this->handlers.push_back(handler);
}

void MidiInterface::send_message(midi_msg &message) {
  if (!this->midi_out) {
    throw runtime_error("Can't send midi message with no connected output");
  }
  this->midi_out->sendMessage(&message);
}

bool string_contains_any_substring(string s, vector<string> substrings) {
  for (const auto &substr : substrings) {
    if (s.find(substr) != string::npos) {
      return true;
    }
  }
  return false;
}

int MidiInterface::find_port(RtMidi *rtmidi, LibPushPort port) {
  unsigned int port_count = rtmidi->getPortCount();
  string port_name;
  for (unsigned int i = 0; i < port_count; ++i) {
    port_name = rtmidi->getPortName(i);
    if (port_name.find(COMMON_PORT_NAME) != string::npos) {
      if (port == LibPushPort::USER &&
          string_contains_any_substring(port_name, USER_PORT_STRINGS)) {
        return i;
      } else if (port == LibPushPort::LIVE &&
                 !string_contains_any_substring(port_name, USER_PORT_STRINGS)) {
        return i;
      }
    }
  }

  throw runtime_error(
      "Can't find Push midi inputs and outputs for chosen port");
}

void MidiInterface::disconnect() {
  if (!this->midi_in || !this->midi_out) {
    throw runtime_error(
        "Can't disconnect from Push midi port unless already connected");
  }

  this->midi_in.reset(nullptr);
  this->midi_out.reset(nullptr);
}

void MidiInterface::handle_midi_input(double delta, midi_msg *message,
                                      void *this_ptr) {
  MidiInterface *self = static_cast<MidiInterface *>(this_ptr);

  try {
    for (const auto &handler : self->handlers) {
      handler->handle_message(*message);
    }
  } catch (exception &ex) {
    cerr << "Exception on MIDI thread: " << ex.what() << endl;
  }
}

MidiInterface::~MidiInterface() {
  if (this->midi_in && this->midi_out) {
    this->disconnect();
  }
}
