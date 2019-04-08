cmake_minimum_required(VERSION 2.6)

project (push)
set(push_VERSION_MAJOR 0)
set(push_VERSION_MINOR 0)
set(push_VERSION "${push_VERSION_MAJOR}.${push_VERSION_MINOR}")

include(GNUInstallDirs)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Find libusb
find_path(LIBUSB_INCLUDE_DIR
  NAMES libusb.h
  PATH_SUFFIXES "include" "libusb" "libusb-1.0")
find_library(LIBUSB_LIBRARY
  NAMES usb usb-1.0
  PATH_SUFFIXES "lib" "lib32" "lib64")

# Create libpush
set(SOURCE_FILES src/push.cpp)
add_library(${PROJECT_NAME} ${SOURCE_FILES})
set_target_properties(${PROJECT_NAME} PROPERTIES
  VERSION ${push_VERSION}
  SOVERSION 1)

target_include_directories(${PROJECT_NAME} PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
  PRIVATE ${LIBUSB_INCLUDE_DIR})

# Link libpush with libusb
target_link_libraries(${PROJECT_NAME} ${LIBUSB_LIBRARY})

# Install libpush
install(TARGETS ${PROJECT_NAME} EXPORT libpushConfig
    ARCHIVE  DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY  DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME})
install(EXPORT libpushConfig DESTINATION share/libpush/cmake)
export(TARGETS ${PROJECT_NAME} FILE libpush.cmake)

# Create test executable
include_directories("${PROJECT_BINARY_DIR}")
add_executable(main src/main.cpp)
target_link_libraries(main ${PROJECT_NAME})
install (TARGETS main DESTINATION bin)
