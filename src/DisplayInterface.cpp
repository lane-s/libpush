#include "DisplayInterface.hpp"

using namespace std;

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

DisplayInterface::~DisplayInterface() {
  if (this->push2_handle) {
    this->disconnect();
  }
}
