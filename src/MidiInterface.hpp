#include "RtMidi.h"

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
};
