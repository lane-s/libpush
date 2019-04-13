#include "MidiMessageListener.hpp"

using namespace std;

template <typename Callback, typename Event>
using handler_fn =
    typename MidiMessageListener<Callback, Event>::handler_fn;

template <typename Callback, typename Event>
MidiMessageListener<Callback, Event>::MidiMessageListener(
    handler_fn handler)
    : handler_func(handler) {}

template <typename Callback, typename Event>
void MidiMessageListener<Callback, Event>::register_callback(Callback cb,
                                                            void *user_data) {
  this->callbacks.push_back(make_tuple(cb, user_data));
}

template <typename Callback, typename Event>
void MidiMessageListener<Callback, Event>::handle_message(midi_msg *message) {
  byte msg_type = *message->begin();
  unique_ptr<Event> result = this->handler_func(msg_type, message);
  if (result) {
    for (auto cb : this->callbacks) {
      Callback fn = get<0>(cb);
      void* user_data = get<1>(cb);
      fn (*result, user_data);
    }
  }
}

// It's necessary to define the specific instances of the template that are going to be used
// in the library in order to avoid linking errors
// Alternatively the template implementation could be defined in the header
template class MidiMessageListener<LibPushPadCallback, LibPushPadEvent>;
template class MidiMessageListener<LibPushButtonCallback, LibPushButtonEvent>;
template class MidiMessageListener<LibPushEncoderCallback, LibPushEncoderEvent>;
template class MidiMessageListener<LibPushTouchStripCallback, LibPushTouchStripEvent>;
