#include "MidiInterface.hpp"

using namespace std;

using LedSys = MidiInterface::LedSysex;
using PadSys = MidiInterface::PadSysex;
using TouchSys = MidiInterface::TouchStripSysex;
using PedalSys = MidiInterface::PedalSysex;
using MiscSys = MidiInterface::MiscSysex;
using byte_vec = MidiInterface::byte_vec;
using MidiMsgType = MidiInterface::MidiMsgType;

const string PUSH2_LIVE_PORT_NAME = "Ableton Push 2 Live Port";
const string PUSH2_USER_PORT_NAME = "Ableton Push 2 User Port";

/// Sequence of bytes that precedes every MIDI sysex message sent or received from Push
byte_vec SYSEX_PREFIX = {0xF0, 0x00, 0x21, 0x1D, 0x01, 0x01};
/// Byte marking the end of a sysex message
unsigned char SYSEX_SUFFIX = 0xF7;

/// Set of sysex commands that provide a reply
unordered_set<unsigned char> commands_with_reply = {
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

MidiInterface::MidiInterface() : midi_in(nullptr), midi_out(nullptr) {
  for (unsigned char command : commands_with_reply) {
    this->sysex_reply_queues[command] = queue<byte_vec>();
  }
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
  this->sysex_call(MiscSys::get_display_brightness, {});
  cout << "Brightness message sent" << endl;
}

int MidiInterface::find_port(RtMidi *rtmidi, LibPushPort port) {
  unsigned int port_count = rtmidi->getPortCount();
  string port_name;
  for (unsigned int i = 0; i < port_count; ++i) {
    port_name = rtmidi->getPortName(i);
    if ((port_name == PUSH2_LIVE_PORT_NAME && port == LibPushPort::LIVE) ||
        (port_name == PUSH2_USER_PORT_NAME && port == LibPushPort::USER)) {
      return i;
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

byte_vec MidiInterface::sysex_call(unsigned char command, byte_vec args) {
  if (!this->midi_in || !this->midi_out) {
    throw runtime_error("Can't make sysex call when disconnected");
  }

  byte_vec message(SYSEX_PREFIX);
  message.push_back(command);
  message.insert(message.end(), args.begin(), args.end());
  message.push_back(SYSEX_SUFFIX);
  this->midi_out->sendMessage(&message);

  byte_vec reply;
  if (commands_with_reply.count(command)) {
    reply = this->get_sysex_reply(command);

    for (int i : reply) {
      cout << hex << i;
      cout << " ";
    }
    cout << endl;
  }
  return reply;
}

void MidiInterface::handle_midi_input(double delta, byte_vec *message,
                                      void *this_ptr) {
  MidiInterface *self = static_cast<MidiInterface *>(this_ptr);
  for (int i : *message) {
    cout << hex << i;
    cout << " ";
  }
  cout << endl;

  unsigned char msg_type = *message->begin();
  switch (msg_type) {
  case MidiMsgType::sysex:
    self->handle_sysex_message(message);
    break;
  case MidiMsgType::note_on:
    self->handle_note_on_message(message);
    break;
  case MidiMsgType::note_off:
    self->handle_note_off_message(message);
    break;
  case MidiMsgType::aftertouch:
    self->handle_aftertouch_message(message);
    break;
  case MidiMsgType::cc:
    self->handle_cc_message(message);
    break;
  case MidiMsgType::pitch_bend:
    self->handle_pitch_bend_message(message);
    break;
  }
}

byte_vec MidiInterface::get_sysex_reply(unsigned char command) {
  promise<byte_vec> p;
  auto f = p.get_future();
  thread listener(&poll_for_sysex_reply, command, move(p), this);
  listener.join();
  return f.get();
}

void MidiInterface::poll_for_sysex_reply(unsigned char command,
                                         std::promise<byte_vec> p,
                                         MidiInterface* self) {
  // Poll until a message is queued for the given command
  while (1) {
    unique_lock<mutex> lock(self->reply_queues_lock);

    auto iter = self->sysex_reply_queues.find(command);
    if (iter != self->sysex_reply_queues.end()) {
      queue<byte_vec> &queue = iter->second;
      if (!queue.empty()) {
        p.set_value(queue.front());
        queue.pop();
        lock.unlock();
        break;
      }
    }
  }
}

void MidiInterface::handle_sysex_message(byte_vec *message) {
  // Put the message args on the reply queue for the command
  auto prefix_end = message->begin() + SYSEX_PREFIX.size();
  unsigned char command = *prefix_end;
  if (commands_with_reply.count(command)) {
    byte_vec args(prefix_end + 1, message->end() - 1);
    lock_guard<mutex> lock(this->reply_queues_lock);
    this->sysex_reply_queues[command].push(args);
  }
}

void MidiInterface::handle_note_on_message(byte_vec *message) {}

void MidiInterface::handle_note_off_message(byte_vec *message) {}

void MidiInterface::handle_aftertouch_message(byte_vec *message) {}

void MidiInterface::handle_cc_message(byte_vec *message) {}

void MidiInterface::handle_pitch_bend_message(byte_vec *message) {}

MidiInterface::~MidiInterface() {
  if (this->midi_in && this->midi_out) {
    this->disconnect();
  }
}
