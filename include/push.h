#ifndef LIBPUSH_H_
#define LIBPUSH_H_ 1

#include "exported.h"

#define LIBPUSH_DISPLAY_HEIGHT 160
#define LIBPUSH_DISPLAY_WIDTH 960

extern "C" {

typedef enum LibPushPort { LIVE, USER } LibPushPort;

/// Initialize libpush and attempt to connect to Push
///
/// \param port the port to use for MIDI communication (Live or User)
/// \returns true if the connection is made, false otherwise
/// \effects Allows other libpush functions to be called if a connection is made
/// successfully
/// \requires libpush is not already connected to Push
EXPORTED bool libpush_connect(LibPushPort port);

/// Disconnect and cleanup
///
/// \effects Disconnects from Push and cleans up after libpush
/// \requires libpush is connected to Push
EXPORTED bool libpush_disconnect();

/// Draw a single frame to Push's display
///
/// \param pixel_buffer A LIBPUSH_DISPLAY_HEIGHT by LIBPUSH_DISPLAY_WIDTH array of 16bit integers representing pixels to be drawn to Push's display
/// \effects Draws the pixels to Push's display top to bottom, left to right
/// \requires libpush is connected to Push
EXPORTED void libpush_draw_frame(unsigned short int (
    &pixel_buffer)[LIBPUSH_DISPLAY_HEIGHT][LIBPUSH_DISPLAY_WIDTH]);

typedef enum LibPushPadEventType {
  LP_PAD_PRESSED = 0,
  LP_PAD_RELEASED = 1,
  LP_PAD_AFTERTOUCH = 2,
} LibPushPadEventType;

/// \notes Pad coordinates go from top to bottom, left to right
typedef struct LibPushPadEvent {
  LibPushPadEventType event_type;
  unsigned int x; //< The x coordinate of the pad (0-7)
  unsigned int y; //< The y coordinate of the pad (0-7)
  unsigned int velocity;
} LibPushPadEvent;
typedef void (*LibPushPadCallback)(LibPushPadEvent event, void *context);

/// All of Push's buttons
typedef enum LibPushButton {
  LP_PLAY_BTN = 85,
  LP_RECORD_BTN = 86,
  LP_AUTOMATE_BTN = 89,
  LP_FIXED_LENGTH_BTN = 90,
  LP_NEW_BTN = 87,
  LP_DUPLICATE_BTN = 88,
  LP_QUANTIZE_BTN = 116,
  LP_DOUBLE_LOOP_BTN = 117,
  LP_CONVERT_BTN = 35,
  LP_UNDO_BTN = 119,
  LP_DELETE_BTN = 118,
  LP_TAP_TEMPO_BTN = 3,
  LP_METRONOME_BTN = 9,
  LP_DISPLAY_TOP_BTN = 0,    //< The row of 8 buttons above the display
  LP_DISPLAY_BOTTOM_BTN = 1, //< The row of 8 buttons below the display
  LP_SCENE_BTN = 2, //< The column of 8 buttons to the right of the pad grid
  LP_ADD_DEVICE_BTN = 52,
  LP_ADD_TRACK_BTN = 53,
  LP_MASTER_BTN = 28,
  LP_SETUP_BTN = 30,
  LP_USER_BTN = 59,
  LP_DEVICE_BTN = 110,
  LP_BROWSE_BTN = 111,
  LP_MIX_BTN = 112,
  LP_CLIP_BTN = 113,
  LP_LEFT_BTN = 44,
  LP_UP_BTN = 46,
  LP_RIGHT_BTN = 45,
  LP_DOWN_BTN = 47,
  LP_REPEAT_BTN = 56,
  LP_ACCENT_BTN = 57,
  LP_SCALE_BTN = 58,
  LP_LAYOUT_BTN = 31,
  LP_NOTE_BTN = 50,
  LP_SESSION_BTN = 51,
  LP_PAGE_LEFT_BTN = 62,
  LP_PAGE_RIGHT_BTN = 63,
  LP_OCTAVE_UP_BTN = 55,
  LP_OCTAVE_DOWN_BTN = 54,
  LP_SHIFT_BTN = 49,
  LP_SELECT_BTN = 48,
} LibPushButton;

typedef enum LibPushButtonEventType {
  LP_BTN_PRESSED = 0,
  LP_BTN_RELEASED = 1,
} LibPushButtonEventType;

/// Event fired when a button is pressed or released
typedef struct LibPushButtonEvent {
  LibPushButtonEventType event_type;
  LibPushButton button;
  int index; //< Only relevant for buttons with type LP_DISPLAY_TOP, LP_DISPLAY_BOTTOM, or LP_SCENE_BTN(0-7)
} LibPushButtonEvent;
typedef void (*LibPushButtonCallback)(LibPushButtonEvent event, void *context);

typedef enum LibPushEncoderEventType {
  LP_ENCODER_TOUCHED = 0,
  LP_ENCODER_MOVED = 1,
  LP_ENCODER_RELEASED = 2,
} LibPushEncoderEventType;

/// Event fired when an encoder is turned
typedef struct LibPushEncoderEvent {
  LibPushEncoderEventType event_type;
  int index; //< (0-10) From left to right
  double
      delta; //< (-1.0 - 1.0) 1 indicates a full turn. Only relevant for the LP_ENCODER_MOVED event type.
} LibPushEncoderEvent;

typedef void (*LibPushEncoderCallback)(LibPushEncoderEvent event,
                                       void *context);

typedef enum LibPushTouchStripEventType {
  LP_TOUCH_STRIP_PRESSED = 0,
  LP_TOUCH_STRIP_MOVED = 1,
  LP_TOUCH_STRIP_RELEASED = 2,
} LibPushTouchStripEventType;

/// \notes The touch strip position is in the range [-1, 1], with 0 being the middle
typedef struct LibPushTouchStripEvent {
  LibPushTouchStripEventType event_type;
  double position;
} LibPushTouchStripEvent;
typedef void (*LibPushTouchStripCallback)(LibPushTouchStripEvent event,
                                          void *context);

/// \effects Registers a function to be called when a pad is pressed, released, or aftertouch data is received
EXPORTED void libpush_register_pad_callback(LibPushPadCallback cb,
                                            void *context);

/// \effects Registers a function to be called when a button is pressed or released
EXPORTED void libpush_register_button_callback(LibPushButtonCallback cb,
                                               void *context);

/// \effects Registers a function to be called when an encoder is turned
EXPORTED void libpush_register_encoder_callback(LibPushEncoderCallback cb,
                                                void *context);

/// \effects Registers a function to be called when the touch strip is pressed, moved, or released
EXPORTED void
libpush_register_touch_strip_callback(LibPushTouchStripCallback cb,
                                      void *context);
}

#endif
