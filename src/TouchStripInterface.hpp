#pragma once
#include "MidiInterface.hpp"
#include "MidiMessageListener.hpp"
#include "MidiMsg.hpp"
#include "SysexInterface.hpp"
#include "push.h"
#include <memory>

/// This class provides an API for getting input
/// from Push's touch strip
class TouchStripInterface {
public:
  enum TouchStripSysex : byte {
    SET_TOUCH_STRIP_CONFIGURATION = 0x17,
    GET_TOUCH_STRIP_CONFIGURATION = 0x18,
    SET_TOUCH_STRIP_LEDS = 0x19
  };

  TouchStripInterface(MidiInterface &midi, SysexInterface &sysex);

  void register_callback(LibPushTouchStripCallback cb, void *context);

  /// \param The configuration object
  /// \effects Updates the touch strip according to the configuration flags
  void set_config(LibPushTouchStripConfig cfg);

  /// \returns The current touch strip configuration
  LibPushTouchStripConfig get_config();

  /// \param brightness An array of brightness values corresponding to each led of the touch strip
  void set_leds(byte (&brightness)[LIBPUSH_TOUCH_STRIP_LEDS]);

private:
  SysexInterface &sysex;
  MidiMessageListener<LibPushTouchStripEvent> listener;
  static std::unique_ptr<LibPushTouchStripEvent>
  handle_message(byte msg_type, midi_msg &message);
};
