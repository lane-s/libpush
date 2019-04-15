#include "MidiMessageListener.hpp"

using namespace std;

template <typename Event>
using callback = typename MidiMessageListener<Event>::callback;

template <typename Event>
using handler_fn = typename MidiMessageListener<Event>::handler_fn;

template <typename Event>
MidiMessageListener<Event>::MidiMessageListener(handler_fn handler)
    : handler_func(handler) {}

template <typename Event>
void MidiMessageListener<Event>::register_callback(callback cb, void *context) {
  this->callbacks.push_back(make_tuple(cb, context));
}

template <typename Event>
void MidiMessageListener<Event>::handle_message(midi_msg &message) {
  if (!this->callbacks.size()) {
    return;
  }

  byte msg_type = get_midi_type(message);
  unique_ptr<Event> result = this->handler_func(msg_type, message);
  if (result) {
    for (auto cb : this->callbacks) {
      callback fn = get<0>(cb);
      void *context = get<1>(cb);
      fn(*result, context);
    }
  }
}

// It's necessary to define the specific instances of the template that are going to be used
// in the library in order to avoid linking errors
// Alternatively the template implementation could be defined in the header
template class MidiMessageListener<LibPushPadEvent>;
template class MidiMessageListener<LibPushButtonEvent>;
template class MidiMessageListener<LibPushEncoderEvent>;
template class MidiMessageListener<LibPushTouchStripEvent>;
