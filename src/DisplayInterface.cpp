#include "DisplayInterface.hpp"

using namespace std;
using Pixel = DisplayInterface::Pixel;

const unsigned int ABLETON_VENDOR_ID = 0x2982;
const unsigned int PUSH2_PRODUCT_ID = 0x1967;

unsigned char frame_header[16] = {
  0xFF, 0xCC, 0xAA, 0x88,
  0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00
};

const unsigned char signal_shaping_pattern_len = 4;
const unsigned char signal_shaping_pattern[signal_shaping_pattern_len] = { 0xE7, 0xF3, 0xE7, 0xFF };

const unsigned char PUSH2_BULK_EP_OUT = 0x01;
const unsigned int TRANSFER_TIMEOUT = 500;

DisplayInterface::DisplayInterface() : push2_handle(NULL) {
}

void DisplayInterface::connect()
{
  if (this->push2_handle) {
    throw runtime_error("Can't connect to Push display when already connected");
  }

  int result;
  string error_message;

  if ((result = libusb_init(NULL)) < 0)
  {
    error_message = to_string(result) + " could not initialize libusb";
    throw runtime_error(error_message);
  }

  libusb_set_debug(NULL, LIBUSB_LOG_LEVEL_ERROR);

  libusb_device** devices;
  ssize_t count;
  count = libusb_get_device_list(NULL, &devices);
  if (count < 0)
  {
    error_message = to_string(count) + " could not get the usb device list";
    throw runtime_error(error_message);
  }

  libusb_device* device;
  libusb_device_handle* device_handle = NULL;

  // set message in case we get to the end of the list w/o finding a device
  error_message = "Ableton Push 2 device not found";

  for (int i = 0; (device = devices[i]) != NULL; i++)
  {
    struct libusb_device_descriptor descriptor;
    if ((result = libusb_get_device_descriptor(device, &descriptor)) < 0)
    {
      error_message = "error: " + to_string(result) + " could not get usb device descriptor";
      continue;
    }

    if (descriptor.bDeviceClass == LIBUSB_CLASS_PER_INTERFACE
      && descriptor.idVendor == ABLETON_VENDOR_ID
      && descriptor.idProduct == PUSH2_PRODUCT_ID)
    {
      if ((result = libusb_open(device, &device_handle)) < 0)
      {
        error_message = "error: " + to_string(result) + " could not open Ableton Push 2 device";
      }
      else if ((result = libusb_claim_interface(device_handle, 0)) < 0)
      {
        error_message = "error: " + to_string(result) + " could not claim interface 0 of Push 2 device";
        libusb_close(device_handle);
        device_handle = NULL;
      }
      else
      {
        break; // successfully opened
      }
    }
  }

  libusb_free_device_list(devices, 1);

  if (device_handle == NULL)
  {
    throw runtime_error(error_message);
  }

  this->push2_handle = device_handle;
}

void DisplayInterface::disconnect()
{
  if (this->push2_handle) {
    libusb_release_interface(this->push2_handle, 0);
    libusb_close(this->push2_handle);
  } else {
    throw runtime_error("Can't disconnect from Push display when not connected");
  }
}

void DisplayInterface::draw_frame(Pixel (&pixel_buffer)[DISPLAY_HEIGHT][DISPLAY_WIDTH]) {
  int result = libusb_bulk_transfer(
    this->push2_handle,
    PUSH2_BULK_EP_OUT,
    frame_header,
    sizeof(frame_header),
    NULL,
    TRANSFER_TIMEOUT);

  if (result != 0) {
    throw runtime_error("Frame header transfer failed");
  }

  unsigned char frame_buffer[FRAME_BUFFER_LENGTH];
  DisplayInterface::fill_frame(pixel_buffer, frame_buffer);

  result = libusb_bulk_transfer(
    this->push2_handle,
    PUSH2_BULK_EP_OUT,
    frame_buffer,
    FRAME_BUFFER_LENGTH,
    NULL,
    TRANSFER_TIMEOUT);

  if (result != 0) {
    throw runtime_error("Frame header transfer failed");
  }
}

void DisplayInterface::fill_frame(Pixel (&pixel_buffer)[DISPLAY_HEIGHT][DISPLAY_WIDTH],
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
      frame_buffer[(row * ROW_LENGTH) + col*2] = LSB;
      frame_buffer[(row * ROW_LENGTH) + col*2 + 1] = MSB;
    }

    // Pad each row with 128 filler bytes
    for (int col = DISPLAY_WIDTH_BYTES; col < ROW_LENGTH; ++col) {
      frame_buffer[row * ROW_LENGTH + col] = 0x00;
    }
  }
}

DisplayInterface::~DisplayInterface() {
  if (this->push2_handle) {
    this->disconnect();
  }
}
