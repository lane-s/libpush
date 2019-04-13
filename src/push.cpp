#include "push.hpp"

using namespace std;
using Pixel = unsigned short int;

bool libpush_connect(LibPushPort port) {
  try {
    midi_interface = new MidiInterface();
    midi_interface->connect(port);
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

void libpush_draw_frame(
    Pixel (&buffer)[LIBPUSH_DISPLAY_HEIGHT][LIBPUSH_DISPLAY_WIDTH]) {
  try {
    display_interface->draw_frame(buffer);
  } catch (exception &ex) {
    cerr << ex.what() << endl;
  }
}
