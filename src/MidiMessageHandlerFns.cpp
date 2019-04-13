#include "MidiMessageHandlerFns.hpp"

using namespace std;

unique_ptr<LibPushPadEvent> pad_message_handler_fn(byte msg_type,
                                                   midi_msg &message) {
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

const uint PAD_MATRIX_DIM = 8;
const uint FIRST_PAD_N = 36;

tuple<uint, uint> pad_number_to_coordinates(uint n) {
  uint x = (n - FIRST_PAD_N) % PAD_MATRIX_DIM;
  uint y = (n - FIRST_PAD_N) / PAD_MATRIX_DIM;
  y = (PAD_MATRIX_DIM - 1) - y; //y goes from top to bottom
  return make_tuple(x, y);
}

unique_ptr<LibPushButtonEvent> button_message_handler_fn(byte msg_type,
                                                         midi_msg &message) {
  return nullptr;
}

unique_ptr<LibPushEncoderEvent> encoder_message_handler_fn(byte msg_type,
                                                           midi_msg &message) {
  return nullptr;
}

unique_ptr<LibPushTouchStripEvent>
touch_strip_message_handler_fn(byte msg_type, midi_msg &message) {
  return nullptr;
}
