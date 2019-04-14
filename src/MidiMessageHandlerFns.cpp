#include "MidiMessageHandlerFns.hpp"
#include <unordered_set>
#include <iostream>

using namespace std;

const uint PAD_MATRIX_DIM = 8;
const uint FIRST_PAD_N = 36;

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
  int pad_number = get_midi_number(msg_type, message);
  if (pad_number < FIRST_PAD_N ||
      pad_number >= FIRST_PAD_N + PAD_MATRIX_DIM * PAD_MATRIX_DIM) {
    return nullptr;
  }

  unique_ptr<LibPushPadEvent> event = make_unique<LibPushPadEvent>();
  event->event_type = event_type;
  event->velocity = get_midi_value(msg_type, message);

  auto pad_coords = pad_number_to_coordinates(pad_number);
  event->x = get<0>(pad_coords);
  event->y = get<1>(pad_coords);

  return event;
}

tuple<uint, uint> pad_number_to_coordinates(uint n) {
  uint x = (n - FIRST_PAD_N) % PAD_MATRIX_DIM;
  uint y = (n - FIRST_PAD_N) / PAD_MATRIX_DIM;
  y = (PAD_MATRIX_DIM - 1) - y; //y goes from top to bottom
  return make_tuple(x, y);
}

const uint TOP_BTN_ROW_START = 102;
const uint BOTTOM_BTN_ROW_START = 20;
const uint SCENE_BTN_COL_START = 43;

unordered_set<uint> push_btn_numbers = {
    LibPushButton::LP_PLAY_BTN,        LibPushButton::LP_RECORD_BTN,
    LibPushButton::LP_AUTOMATE_BTN,    LibPushButton::LP_FIXED_LENGTH_BTN,
    LibPushButton::LP_NEW_BTN,         LibPushButton::LP_DUPLICATE_BTN,
    LibPushButton::LP_QUANTIZE_BTN,    LibPushButton::LP_DOUBLE_LOOP_BTN,
    LibPushButton::LP_CONVERT_BTN,     LibPushButton::LP_UNDO_BTN,
    LibPushButton::LP_DELETE_BTN,      LibPushButton::LP_TAP_TEMPO_BTN,
    LibPushButton::LP_METRONOME_BTN,   LibPushButton::LP_ADD_DEVICE_BTN,
    LibPushButton::LP_ADD_TRACK_BTN,   LibPushButton::LP_MASTER_BTN,
    LibPushButton::LP_SETUP_BTN,       LibPushButton::LP_USER_BTN,
    LibPushButton::LP_DEVICE_BTN,      LibPushButton::LP_BROWSE_BTN,
    LibPushButton::LP_MIX_BTN,         LibPushButton::LP_CLIP_BTN,
    LibPushButton::LP_CLIP_BTN,        LibPushButton::LP_LEFT_BTN,
    LibPushButton::LP_UP_BTN,          LibPushButton::LP_RIGHT_BTN,
    LibPushButton::LP_DOWN_BTN,        LibPushButton::LP_REPEAT_BTN,
    LibPushButton::LP_ACCENT_BTN,      LibPushButton::LP_SCALE_BTN,
    LibPushButton::LP_LAYOUT_BTN,      LibPushButton::LP_NOTE_BTN,
    LibPushButton::LP_SESSION_BTN,     LibPushButton::LP_PAGE_LEFT_BTN,
    LibPushButton::LP_PAGE_RIGHT_BTN,  LibPushButton::LP_OCTAVE_UP_BTN,
    LibPushButton::LP_OCTAVE_DOWN_BTN, LibPushButton::LP_SHIFT_BTN,
    LibPushButton::LP_SELECT_BTN};

