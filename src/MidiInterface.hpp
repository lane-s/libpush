#include "RtMidi.h"
#include <iostream>
#include <string>

class MidiInterface {
public:
  enum Port {
    LIVE,
    USER
  };

  MidiInterface();
  ~MidiInterface();

  void connect(Port port);
  void disconnect();

private:
  RtMidiIn* midi_in = NULL;
  RtMidiOut* midi_out = NULL;

  static int find_port(RtMidi* rtmidi, Port port);
};
