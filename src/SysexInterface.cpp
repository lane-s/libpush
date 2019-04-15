#include "SysexInterface.hpp"
using namespace std;

using LedSys = SysexInterface::LedSysex;
using PadSys = SysexInterface::PadSysex;
using TouchSys = SysexInterface::TouchStripSysex;
using PedalSys = SysexInterface::PedalSysex;
using MiscSys = SysexInterface::MiscSysex;

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

SysexInterface::SysexInterface(std::unique_ptr<RtMidiOut> &midi_out)
    : midi_out(midi_out) {
  for (byte command : commands_with_reply) {
    this->sysex_reply_queues[command] = queue<midi_msg>();
  }
}

midi_msg SysexInterface::sysex_call(byte command, midi_msg args) {
  if (!this->midi_out) {
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
  }
  return reply;
}

midi_msg SysexInterface::get_sysex_reply(byte command) {
  promise<midi_msg> p;
  auto f = p.get_future();
  thread listener(&poll_for_sysex_reply, command, move(p), this);
  listener.join();
  return f.get();
}

void SysexInterface::poll_for_sysex_reply(byte command,
                                          std::promise<midi_msg> p,
                                          SysexInterface *self) {
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

void SysexInterface::handle_sysex_message(midi_msg &message) {
  // Put the message args on the reply queue for the command
  auto prefix_end = message.begin() + SYSEX_PREFIX.size();
  byte command = *prefix_end;
  if (commands_with_reply.count(command)) {
    midi_msg args(prefix_end + 1, message.end() - 1);
    lock_guard<mutex> lock(this->reply_queues_lock);
    this->sysex_reply_queues[command].push(args);
  }
}
