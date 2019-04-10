#include "push.h"
#include "libusb.h"
#include "RtMidi.h"
#include <iostream>
#include <string>

const unsigned int ABLETON_VENDOR_ID = 0x2982;
const unsigned int PUSH2_PRODUCT_ID = 0x1967;

libusb_device_handle* push2_device_handle;

static libusb_device_handle* open_push2_device();
static void close_push2_device(libusb_device_handle* device_handle);
