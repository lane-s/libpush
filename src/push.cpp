#include "push.hpp"

using namespace std;
using Port = MidiInterface::Port;

bool libpush_connect(bool use_live_port) {
  try {
    midi_interface = new MidiInterface();
    if (use_live_port) {
      midi_interface->connect(Port::LIVE);
    } else {
      midi_interface->connect(Port::USER);
    }
  } catch (exception &ex) {
    cout << ex.what() << endl;
    return false;
  }

  try {
    display_interface = new DisplayInterface();
    display_interface->connect();
  } catch (exception &ex) {
    cerr << ex.what() << endl;
    return false;
  }

  return true;
}

bool libpush_disconnect() {
  if (!display_interface || !midi_interface) {
    return false;
  }

  try {
    display_interface->disconnect();
    midi_interface->disconnect();
    return true;
  } catch (exception &ex) {
    cerr << ex.what() << endl;
    return false;
  }
}
