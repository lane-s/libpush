#include "MidiMessageHandler.hpp"
#include "MidiMsg.hpp"
#include "push.h"
#include <memory>

template <typename Event>
class MidiMessageListener : public MidiMessageHandler {
public:
  using handler_fn = std::unique_ptr<Event> (*)(byte msg_type,
                                                midi_msg &message);
  using callback = void (*)(Event, void *);

  // \param handler The message handler that translates an incoming midi message into an Event object
  MidiMessageListener(handler_fn handler);

  // \param cb A C style callback that will be called when an event occurs
  // \param context A pointer to any data that needs to accessed when the callback is called
  // \effects Ensures cb will be called when this handler detects an event
  void register_callback(callback cb, void *context);

  /// \param message The incoming MIDI message
  /// \effects Determines if the message should trigger an event. If so, construct the event object and pass it to all registered callbacks
  void handle_message(midi_msg &message) override;

private:
  handler_fn handler_func;
  std::vector<std::tuple<callback, void *>> callbacks;
};
