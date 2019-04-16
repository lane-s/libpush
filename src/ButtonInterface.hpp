#include "LedInterface.hpp"
#include "MidiInterface.hpp"
#include "MidiMessageListener.hpp"
#include "MidiMsg.hpp"
#include "push.h"
#include <memory>
/// This class provides an API for getting input
/// from Push's buttons
class ButtonInterface {
public:
  ButtonInterface(MidiInterface &midi, LedInterface &leds);

  void register_callback(LibPushButtonCallback cb, void *context);

  /// \param btn The button to set the color for
  /// \The index of the color in the current color palette
  void set_button_led_color(LibPushButton btn, uint color_index);

private:
  LedInterface &leds;
  MidiMessageListener<LibPushButtonEvent> listener;
  static std::unique_ptr<LibPushButtonEvent> handle_message(byte msg_type,
                                                            midi_msg &message);

  static std::unordered_set<uint> button_numbers;
};
