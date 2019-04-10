#include "MidiInterface.hpp"

using namespace std;

MidiInterface::MidiInterface() {
}

void MidiInterface::connect(MidiInterface::Port port) {
  if(this->midi_in || this->midi_out) {
    throw runtime_error("Can't connect to Push midi port if already connected");
  }

  this->midi_in = new RtMidiIn();
  this->midi_out = new RtMidiOut();
}

void MidiInterface::disconnect() {
  if(!this->midi_in || !this->midi_out) {
    throw runtime_error("Can't disconnect from Push midi port unless already connected");
  }

  delete this->midi_in;
  delete this->midi_out;
}

MidiInterface::~MidiInterface() {
  if(this->midi_in && this->midi_out) {
    this->disconnect();
  }
}
