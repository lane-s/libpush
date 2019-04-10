#include "push.h"
#include <chrono>
#include <thread>
#include <iostream>

using namespace std;

int main(int argc, char* argv[]) {
  if (libpush_connect(true)) {
    cout << "Successfully connected" << endl;
    this_thread::sleep_for(chrono::milliseconds(5000));
    libpush_disconnect();
    cout << "Disconnected" << endl;
  } else {
    cout << "Unable to connect" << endl;
  }
}
