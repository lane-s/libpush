#include "PedalInterface.hpp"

using namespace std;

unordered_set<byte> PedalInterface::possible_cc_numbers = {64, 65, 66, 69};

PedalInterface::PedalInterface(MidiInterface &midi, SysexInterface &sysex)
    : sysex(sysex),
      listener([this](byte msg_type,
                      midi_msg &message) -> unique_ptr<LibPushPedalEvent> {
        return this->handle_message(msg_type, message);
      }) {
  midi.register_handler(&this->listener);
  this->available_cc_numbers = {65, 66};
  this->contact_cc_numbers = {{LibPushPedalContact::LP_PEDAL_1_RING, 64},
                              {LibPushPedalContact::LP_PEDAL_2_RING, 69}};
}

LibPushPedalSampleData PedalInterface::sample_pedals(byte sample_size) {
  midi_msg args({sample_size});
  midi_msg reply = this->sysex.sysex_call(PedalSysex::SAMPLE_PEDAL_DATA, args);

  LibPushPedalSampleData data;
  data.pedal_1_ring = reply[0] | (reply[1] << 7);
  data.pedal_1_tip = reply[2] | (reply[3] << 7);
  data.pedal_2_ring = reply[4] | (reply[5] << 7);
  data.pedal_2_tip = reply[6] | (reply[7] << 7);
  return data;
}

void PedalInterface::set_pedal_configuration(LibPushPedalContact contact,
                                             bool enable) {
  byte cc_val;
  if (enable) {
    if (this->contact_cc_numbers.find(contact) !=
        this->contact_cc_numbers.end()) {
      // The contact already has a cc value assigned
      return;
    }

    // Remove cc val from available and add it to the map
    cc_val = this->available_cc_numbers.back();
    this->available_cc_numbers.pop_back();
    this->contact_cc_numbers[contact] = cc_val;
  } else {
    auto key = this->contact_cc_numbers.find(contact);
    if (key == this->contact_cc_numbers.end()) {
      // The contact has no cc value assigned
      return;
    }

    cc_val = 127; // Value to unassign contact

    // Get the cc value assigned to the contact and add it to the available list
    this->available_cc_numbers.push_back(key->second);
    this->contact_cc_numbers.erase(key);
  }

  midi_msg args({static_cast<byte>(contact), cc_val, 0, 0});
  sysex.sysex_call(PedalSysex::SET_PEDAL_CONFIGURATION, args);
}

void PedalInterface::set_pedal_curve_limits(LibPushPedalContact contact,
                                            unsigned short heel_down,
                                            unsigned short toe_down) {
  midi_msg args({static_cast<byte>(contact),
                 static_cast<byte>(heel_down & 0x7F),
                 static_cast<byte>((heel_down & 0x1F) >> 7),
                 static_cast<byte>(toe_down & 0x7F),
                 static_cast<byte>((toe_down & 0x1F) >> 7)});
  sysex.sysex_call(PedalSysex::SET_PEDAL_CURVE_LIMITS, args);
}

constexpr byte CURVE_STEP = LIBPUSH_PEDAL_CURVE_ENTRIES / 4;
void PedalInterface::set_pedal_curve_entries(
    LibPushPedalContact contact, byte (&entries)[LIBPUSH_PEDAL_CURVE_ENTRIES]) {

  for (byte i = 0; i < LIBPUSH_PEDAL_CURVE_ENTRIES; i += CURVE_STEP) {
    // Entries are set 4 at a time
    midi_msg args({static_cast<byte>(contact), i});
    for (byte j = i; j < i + CURVE_STEP; ++j) {
      args.push_back(entries[j] & 0x7F);
      args.push_back(entries[j] >> 7);
    }
    sysex.sysex_call(PedalSysex::SET_PEDAL_CURVE_ENTRIES, args);
  }
}

void PedalInterface::register_callback(LibPushPedalCallback cb, void *context) {
  this->listener.register_callback(cb, context);
}

unique_ptr<LibPushPedalEvent>
PedalInterface::handle_message(byte msg_type, midi_msg &message) {
  if (msg_type != MidiMsgType::cc) {
    return nullptr;
  }
  byte cc_number = get_midi_number(msg_type, message);
  if (!PedalInterface::possible_cc_numbers.count(cc_number)) {
    return nullptr;
  }

  byte val = get_midi_value(msg_type, message);
  unique_ptr<LibPushPedalEvent> event = make_unique<LibPushPedalEvent>();
  event->value = (val / 127);

  for (auto itr = this->contact_cc_numbers.begin();
       itr != this->contact_cc_numbers.end(); ++itr) {
    if (cc_number == itr->second) {
      event->contact = itr->first;
      break;
    }
  }

  return event;
}
