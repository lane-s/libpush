#include "DisplayInterface.hpp"

using namespace std;
using Pixel = DisplayInterface::Pixel;
using DeviceHandlePtr = DisplayInterface::DeviceHandlePtr;
using DeviceListPtr =
    unique_ptr<libusb_device *, function<void(libusb_device **)>>;

const unsigned int ABLETON_VENDOR_ID = 0x2982;
const unsigned int PUSH2_PRODUCT_ID = 0x1967;

unsigned char frame_header[16] = {0xFF, 0xCC, 0xAA, 0x88, 0x00, 0x00,
                                  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                  0x00, 0x00, 0x00, 0x00};

const unsigned char signal_shaping_pattern_len = 4;
const unsigned char signal_shaping_pattern[signal_shaping_pattern_len] = {
    0xE7, 0xF3, 0xE7, 0xFF};

const unsigned char PUSH2_BULK_EP_OUT = 0x01;
const unsigned int TRANSFER_TIMEOUT = 500;

DisplayInterface::DisplayInterface(SysexInterface &sysex)
    : push2_handle(nullptr), sysex(sysex) {}

void DisplayInterface::connect() {
  if (this->push2_handle) {
    throw runtime_error("Can't connect to Push display when already connected");
  }

  int result;
  if ((result = libusb_init(NULL)) < 0) {
    throw runtime_error(to_string(result) + " could not initialize libusb");
  }

  libusb_set_debug(NULL, LIBUSB_LOG_LEVEL_ERROR);

  this->push2_handle =
      DeviceHandlePtr(this->find_device(PUSH2_PRODUCT_ID, ABLETON_VENDOR_ID),
                      [](libusb_device_handle *handle) {
                        libusb_release_interface(handle, 0);
                        libusb_close(handle);
                      });

  if (!this->push2_handle) {
    throw runtime_error("Ableton Push 2 Device Not Found");
  }
}

libusb_device_handle *DisplayInterface::find_device(unsigned int PRODUCT_ID,
                                                    unsigned int VENDOR_ID) {

  DeviceListPtr devices(get_device_list(), [](libusb_device **device_list) {
    libusb_free_device_list(device_list, 1);
  });

  libusb_device_handle *device_handle;
  libusb_device *device;
  int result;

  for (int i = 0; (device = devices.get()[i]) != NULL; i++) {
    struct libusb_device_descriptor descriptor;
    if ((result = libusb_get_device_descriptor(device, &descriptor)) < 0) {
      continue;
    }

    if (descriptor.bDeviceClass == LIBUSB_CLASS_PER_INTERFACE &&
        descriptor.idVendor == VENDOR_ID &&
        descriptor.idProduct == PRODUCT_ID) {
      if ((result = libusb_open(device, &device_handle)) < 0) {
        throw runtime_error("error: " + to_string(result) +
                            " could not open Ableton Push 2 device");
      } else if ((result = libusb_claim_interface(device_handle, 0)) < 0) {
        libusb_close(device_handle);
        device_handle = NULL;
        throw runtime_error("error: " + to_string(result) +
                            " could not claim interface 0 of Push 2 device");
      } else {
        break; // successfully opened
      }
    }
  }

  return device_handle;
}

libusb_device **DisplayInterface::get_device_list() {
  libusb_device **devices;
  ssize_t count = libusb_get_device_list(NULL, &devices);
  if (count < 0) {
    throw runtime_error(to_string(count) +
                        " could not get the usb device list");
  }

  return devices;
}

void DisplayInterface::disconnect() {
  if (this->push2_handle) {
    this->push2_handle.reset(nullptr);
  } else {
    throw runtime_error(
        "Can't disconnect from Push display when not connected");
  }
}

void DisplayInterface::draw_frame(
    Pixel (&pixel_buffer)[DISPLAY_HEIGHT][DISPLAY_WIDTH]) {
  int result = libusb_bulk_transfer(this->push2_handle.get(), PUSH2_BULK_EP_OUT,
                                    frame_header, sizeof(frame_header), NULL,
                                    TRANSFER_TIMEOUT);

  if (result != 0) {
    throw runtime_error("Frame header transfer failed");
  }

  unsigned char frame_buffer[FRAME_BUFFER_LENGTH];
  DisplayInterface::fill_frame(pixel_buffer, frame_buffer);

  result = libusb_bulk_transfer(this->push2_handle.get(), PUSH2_BULK_EP_OUT,
                                frame_buffer, FRAME_BUFFER_LENGTH, NULL,
                                TRANSFER_TIMEOUT);

  if (result != 0) {
    throw runtime_error("Frame buffer transfer failed");
  }
}

void DisplayInterface::fill_frame(
    Pixel (&pixel_buffer)[DISPLAY_HEIGHT][DISPLAY_WIDTH],
    unsigned char (&frame_buffer)[FRAME_BUFFER_LENGTH]) {
  for (int row = 0; row < DISPLAY_HEIGHT; ++row) {
    int pattr_index = 0;
    for (int col = 0; col < DISPLAY_WIDTH; ++col) {
      // Fill frame buffer row by row in little endian order
      unsigned char MSB = pixel_buffer[row][col] >> 8;
      unsigned char LSB = pixel_buffer[row][col] & 0x00FF;

      // XOR with the signal shaping pattern in little endian order
      LSB ^= signal_shaping_pattern[pattr_index++ % signal_shaping_pattern_len];
      MSB ^= signal_shaping_pattern[pattr_index++ % signal_shaping_pattern_len];

      // Fill 2 bytes in the frame buffer for each pixel
      frame_buffer[(row * ROW_LENGTH) + col * 2] = LSB;
      frame_buffer[(row * ROW_LENGTH) + col * 2 + 1] = MSB;
    }

    // Pad each row with 128 filler bytes
    for (int col = DISPLAY_WIDTH_BYTES; col < ROW_LENGTH; ++col) {
      frame_buffer[row * ROW_LENGTH + col] = 0x00;
    }
  }
}

void DisplayInterface::set_brightness(byte brightness) {
  midi_msg args;
  args.push_back(brightness & 0x7F);
  args.push_back(brightness >> 7);
  this->sysex.sysex_call(DisplaySysex::SET_DISPLAY_BRIGHTNESS, args);
}

byte DisplayInterface::get_brightness() {
  midi_msg args;
  midi_msg reply =
      this->sysex.sysex_call(DisplaySysex::GET_DISPLAY_BRIGHTNESS, args);
  return (reply[0] | (reply[1] << 7));
}

DisplayInterface::~DisplayInterface() {}
