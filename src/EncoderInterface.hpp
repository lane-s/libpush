#include "MidiInterface.hpp"
#include "MidiMessageListener.hpp"
#include "MidiMsg.hpp"
#include "push.h"
#include <memory>

/// This class provides an API for getting input
/// from Push's encoders
class EncoderInterface {
public:
  EncoderInterface(MidiInterface &midi);

  void register_callback(LibPushEncoderCallback cb, void *context);

private:
  MidiMessageListener<LibPushEncoderEvent> listener;
  static std::unique_ptr<LibPushEncoderEvent> handle_message(byte msg_type,
                                                             midi_msg &message);

  /// \param val The 7 bit two's complement value representing the encoder delta
  /// \param index The index of the encoder that was moved
  /// \returns A delta value from -1 (fastest turning to the left) to 1 (fastest turning to the right)
  static double get_delta(uint val, int index);
};
