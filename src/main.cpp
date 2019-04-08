#include "push.h"
#include <chrono>
#include <thread>
#include <iostream>

using namespace std;

int main(int argc, char* argv[]) {
  if (connect()) {
    cout << "Successfully connected" << endl;
  } else {
    cout << "Unable to connect" << endl;
  }
  this_thread::sleep_for(chrono::milliseconds(5000));
  disconnect();
  cout << "Disconnected" << endl;
}
