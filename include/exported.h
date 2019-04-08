#pragma once

// Define EXPORTED for any platform
#if defined _WIN32 || defined __CYGWIN__
  #ifdef WIN_EXPORT
    #ifdef __GNUC__
      #define EXPORTED __attribute__ ((dllexport))
    #else
      #define EXPORTED __declspec(dllexport)
    #endif
  #else
    #ifdef __GNUC__
      #define EXPORTED __attribute__ ((dllimport))
    #else
      #define EXPORTED __declspec(dllimport)
    #endif
  #endif
  #define NOT_EXPORTED
#else
  #if __GNUC__ >= 4
    #define EXPORTED __attribute__ ((visibility ("default")))
    #define NOT_EXPORTED  __attribute__ ((visibility ("hidden")))
  #else
    #define EXPORTED
    #define NOT_EXPORTED
  #endif
#endif
