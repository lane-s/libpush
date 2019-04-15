#pragma once
#include "MidiMsg.hpp"
#include "RtMidi.h"
#include "SysexInterface.hpp"

/// Responsible for controlling Push's white and rgb leds
class LedInterface {
public:
  enum LedSysex : byte {
    SET_LED_COLOR_PALETTE_ENTRY = 0x03,
    GET_LED_COLOR_PALETTE_ENTRY = 0x04,
    REAPPLY_COLOR_PALETTE = 0x05,
    SET_LED_BRIGHTNESS = 0x06,
    GET_LED_BRIGHTNESS = 0x07,
    SET_LED_PWM_FREQ_CORRECTION = 0x0B,
    SET_LED_WHITE_BALANCE = 0x14,
    GET_LED_WHITE_BALANCE = 0x15
  };

  enum LedColorGroup : byte {
    RGB_BTN_RED = 0,
    RGB_BTN_GREEN = 1,
    RGB_BTN_BLUE = 2,
    PAD_RED = 3,
    PAD_GREEN = 4,
    PAD_BLUE = 5,
    DISPLAY_BTN_RED = 6,
    DISPLAY_BTN_GREEN = 7,
    DISPLAY_BTN_BLUE = 8,
    WHITE_BUTTONS = 9,
    TOUCH_STRIP = 10,
  };

  LedInterface(MidiInterface &midi, SysexInterface &sysex);

  /// Set an LED color palette entry
  ///
  /// The colors of Push's LEDs are not set directly.
  /// Instead you set a color palette, and then
  /// set each led to use a color from the palette.
  /// The LEDs will not actually change until
  /// Reapply color palette is called
  /// \param color_index (0-127) the index of the color entry in the palette
  /// \param color the color to set the entry to
  /// \effects Sets the palette entry at color_index to color. Change will not be visible until reapply_color_palette is called
  void set_led_color_palette_entry(byte color_index, LibPushLedColor color);

  /// Get an LED color palette entry
  ///
  /// \param color_index (0-127) the idnex of the color entry in the palette to get
  /// \returns The color of the palette entry at color_index
  LibPushLedColor get_led_color_palette_entry(byte color_index);

  /// Update the color palette to use new colors
  ///
  /// \effects All leds using a palette entry that was set to a different color since the last call to reapply_color_palette will be updated to the new color
  void reapply_color_palette();

  /// Sets the brightness of all Push's leds
  ///
  /// \param brightness (0-127) The desired brightness value
  void set_global_led_brightness(byte brightness);

  /// Get the global brightness of Push's leds
  ///
  /// \returns (0-127) The current global brightness
  byte get_global_led_brightness();

  /// Sets the led pwm frequency
  ///
  /// Used to avoid conflicts with the shuttering frequency of video cameras
  /// \param (20-116) The pwm frequency in Hz
  void set_led_pwm_freq(int freq);

  /// Sets the led white balance
  ///
  /// White balance is used to set the brightness of specific groups of leds
  /// This is used to balance differences in rgb and white led brightness
  /// \param color_group Which led color group to set the white balance for
  /// \param balance_factor (0-1024) The white balance factor
  void set_led_white_balance(LedColorGroup color_group,
                             unsigned short balance_factor);

  /// Gets the led white balance
  ///
  /// \param color_group Which led color group to get the balance factor for
  /// \returns (0-1024) A white balance factor
  unsigned short get_led_white_balance(LedColorGroup color_group);

  void set_led_color(MidiMsgType msg_type, uint midi_number,
                     LibPushLedAnimation animation, uint color_index);

private:
  MidiInterface &midi;
  SysexInterface &sysex;
};
