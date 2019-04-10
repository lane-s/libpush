#include "libusb.h"
#include <iostream>
#include <string>
#include <exception>

const unsigned int ABLETON_VENDOR_ID = 0x2982;
const unsigned int PUSH2_PRODUCT_ID = 0x1967;

class DisplayInterface {
public:
  DisplayInterface(){};
  ~DisplayInterface();

  void connect();
  void disconnect();
private:
  libusb_device_handle* push2_handle = NULL;
};
