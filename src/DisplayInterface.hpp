#pragma once
#include "MidiMsg.hpp"
#include "SysexInterface.hpp"
#include "libusb.h"
#include "push.h"
#include <exception>
#include <functional>
#include <iostream>
#include <memory>
#include <string>

#define DISPLAY_CONSTANTS
#define DISPLAY_HEIGHT LIBPUSH_DISPLAY_HEIGHT
#define DISPLAY_WIDTH LIBPUSH_DISPLAY_WIDTH
/// Since pixels are 16 bits, each one requires 2 bytes in the frame buffer
#define DISPLAY_WIDTH_BYTES (DISPLAY_WIDTH * 2)
/// Extra bytes added to the end of each row in the frame buffer
#define DISPLAY_PADDING_BYTES 128
/// The length of a single row of pixels in bytes
#define ROW_LENGTH (DISPLAY_WIDTH_BYTES + DISPLAY_PADDING_BYTES)
/// The total length of a complete frame buffer
#define FRAME_BUFFER_LENGTH (ROW_LENGTH * (DISPLAY_HEIGHT * 2))

/// A convenient interface to Push's display
///
/// Responsible for managing a connection to the bulk usb interface used for Push's display
/// and drawing buffers of pixels to the display. Uses libusb under the hood.
///
/// \notes The display's brightness is controlled by the MidiInterface
class DisplayInterface {
public:
  using Pixel = unsigned short int;
  using DeviceHandlePtr =
      std::unique_ptr<libusb_device_handle,
                      std::function<void(libusb_device_handle *)>>;

  enum DisplaySysex : byte {
    SET_DISPLAY_BRIGHTNESS = 0x08,
    GET_DISPLAY_BRIGHTNESS = 0x09,
  };

  DisplayInterface(SysexInterface &sysex);
  ~DisplayInterface();

  /// Connect to Push's display
  ///
  /// \effects Initializes libusb and enables draw_frame to be called successfully
  /// \requires The display is not already connected
  /// \throws [std::runtime_error]() if a connection can't be established
  void connect();

  /// Disconnect from Push's display
  ///
  /// \effects Closes the connection to the display and cleans up
  /// \requires The display is connected
  /// \throws [std::runtime_error]() if the display is not connected
  void disconnect();

  /// Draw a single frame of pixels to Push's display
  ///
  /// \param pixel_buffer A LIBPUSH_DISPLAY_HEIGHT by LIBPUSH_DISPLAY_WIDTH array of 16bit integers representing pixels
  /// \effects Draws the pixels to Push's display top to bottom, left to right
  /// \requires The display is connected
  void draw_frame(Pixel (&pixel_buffer)[DISPLAY_HEIGHT][DISPLAY_WIDTH]);

  /// \param (0-127) The display brightness
  void set_brightness(byte brightness);

  /// \returns (0-127) The current display brightness
  byte get_brightness();

private:
  DeviceHandlePtr push2_handle;
  SysexInterface &sysex;

  /// Find a usb device using libusb
  ///
  /// \param PRODUCT_ID The device's product identifier
  /// \param VENDOR_ID The device's vendor identifier
  /// \returns A handle for the device
  static libusb_device_handle *find_device(unsigned int PRODUCT_ID,
                                           unsigned int VENDOR_ID);
  /// Get the list of available usb devices
  ///
  /// \returns A list of available usb devices
  static libusb_device **get_device_list();

  /// Fills a frame_buffer in the manner expected by Push
  ///
  /// \param pixel_buffer The buffer of pixels to be rendered
  /// \param frame_buffer The frame buffer to fill
  /// \effects Fills the frame_buffer based on the pixel_buffer with bytes in little endian order
  /// \effects XORs the rows of pixels with a signal shaping pattern
  /// \effects Adds padding bytes at the end of each row
  static void fill_frame(Pixel (&pixel_buffer)[DISPLAY_HEIGHT][DISPLAY_WIDTH],
                         unsigned char (&frame_buffer)[FRAME_BUFFER_LENGTH]);
};
