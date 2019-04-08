# libpush #

libpush provides an API for Ableton's Push 2 controller.

While it is completely possible to program for Push using the interface described [here](https://github.com/Ableton/push-interface/blob/master/doc/AbletonPush2MIDIDisplayInterface.asc), this library is intended to make the process easier for application programmers and provide a basis for APIs in higher level languages like python.

## build ##
libpush uses cmake to support cross platform builds. It is currently only being tested on OSX.

`cd libpush`
`cmake .`
`make install`
