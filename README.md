# libpush #

libpush provides an API for Ableton's Push 2 controller.

While it is completely possible to program for Push using the interface described [here](https://github.com/Ableton/push-interface/blob/master/doc/AbletonPush2MIDIDisplayInterface.asc), this library is intended to make the process easier for application programmers and provide a basis for APIs in higher level languages like Python or Clojure.

## API documentation ##
Coming soon

## Build ##
libpush uses cmake to support cross platform builds, but it is currently only being tested on OSX.

Users are required to manually install [libusb](https://libusb.info/) for the time being in order to build the library.

`cd libpush`
`make`
