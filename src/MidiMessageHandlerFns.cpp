#include "MidiMessageHandlerFns.hpp"

using namespace std;

unique_ptr<LibPushPadEvent> pad_message_handler_fn(byte msg_type, midi_msg* message) {
  return nullptr;
}

unique_ptr<LibPushButtonEvent> button_message_handler_fn(byte msg_type, midi_msg* message) {
  return nullptr;
}

unique_ptr<LibPushEncoderEvent> encoder_message_handler_fn(byte msg_type, midi_msg* message) {
  return nullptr;
}

unique_ptr<LibPushTouchStripEvent> touch_strip_message_handler_fn(byte msg_type, midi_msg* message) {
  return nullptr;
}
