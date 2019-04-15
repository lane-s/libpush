cmake_minimum_required(VERSION 3.14.1)

project (libpush)
set(${PROJECT_NAME}_VERSION_MAJOR 0)
set(${PROJECT_NAME}_VERSION_MINOR 0)
set(${PROJECT_NAME}_VERSION "${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}")

include(GNUInstallDirs)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

if(WIN32)
  set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
  add_compile_definitions(WIN_EXPORT)
endif(WIN32)

set(PRIVATE_INCLUDES)
set(LINK_LIBS)

# Find libusb
find_path(LIBUSB_INCLUDE_DIR
  NAMES libusb.h
  PATH_SUFFIXES "include" "libusb" "libusb-1.0")
find_library(LIBUSB_LIBRARY
  NAMES usb usb-1.0
  PATH_SUFFIXES "lib" "lib32" "lib64")

message(STATUS ${LIBUSB_INCLUDE_DIR})
message(STATUS ${LIBUSB_LIBRARY})

if((NOT LIBUSB_INCLUDE_DIR) OR (NOT LIBUSB_LIBRARY))
  message(FATAL_ERROR "Can't find libusb. Please verify that it is installed")
endif()

list(APPEND PRIVATE_INCLUDES ${LIBUSB_INCLUDE_DIR})
list(APPEND LINK_LIBS ${LIBUSB_LIBRARY})

# Find rtmidi
find_path(RTMIDI_INCLUDE_DIR
  NAMES rtmidi_c.h
  PATH_SUFFIXES "include" "rtmidi")

find_library(RTMIDI_LIBRARY
  NAMES rtmidi
  PATH_SUFFIXES "lib" "lib32" "lib64")

if((NOT RTMIDI_INCLUDE_DIR) OR (NOT RTMIDI_LIBRARY))
  # Pull rtmidi as submodule and build
  execute_process(COMMAND git submodule update --init --external/rtmidi
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
  add_subdirectory(external/rtmidi)
  set(RTMIDI_LIBRARY rtmidi_static)
  set(RTMIDI_INCLUDE_DIR external/rtmidi)
endif()

list(APPEND PRIVATE_INCLUDES ${RTMIDI_INCLUDE_DIR})
list(APPEND LINK_LIBS ${RTMIDI_LIBRARY})

# Create push shared library
set(SOURCE_FILES src/push.cpp src/DisplayInterface.cpp src/MidiInterface.cpp
  src/MidiMessageListener.cpp src/MidiMessageHandlerFns.cpp src/MidiMsg.cpp
  src/SysexInterface.cpp src/LedInterface.cpp src/MiscSysexInterface.cpp
  src/PedalInterface.cpp src/EncoderInterface.cpp src/PadInterface.cpp)
add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES})
set_target_properties(${PROJECT_NAME} PROPERTIES
  VERSION ${${PROJECT_NAME}_VERSION}
  SOVERSION 1)

target_include_directories(${PROJECT_NAME} PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
  PRIVATE ${PRIVATE_INCLUDES})

# Link libpush with libusb and rtmidi
target_link_libraries(${PROJECT_NAME} PRIVATE ${LINK_LIBS})

# Install libpush
install(TARGETS ${PROJECT_NAME}
    EXPORT push-export
    ARCHIVE  DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY  DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME})

# Export for use with find_package (i.e. find_package(libpush))
install(EXPORT push-export
  FILE ${PROJECT_NAME}Config.cmake
  NAMESPACE ${PROJECT_NAME}::
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME})

# Create test executable
add_executable(main src/main.cpp)
target_link_libraries(main ${PROJECT_NAME})
install (TARGETS main DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
