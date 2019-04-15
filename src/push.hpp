#include "DisplayInterface.hpp"
#include "EncoderInterface.hpp"
#include "LedInterface.hpp"
#include "MidiInterface.hpp"
#include "MiscSysexInterface.hpp"
#include "PedalInterface.hpp"
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
  MiscSysexInterface misc;
  LedInterface leds;
  PedalInterface pedals;
  EncoderInterface encoders;
};
