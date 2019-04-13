#include "MidiMessageHandlerFns.hpp"

using namespace std;
using uint = unsigned int;

uint get_midi_number(byte msg_type, midi_msg *message) {
  switch (msg_type) {
  case MidiMsgType::note_on:
  case MidiMsgType::note_off:
  case MidiMsgType::aftertouch:
  case MidiMsgType::cc:
    return (*message)[1];
  default:
    throw runtime_error("Can't get number for midi message type: " +
                        to_string(msg_type));
  }
}

uint get_midi_velocity(byte msg_type, midi_msg *message) {
  switch (msg_type) {
  case MidiMsgType::note_on:
  case MidiMsgType::note_off:
  case MidiMsgType::aftertouch:
    return (*message)[2];
  case MidiMsgType::channel_pressure:
    return (*message)[1];
  default:
    throw runtime_error("Can't get velocity for midi message type: " + to_string(msg_type));
  }
}

const uint PAD_MATRIX_DIM = 8;
const uint FIRST_PAD_N = 36;

tuple<uint, uint> pad_number_to_coordinates(uint n) {
  uint x = (n - FIRST_PAD_N) % PAD_MATRIX_DIM;
  uint y = (n - FIRST_PAD_N) / PAD_MATRIX_DIM;
  y = (PAD_MATRIX_DIM - 1) - y; //y goes from top to bottom
  return make_tuple(x, y);
}

unique_ptr<LibPushPadEvent> pad_message_handler_fn(byte msg_type,
                                                   midi_msg *message) {
  LibPushPadEventType event_type;
  switch (msg_type) {
  case MidiMsgType::note_on:
    event_type = LibPushPadEventType::LP_PAD_PRESSED;
    break;
  case MidiMsgType::note_off:
    event_type = LibPushPadEventType::LP_PAD_RELEASED;
    break;
  case MidiMsgType::channel_pressure:
  case MidiMsgType::aftertouch:
    event_type = LibPushPadEventType::LP_PAD_AFTERTOUCH;
    break;
  default:
    return nullptr;
  }

  unique_ptr<LibPushPadEvent> event = make_unique<LibPushPadEvent>();
  event->event_type = event_type;
  event->velocity = get_midi_velocity(msg_type, message);

  int pad_number = get_midi_number(msg_type, message);
  auto pad_coords = pad_number_to_coordinates(pad_number);
  event->x = get<0>(pad_coords);
  event->y = get<1>(pad_coords);

  return event;
}

unique_ptr<LibPushButtonEvent> button_message_handler_fn(byte msg_type,
                                                         midi_msg *message) {
  return nullptr;
}

unique_ptr<LibPushEncoderEvent> encoder_message_handler_fn(byte msg_type,
                                                           midi_msg *message) {
  return nullptr;
}

unique_ptr<LibPushTouchStripEvent>
touch_strip_message_handler_fn(byte msg_type, midi_msg *message) {
  return nullptr;
}
