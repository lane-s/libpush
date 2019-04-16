#include "MidiMessageHandlerFns.hpp"
#include <iostream>
#include <unordered_set>

using namespace std;

constexpr uint TOP_BTN_ROW_START = 102;
constexpr uint BOTTOM_BTN_ROW_START = 20;
constexpr uint SCENE_BTN_COL_START = 43;

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
      btn_number < TOP_BTN_ROW_START + LIBPUSH_PAD_MATRIX_DIM) {
    event->button = LibPushButton::LP_DISPLAY_TOP_BTN;
    event->index = btn_number - TOP_BTN_ROW_START;
  } else if (btn_number >= BOTTOM_BTN_ROW_START &&
             btn_number < BOTTOM_BTN_ROW_START + LIBPUSH_PAD_MATRIX_DIM) {
    event->button = LibPushButton::LP_DISPLAY_BOTTOM_BTN;
    event->index = btn_number - BOTTOM_BTN_ROW_START;
  } else if (btn_number <= SCENE_BTN_COL_START &&
             btn_number > SCENE_BTN_COL_START - LIBPUSH_PAD_MATRIX_DIM) {
    event->button = LibPushButton::LP_SCENE_BTN;
    event->index = SCENE_BTN_COL_START - btn_number;
  } else if (push_btn_numbers.count(btn_number)) {
    event->button = static_cast<LibPushButton>(btn_number);
    event->index = -1;
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
