#include "LedInterface.hpp"
#include "MidiInterface.hpp"
#include "MidiMessageListener.hpp"
#include "MidiMsg.hpp"
#include "SysexInterface.hpp"
#include "push.h"
#include <memory>

/// This class provides an API for getting input
/// from Push's encoders
class PadInterface {
public:
  enum PadSysex : byte {
    SET_PAD_PARAMETERS = 0x1B,
    SET_AFTERTOUCH_MODE = 0x1E,
    GET_AFTERTOUCH_MODE = 0x1F,
    SET_PAD_VELOCITY_CURVE_ENTRY = 0x20,
    SELECT_PAD_SETTINGS = 0x28,
    GET_SELECTED_PAD_SETTINGS = 0x29
  };

  PadInterface(MidiInterface &midi, SysexInterface &sysex, LedInterface &leds);

  void register_callback(LibPushPadCallback cb, void *context);

  /// \param low The lowest weight value that will trigger a pad's aftertoucuh
  /// \param high The highest weight value that will trigger a pad's aftertoucuh
  /// \effects Sets the aftertouch thresholds for all pads
  void set_global_aftertouch_range(unsigned short low, unsigned short high);

  /// \param mode The aftertouch mode
  /// \effects Sets the aftertouch mode for all pads to mode
  void set_global_aftertouch_mode(LibPushAftertouchMode mode);

  /// \returns The current aftertouch mode for Push's pads
  LibPushAftertouchMode get_global_aftertouch_mode();

  /// \param entries An array representing a velocity curve
  /// \effects Uses the given velocity curve for all pads
  void set_global_pad_velocity_curve(
      byte (&entries)[LIBPUSH_PAD_VELOCITY_CURVE_ENTRIES]);

  /// \param x (0-7) The row of the pad to set the sensitivity for
  /// \param y (0-7) The column of the pad to set the sensitivity for
  /// \param sensitivity The sensitivity value to use
  /// \effects Sets the pad at (x, y) to the given sensitivity
  void set_pad_sensitivity(byte x, byte y, LibPushPadSensitivity sensitivity);

  /// \param sensitivity The sensitivity value to use
  /// \effects Sets all pads to the given sensitivity
  void set_global_pad_sensitivity(LibPushPadSensitivity sensitivity);

  /// \param x (0-7) The row of the pad
  /// \param y (0-7) The column of the pad
  /// \returns The sensitivity value of the pad at (x, y)
  LibPushPadSensitivity get_pad_sensitivity(byte x, byte y);

  /// \param x (0-7) The row of the pad
  /// \param y (0-7) The column of the pad
  /// \param color_index (0-127) The index of the color in the palette
  /// \effects Sets the pad at (x, y) to the color in the palette at color_index
  void set_pad_color(byte x, byte y, uint color_index);

  /// \param x (0-7) The row of the pad
  /// \param y (0-7) The column of the pad
  /// \param color_index (0-127) The index of the color in the palette
  /// \param anim The animation to use
  /// \effects Sets the pad at (x, y) to the color in the palette at color_index and activates the specified animation
  void set_pad_animation(byte x, byte y, uint color_index,
                         LibPushLedAnimation anim);

  /// \param n The midi note number of a pad
  /// \returns The (x, y) coordinates the pad with note number n
  static std::tuple<uint, uint> pad_number_to_coordinates(uint n);

private:
  SysexInterface &sysex;
  LedInterface &leds;
  MidiMessageListener<LibPushPadEvent> listener;
  static std::unique_ptr<LibPushPadEvent> handle_message(byte msg_type,
                                                         midi_msg &message);
};

void foo();
