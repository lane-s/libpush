#include "MidiMsg.hpp"
#include "push.h"
#include <memory>
#include <string>

std::unique_ptr<LibPushPadEvent> pad_message_handler_fn(byte msg_type, midi_msg &message);
std::tuple<uint, uint> pad_number_to_coordinates(uint n);

std::unique_ptr<LibPushButtonEvent> button_message_handler_fn(byte msg_type, midi_msg &message);

std::unique_ptr<LibPushEncoderEvent> encoder_message_handler_fn(byte msg_type, midi_msg &message);

/// \param val The 7 bit two's complement value representing the encoder delta
/// \returns A delta value from -1 (fastest turning to the left) to 1 (fastest turning to the right)
double get_encoder_delta(uint val, int index);

std::unique_ptr<LibPushTouchStripEvent> touch_strip_message_handler_fn(byte msg_type, midi_msg &message);
