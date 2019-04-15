#include "EncoderInterface.hpp"

using namespace std;
EncoderInterface::EncoderInterface(MidiInterface &midi)
    : listener(EncoderInterface::handle_message) {
  midi.register_handler(&this->listener);
}

void EncoderInterface::register_callback(LibPushEncoderCallback cb,
                                         void *context) {
  this->listener.register_callback(cb, context);
}

constexpr uint TOP_LEFT_ENCODER_CC = 14;
constexpr uint ENCODER_CC_ROW_START = 71;
constexpr uint TOP_LEFT_ENCODER_NN = 10;
constexpr uint ENCODER_NN_ROW_START = 0;

unique_ptr<LibPushEncoderEvent>
EncoderInterface::handle_message(byte msg_type, midi_msg &message) {

  if (msg_type != MidiMsgType::cc && msg_type != MidiMsgType::note_on) {
    return nullptr;
  }

  unique_ptr<LibPushEncoderEvent> event = make_unique<LibPushEncoderEvent>();

  uint encoder_number = get_midi_number(msg_type, message);
  uint val = get_midi_value(msg_type, message);
  if (msg_type == MidiMsgType::cc) {
    // Encoder was turned
    if (encoder_number >= TOP_LEFT_ENCODER_CC &&
        encoder_number < TOP_LEFT_ENCODER_CC + 2) {
      event->index = encoder_number - TOP_LEFT_ENCODER_CC;
    } else if (encoder_number >= ENCODER_CC_ROW_START &&
               encoder_number <
                   ENCODER_CC_ROW_START + LIBPUSH_PAD_MATRIX_DIM + 1) {
      event->index = encoder_number - ENCODER_CC_ROW_START + 2;
    } else {
      return nullptr;
    }

    event->event_type = LibPushEncoderEventType::LP_ENCODER_MOVED;
    event->delta = EncoderInterface::get_delta(val, event->index);

  } else if (msg_type == MidiMsgType::note_on) {
    // Encoder was touched or released
    if (encoder_number <= TOP_LEFT_ENCODER_NN &&
        encoder_number > TOP_LEFT_ENCODER_NN - 2) {
      event->index = TOP_LEFT_ENCODER_NN - encoder_number;
    } else if (encoder_number >= ENCODER_NN_ROW_START &&
               encoder_number <
                   ENCODER_NN_ROW_START + LIBPUSH_PAD_MATRIX_DIM + 1) {
      event->index = encoder_number - ENCODER_NN_ROW_START + 2;
    } else {
      return nullptr;
    }
    event->delta = 0.0;

    if (val) {
      event->event_type = LibPushEncoderEventType::LP_ENCODER_TOUCHED;
    } else {
      event->event_type = LibPushEncoderEventType::LP_ENCODER_RELEASED;
    }
  }

  return event;
}

constexpr double TEMPO_ENCODER_FULL_TURN = 18.0;
constexpr double ENCODER_FULL_TURN = 210.0;

double EncoderInterface::get_delta(uint val, int index) {
  int sign = (val >> 6)
                 ? -1
                 : 1; //The 7th bit is 1 if the value is negative, 0 otherwise
  if (sign < 0) {
    val = ~val + 1; // Take two's complement
  }
  val &= 0x7F; //Bit mask to remove 8th bit

  double full_turn = index > 0 ? ENCODER_FULL_TURN : TEMPO_ENCODER_FULL_TURN;
  return (val / full_turn) * sign; // Normalize and set sign
}
