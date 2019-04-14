#include "MidiMsg.hpp"

uint get_midi_number(byte msg_type, midi_msg &message) {
  switch (msg_type) {
  case MidiMsgType::note_on:
  case MidiMsgType::note_off:
  case MidiMsgType::aftertouch:
  case MidiMsgType::cc:
    return message[1];
  default:
    throw std::runtime_error("Can't get number for midi message type: " +
                             std::to_string(msg_type));
  }
}

uint get_midi_value(byte msg_type, midi_msg &message) {
  switch (msg_type) {
  case MidiMsgType::note_on:
  case MidiMsgType::note_off:
  case MidiMsgType::aftertouch:
  case MidiMsgType::cc:
    return message[2];
  case MidiMsgType::channel_pressure:
    return message[1];
  default:
    throw std::runtime_error("Can't get velocity for midi message type: " +
                             std::to_string(msg_type));
  }
}
