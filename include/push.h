#ifndef LIBPUSH_H_
#define LIBPUSH_H_

#include "exported.h"
#include <stdbool.h>

#define LIBPUSH_DISPLAY_HEIGHT 160
#define LIBPUSH_DISPLAY_WIDTH 960
#define LIBPUSH_PEDAL_CURVE_ENTRIES 32
#define LIBPUSH_PAD_VELOCITY_CURVE_ENTRIES 128
#define LIBPUSH_PAD_MATRIX_DIM 8
#define LIBPUSH_TOUCH_STRIP_LEDS 30

#ifdef __cplusplus
extern "C" {
#endif

typedef enum LibPushPort { LIVE, USER } LibPushPort;

typedef enum LibPushPadEventType {
  LP_PAD_PRESSED = 0,
  LP_PAD_RELEASED = 1,
  LP_PAD_AFTERTOUCH = 2,
} LibPushPadEventType;

/// \notes Pad coordinates go from top to bottom, left to right
typedef struct LibPushPadEvent {
  LibPushPadEventType event_type;
  unsigned int x; //< (0-7) The x coordinate of the pad
  unsigned int y; //< (0-7) The y coordinate of the pad
  unsigned int velocity;
} LibPushPadEvent;
typedef void (*LibPushPadCallback)(LibPushPadEvent event, void *context);

typedef enum LibPushAftertouchMode {
  LP_CHANNEL = 0,
  LP_POLYPHONIC = 1,
} LibPushAftertouchMode;

typedef enum LibPushPadSensitivity {
  LP_REGULAR_SENSITIVITY = 0,
  LP_REDUCED_SENSITIVTIY = 1,
  LP_LOW_SENSITIVITY = 2,
} LibPushPadSensitivity;

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
  int index; //< (0-7) Only relevant for buttons with type LP_DISPLAY_TOP, LP_DISPLAY_BOTTOM, or LP_SCENE_BTN
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
  int index; //< (0-10) Encoders are indexed from left to right
  double
      delta; //< (-1.0 - 1.0) A full turn to the right has been made when the accumulated delta value is approximately 1. Only relevant for the LP_ENCODER_MOVED event type.
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

typedef struct LibPushTouchStripConfig {
  bool
      controlled_by_host; //< Are the touch strip leds are controlled by the hardware or by the host
  bool led_point; //< Should the leds show as a bar or a point
  bool
      bar_starts_at_center; //< Should the led bar starts at the bottom or the center
  bool
      autoreturn; //< Should the touch strip return to a neutral value when released?
  bool
      autoreturn_to_center; //< Should the touch strip return to the bottom or center when released
} LibPushTouchStripConfig;

typedef enum LibPushPedalContact {
  LP_PEDAL_1_RING = 0,
  LP_PEDAL_1_TIP = 1,
  LP_PEDAL_2_RING = 2,
  LP_PEDAL_3_TIP = 4
} LibPushPedalContact;

typedef struct LibPushPedalEvent {
  LibPushPedalContact contact;
  double value; //< (0-1)
} LibPushPedalEvent;

typedef void (*LibPushPedalCallback)(LibPushPedalEvent event, void *context);

typedef struct LibPushPedalSampleData {
  unsigned short pedal_1_ring;
  unsigned short pedal_1_tip;
  unsigned short pedal_2_ring;
  unsigned short pedal_2_tip;
} LibPushPedalSampleData;

typedef struct LibPushLedColor {
  unsigned char r; //< Red (0-255)
  unsigned char g; //< Green (0-255)
  unsigned char b; //< Blue (0-255)
  unsigned char w; //< White (0-255)
} LibPushLedColor;

/// Leds can be set to one of the following animations
typedef enum LibPushLedAnimationType {
  LP_NO_TRANSITION = 0,
  LP_ONE_SHOT = 1, //< Turn the led off after the given duration
  LP_PULSE = 6,    //< Triangle wave pulse
  LP_BLINK = 11    //< Square wave blink
} LibPushLedAnimationType;

/// Animation duration as a subdivision of the beat
typedef enum LibPushLedAnimationDuration {
  LP_24TH = 0,
  LP_16TH = 1,
  LP_8TH = 2,
  LP_QUARTER = 3,
  LP_HALF = 4
} LibPushLedAnimationDuration;

typedef struct LibPushLedAnimation {
  LibPushLedAnimationType type;
  LibPushLedAnimationDuration duration;
} LibPushLedAnimation;

typedef enum LibPushMidiMode {
  LP_LIVE_MODE = 0,
  LP_USER_MODE = 1,
  LP_DUAL_MODE = 2
} LibPushMidiMode;

typedef enum LibPushPowerSupplyStatus {
  LP_USB_POWER_ONLY = 0,
  LP_EXTERNAL_POWER = 1
} LibPushPowerSupplyStatus;

typedef struct LibPushStats {
  LibPushPowerSupplyStatus power_supply_status;
  int uptime; //< Time since last reboot in seconds
} LibPushStats;

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

/// \param (0-127) The display brightness
EXPORTED void libpush_set_display_brightness(unsigned char brightness);

/// \returns (0-127) The current display brightness
EXPORTED unsigned char libpush_get_display_brightness();

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

/// \effects Registers a function to be called when Push gets new pedal data
EXPORTED void libpush_register_pedal_callback(LibPushPedalCallback cb,
                                              void *context);

/// \param x (0-7) The row of the pad
/// \param y (0-7) The column of the pad
/// \param color_index (0-127) The index of the color in the palette
/// \effects Sets the pad at (x, y) to the color in the palette at color_index
EXPORTED void libpush_set_pad_color(unsigned char x, unsigned char y,
                                    unsigned int color_index);

/// \param color_index (0-127) The index of the color in the palette
/// \effects Sets all pads to the color in the palette at color_index
EXPORTED void libpush_set_global_pad_color(unsigned int color_index);

/// \param x (0-7) The row of the pad
/// \param y (0-7) The column of the pad
/// \param color_index (0-127) The index of the color in the palette
/// \param anim The animation to use
/// \effects Sets the pad at (x, y) to the color in the palette at color_index and activates the specified animation
EXPORTED void libpush_set_pad_animation(unsigned char x, unsigned char y,
                                        unsigned int color_index,
                                        LibPushLedAnimation anim);

/// \param low The lowest weight value that will trigger a pad's aftertoucuh
/// \param high The highest weight value that will trigger a pad's aftertoucuh
/// \effects Sets the aftertouch thresholds for all pads
EXPORTED void libpush_set_global_aftertouch_range(unsigned short low,
                                                  unsigned short high);

/// \param mode The aftertouch mode
/// \effects Sets the aftertouch mode for all pads to mode
EXPORTED void libpush_set_global_aftertouch_mode(LibPushAftertouchMode mode);

/// \returns The current aftertouch mode for Push's pads
EXPORTED LibPushAftertouchMode libpush_get_global_aftertouch_mode();

/// \param entries An array representing a velocity curve
/// \effects Uses the given velocity curve for all pads
EXPORTED void libpush_set_global_pad_velocity_curve(
    unsigned char (&entries)[LIBPUSH_PAD_VELOCITY_CURVE_ENTRIES]);

/// \param x (0-7) The row of the pad to set the sensitivity for
/// \param y (0-7) The column of the pad to set the sensitivity for
/// \param sensitivity The sensitivity value to use
/// \effects Sets the pad at (x, y) to the given sensitivity
void libpush_set_pad_sensitivity(unsigned char x, unsigned char y,
                                 LibPushPadSensitivity sensitivity);

/// \param sensitivity The sensitivity value to use
/// \effects Sets all pads to the given sensitivity
EXPORTED void
libpush_set_global_pad_sensitivity(LibPushPadSensitivity sensitivity);

/// \param x (0-7) The row of the pad
/// \param y (0-7) The column of the pad
/// \returns The sensitivity value of the pad at (x, y)
EXPORTED LibPushPadSensitivity libpush_get_pad_sensitivity(unsigned char x,
                                                           unsigned char y);

/// \param btn The button to set the color for
/// \The index of the color in the current color palette
EXPORTED void libpush_set_button_led_color(LibPushButton btn,
                                           unsigned int color_index);

/// \param The configuration object
/// \effects Updates the touch strip according to the configuration flags
EXPORTED void libpush_set_touch_strip_config(LibPushTouchStripConfig cfg);

/// \returns The current touch strip configuration
EXPORTED LibPushTouchStripConfig libpush_get_touch_strip_config();

/// \param brightness An array of brightness values corresponding to each led of the touch strip
EXPORTED void libpush_set_touch_strip_leds(
    unsigned char (&brightness)[LIBPUSH_TOUCH_STRIP_LEDS]);

/// \returns The average value over sample_size samples
EXPORTED LibPushPedalSampleData
libpush_sample_pedals(unsigned char sample_size);

/// \param contact Which pedal contact to configure
/// \param enable Whether to enable or disable this contact
EXPORTED void libpush_set_pedal_configuration(LibPushPedalContact contact,
                                              bool enable);

/// \param pedal Which contact to set the curve limits for
/// \param heel_down The MIDI number that should be used when the pedal is all the way up (i.e. 0)
/// \param toe_down The MIDI number that should be used when the pedal is all the way down (i.e. 127)
/// \effects Sets the range of MIDI values that the pedal will emit based on the detected voltage
EXPORTED void libpush_set_pedal_curve_limits(LibPushPedalContact contact,
                                             unsigned short heel_down,
                                             unsigned short toe_down);

/// \param pedal Which contact to set the curve entries for
/// \param entries An array of bytes representing a curve
/// \effects After calling this function, the pedal contact will map values to the curve before emitting them
EXPORTED void libpush_set_pedal_curve_entries(
    LibPushPedalContact contact,
    unsigned char (&entries)[LIBPUSH_PEDAL_CURVE_ENTRIES]);

/// Set an LED color palette entry
///
/// The colors of Push's LEDs are not set directly.
/// Instead you set a color palette, and then
/// set each led to use a color from the palette.
/// \param color_index (0-127) the index of the color entry in the palette
/// \param color the color to set the entry to
/// \effects Sets the palette entry at color_index to color. Change will not be visible until reapply_color_palette is called
EXPORTED void libpush_set_led_color_palette_entry(unsigned char color_index,
                                                  LibPushLedColor color);
/// Get an LED color palette entry
///
/// \param color_index (0-127) the idnex of the color entry in the palette to get
/// \returns The color of the palette entry at color_index
EXPORTED LibPushLedColor
libpush_get_led_color_palette_entry(unsigned char color_index);

/// Update the color palette to use new colors
///
/// \effects All leds using a palette entry that was set to a different color since the last call to reapply_color_palette will be updated to the new color
EXPORTED void libpush_reapply_color_palette();

/// Sets the brightness of all Push's leds
///
/// \param brightness (0-127) The desired brightness value
EXPORTED void libpush_set_global_led_brightness(unsigned char brightness);

/// Get the global brightness of Push's leds
///
/// \returns (0-127) The current global brightness
EXPORTED unsigned char libpush_get_global_led_brightness();

/// Sets the led pwm frequency
///
/// Used to avoid conflicts with the shuttering frequency of video cameras
/// \param (20-116) The pwm frequency in Hz
EXPORTED void libpush_set_led_pwm_freq(int freq);

/// \param mode The midi mode to set Push to
/// \effects Setting Live or User mode will cause most midi messages to be set on only those ports
/// \notes Only use this if you want to give control of Push up to another application
EXPORTED void libpush_set_midi_mode(LibPushMidiMode mode);

/// \param run_id A new run starts after each reboot. Use 0 to get stats about the current run
/// \returns Power supply and uptime information
EXPORTED LibPushStats libpush_get_statistics(unsigned char run_id);

#ifdef __cplusplus
}
#endif
#endif
