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
void MidiMessageListener<Event>::register_callback(callback cb,
                                                   void *user_data) {
  this->callbacks.push_back(make_tuple(cb, user_data));
}

template <typename Event>
void MidiMessageListener<Event>::handle_message(midi_msg *message) {
  byte msg_type = *message->begin() & 0xF0;
  unique_ptr<Event> result = this->handler_func(msg_type, message);
  if (result) {
    for (auto cb : this->callbacks) {
      callback fn = get<0>(cb);
      void *user_data = get<1>(cb);
      fn(*result, user_data);
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