unique_ptr<LibPushButtonEvent> button_message_handler_fn(byte msg_type,
                                                         midi_msg &message) {
  if (msg_type != MidiMsgType::cc) {
    return nullptr;
  }

  unique_ptr<LibPushButtonEvent> event = make_unique<LibPushButtonEvent>();

  uint btn_number = get_midi_number(msg_type, message);
  if (btn_number >= TOP_BTN_ROW_START &&
      btn_number < TOP_BTN_ROW_START + PAD_MATRIX_DIM) {
    event->button = LibPushButton::LP_DISPLAY_TOP_BTN;
    event->button_index = btn_number - TOP_BTN_ROW_START;
  } else if (btn_number >= BOTTOM_BTN_ROW_START &&
             btn_number < BOTTOM_BTN_ROW_START + PAD_MATRIX_DIM) {
    event->button = LibPushButton::LP_DISPLAY_BOTTOM_BTN;
    event->button_index = btn_number - BOTTOM_BTN_ROW_START;
  } else if (btn_number <= SCENE_BTN_COL_START &&
             btn_number > SCENE_BTN_COL_START - PAD_MATRIX_DIM) {
    event->button = LibPushButton::LP_SCENE_BTN;
    event->button_index = SCENE_BTN_COL_START - btn_number;
  } else if (push_btn_numbers.count(btn_number)) {
    event->button = static_cast<LibPushButton>(btn_number);
    event->button_index = -1;
  } else {
    return nullptr;
  }

  if (get_midi_value(msg_type, message)) {
    event->event_type = LibPushButtonEventType::LP_BTN_PRESSED;
  } else {
    event->event_type = LibPushButtonEventType::LP_BTN_RELEASED;
  }

  return event;
}

const uint TOP_LEFT_ENCODER_CC = 14;
const uint ENCODER_CC_ROW_START = 71;
const uint TOP_LEFT_ENCODER_NN = 10;
const uint ENCODER_NN_ROW_START = 0;

unique_ptr<LibPushEncoderEvent> encoder_message_handler_fn(byte msg_type,
                                                           midi_msg &message) {
  if (msg_type != MidiMsgType::cc && msg_type != MidiMsgType::note_on) {
    return nullptr;
  }

  unique_ptr<LibPushEncoderEvent> event = make_unique<LibPushEncoderEvent>();

  uint encoder_number = get_midi_number(msg_type, message);
  uint val = get_midi_value(msg_type, message);
  if (msg_type == MidiMsgType::cc) {
    if (encoder_number >= TOP_LEFT_ENCODER_CC &&
        encoder_number < TOP_LEFT_ENCODER_CC + 2) {
      event->index = encoder_number - TOP_LEFT_ENCODER_CC;
    } else if (encoder_number >= ENCODER_CC_ROW_START &&
               encoder_number < ENCODER_CC_ROW_START + PAD_MATRIX_DIM + 1) {
      event->index = encoder_number - ENCODER_CC_ROW_START + 2;
    } else {
      return nullptr;
    }

    event->event_type = LibPushEncoderEventType::LP_ENCODER_MOVED;
    event->delta = get_encoder_delta(val, event->index);

  } else if (msg_type == MidiMsgType::note_on) {
    if (encoder_number <= TOP_LEFT_ENCODER_NN &&
        encoder_number > TOP_LEFT_ENCODER_NN - 2) {
      event->index = TOP_LEFT_ENCODER_NN - encoder_number;
    } else if (encoder_number >= ENCODER_NN_ROW_START &&
               encoder_number < ENCODER_NN_ROW_START + PAD_MATRIX_DIM + 1) {
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

const double TEMPO_ENCODER_FULL_TURN = 18.0;
const double ENCODER_FULL_TURN = 210.0;

double get_encoder_delta(uint val, int index) {
  int sign = (val >> 6)
                 ? -1
                 : 1; //The 7th bit is 1 if the value is negative, 0 otherwise
  if (sign < 0) {
    val = ~val + 1; // Take two's complement
  }
  val &= 0x7F;        //Bit mask to remove 8th bit

  double full_turn = index > 0 ? ENCODER_FULL_TURN : TEMPO_ENCODER_FULL_TURN;
  return (val / full_turn) * sign; // Normalize and set sign
}

unique_ptr<LibPushTouchStripEvent>
touch_strip_message_handler_fn(byte msg_type, midi_msg &message) {
  return nullptr;
}
