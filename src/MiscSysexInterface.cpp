#include "MiscSysexInterface.hpp"

MiscSysexInterface::MiscSysexInterface(SysexInterface &sysex) : sysex(sysex) {
  sysex.register_command_with_reply(MiscSysex::REQUEST_STATISTICS);
}

void MiscSysexInterface::set_midi_mode(LibPushMidiMode mode) {
  midi_msg args({static_cast<unsigned char>(mode)});
  midi_msg reply =
      this->sysex.sysex_call(MiscSysexInterface::SET_MIDI_MODE, args);
}

LibPushStats MiscSysexInterface::get_statistics(byte run_id) {
  midi_msg args({run_id});
  midi_msg reply = this->sysex.sysex_call(MiscSysex::REQUEST_STATISTICS, args);

  LibPushStats stats;
  stats.power_supply_status = static_cast<LibPushPowerSupplyStatus>(reply[0]);
  stats.uptime =
      (reply[2] | (reply[3] << 7) | (reply[4] << 14) | (reply[5] << 21));

  return stats;
}
