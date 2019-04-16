#include "MidiMsg.hpp"
#include "push.h"
#include <memory>
#include <string>

std::unique_ptr<LibPushButtonEvent>
button_message_handler_fn(byte msg_type, midi_msg &message);
