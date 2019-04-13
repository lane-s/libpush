#include "MidiInterface.hpp"

using namespace std;

using LedSys = MidiInterface::LedSysex;
using PadSys = MidiInterface::PadSysex;
using TouchSys = MidiInterface::TouchStripSysex;
using PedalSys = MidiInterface::PedalSysex;
using MiscSys = MidiInterface::MiscSysex;

const string PUSH2_LIVE_PORT_NAME = "Ableton Push 2 Live Port";
const string PUSH2_USER_PORT_NAME = "Ableton Push 2 User Port";

/// Sequence of bytes that precedes every MIDI sysex message sent or received from Push
midi_msg SYSEX_PREFIX = {0xF0, 0x00, 0x21, 0x1D, 0x01, 0x01};
/// Byte marking the end of a sysex message
byte SYSEX_SUFFIX = 0xF7;

/// Set of sysex commands that provide a reply
unordered_set<byte> commands_with_reply = {
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
    this->sysex_reply_queues[command] = queue<midi_msg>();
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

midi_msg MidiInterface::sysex_call(byte command, midi_msg args) {
  if (!this->midi_in || !this->midi_out) {
    throw runtime_error("Can't make sysex call when disconnected");
  }

  midi_msg message(SYSEX_PREFIX);
  message.push_back(command);
  message.insert(message.end(), args.begin(), args.end());
  message.push_back(SYSEX_SUFFIX);
  this->midi_out->sendMessage(&message);

  midi_msg reply;
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

void MidiInterface::handle_midi_input(double delta, midi_msg *message,
                                      void *this_ptr) {
  MidiInterface *self = static_cast<MidiInterface *>(this_ptr);
  for (int i : *message) {
    cout << hex << i;
    cout << " ";
  }
  cout << endl;

  byte msg_type = *message->begin();
  switch (msg_type) {
  case MidiMsgType::sysex:
    self->handle_sysex_message(message);
    break;
  default:
  }
}

midi_msg MidiInterface::get_sysex_reply(byte command) {
  promise<midi_msg> p;
  auto f = p.get_future();
  thread listener(&poll_for_sysex_reply, command, move(p), this);
  listener.join();
  return f.get();
}

void MidiInterface::poll_for_sysex_reply(byte command,
                                         std::promise<midi_msg> p,
                                         MidiInterface *self) {
  // Poll until a message is queued for the given command
  while (1) {
    unique_lock<mutex> lock(self->reply_queues_lock);

    auto iter = self->sysex_reply_queues.find(command);
    if (iter != self->sysex_reply_queues.end()) {
      queue<midi_msg> &queue = iter->second;
      if (!queue.empty()) {
        p.set_value(queue.front());
        queue.pop();
        lock.unlock();
        break;
      }
    }
  }
}

void MidiInterface::handle_sysex_message(midi_msg *message) {
  // Put the message args on the reply queue for the command
  auto prefix_end = message->begin() + SYSEX_PREFIX.size();
  byte command = *prefix_end;
  if (commands_with_reply.count(command)) {
    midi_msg args(prefix_end + 1, message->end() - 1);
    lock_guard<mutex> lock(this->reply_queues_lock);
    this->sysex_reply_queues[command].push(args);
  }
}

MidiInterface::~MidiInterface() {
  if (this->midi_in && this->midi_out) {
    this->disconnect();
  }
}
