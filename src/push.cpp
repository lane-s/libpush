#include "push.hpp"

using namespace std;

static libusb_device_handle* open_push2_device()
{
  int result;

  if ((result = libusb_init(NULL)) < 0)
  {
    cout << "error: " << result << " could not initilialize libusb" << endl;
    return NULL;
  }

  libusb_set_debug(NULL, LIBUSB_LOG_LEVEL_ERROR);

  libusb_device** devices;
  ssize_t count;
  count = libusb_get_device_list(NULL, &devices);
  if (count < 0)
  {
    cout << "error: " << count << " could not get usb device list" << endl;
    return NULL;
  }

  libusb_device* device;
  libusb_device_handle* device_handle = NULL;

  string error_message;

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

  if (device_handle == NULL)
  {
    cout << error_message << endl;
  }

  libusb_free_device_list(devices, 1);
  return device_handle;
}

static void close_push2_device(libusb_device_handle* device_handle)
{
  libusb_release_interface(device_handle, 0);
  libusb_close(device_handle);
}

bool connect() {
  try {
    RtMidiIn midiin;
  } catch (RtMidiError &error) {
    cout << "Could not create midi input with RtMidi" << endl;
    return false;
  }

  push2_device_handle = open_push2_device();
  if (push2_device_handle == NULL) {
    return false;
  }

  return true;
}

void disconnect() {
  close_push2_device(push2_device_handle);
}
