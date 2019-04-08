cmake_minimum_required(VERSION 2.6)

project (push.py)
set(push.py_VERSION_MAJOR 0)
set(push.py VERSION_MINOR 0)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_BUILD_TYPE Debug)

find_path(LIBUSB_INCLUDE_DIR
  NAMES libusb.h
  PATH_SUFFIXES "include" "libusb" "libusb-1.0")
find_library(LIBUSB_LIBRARY
  NAMES usb usb-1.0
  PATH_SUFFIXES "lib" "lib32" "lib64")

include_directories("${PROJECT_BINARY_DIR}")
add_executable(main main.cxx)

target_include_directories(main PRIVATE ${LIBUSB_INCLUDE_DIR})
target_link_libraries(main ${LIBUSB_LIBRARY})

install (TARGETS main DESTINATION bin)
