#include "DisplayInterface.hpp"
#include "LedInterface.hpp"
#include "MidiInterface.hpp"
#include "SysexInterface.hpp"
#include "push.h"
#include <exception>
#include <iostream>
#include <string>

class PushInterface {
public:
  PushInterface(LibPushPort port);
  ~PushInterface();

  DisplayInterface display;
  MidiInterface midi;
  SysexInterface sysex;
  LedInterface leds;
};
