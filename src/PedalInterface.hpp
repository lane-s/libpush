#pragma once
#include "MidiInterface.hpp"
#include "MidiMessageHandler.hpp"
#include "MidiMessageListener.hpp"
#include "MidiMsg.hpp"
#include "SysexInterface.hpp"
#include "push.h"
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <vector>

/// PedalInterface provides an API for accessing sustain pedals or expression pedals
/// plugged in to Push. It provides an simple interface to send pedal-related sysex
/// messages to Push to detect pedal changes.
class PedalInterface {
public:
  enum PedalSysex : byte {
    SAMPLE_PEDAL_DATA = 0x0C,
    SET_PEDAL_CONFIGURATION = 0x30,
    SET_PEDAL_CURVE_LIMITS = 0x31,
    SET_PEDAL_CURVE_ENTRIES = 0x32
  };

  PedalInterface(MidiInterface &midi, SysexInterface &sysex);

  /// \param (0-19) log2 of the number of samples to average when sampling the pedal data
  /// \returns The average value over sample_size samples
  LibPushPedalSampleData sample_pedals(byte sample_size);

  /// \param contact Which pedal contact to configure
  /// \param enable Whether to enable or disable this contact
  void set_pedal_configuration(LibPushPedalContact contact, bool enable);

  /// \param pedal Which contact to set the curve limits for
  /// \param heel_down The MIDI number that should be used when the pedal is all the way up (i.e. 0)
  /// \param toe_down The MIDI number that should be used when the pedal is all the way down (i.e. 127)
  /// \effects Sets the range of MIDI values that the pedal will emit based on the detected voltage
  void set_pedal_curve_limits(LibPushPedalContact contact,
                              unsigned short heel_down,
                              unsigned short toe_down);

  /// \param pedal Which contact to set the curve entries for
  /// \param entries An array of bytes representing a curve
  /// \effects After calling this function, the pedal contact will map values to the curve before emitting them
  void set_pedal_curve_entries(LibPushPedalContact contact,
                               byte (&entries)[LIBPUSH_PEDAL_CURVE_ENTRIES]);

  void register_callback(LibPushPedalCallback cb, void *context);

private:
  SysexInterface &sysex;

  MidiMessageListener<LibPushPedalEvent> listener;
  std::unique_ptr<LibPushPedalEvent> handle_message(byte msg_type,
                                                    midi_msg &message);
  std::vector<byte>
      available_cc_numbers; //< Which cc numbers can be used for pedals
  std::unordered_map<LibPushPedalContact, byte>
      contact_cc_numbers; //< What cc number is each contact currently set to
  static std::unordered_set<byte> possible_cc_numbers;
};
