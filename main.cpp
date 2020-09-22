
// http://stackoverflow.com/questions/27581500/hook-into-linux-key-event-handling/27693340#27693340
// vim: shiftwidth=2

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <linux/input.h>
#include <linux/uinput.h>
#include <thread>
#include <chrono>
#include <iostream>
#include "third-party/json.hpp"

using std::cin;
using std::cerr;
using std::string;
using json = nlohmann::json;

#define die(n, str, args...) do { \
  perror(str); \
  exit(n); \
} while(0)

void sendTypeCodeValue(int fdo, int type, int code, int value) {
  struct input_event ev2;
  ev2.time.tv_sec = 0;
  ev2.time.tv_usec = 0;
  ev2.type = type;
  ev2.code = code;
  ev2.value = value;
  write(fdo, &ev2, sizeof(struct input_event));
}

void sendFullSet(int fdo, int code, int value) {
  sendTypeCodeValue(fdo, 4, 4, code);
  sendTypeCodeValue(fdo, 1, code, value);
  sendTypeCodeValue(fdo, 0, 0, 0);
}

int main(int argc, char **argv) {
  int fdo = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
  if (fdo < 0) die(3, "error: open");

  if (ioctl(fdo, UI_SET_EVBIT, EV_SYN) < 0) die(5, "error: ioctl");
  if (ioctl(fdo, UI_SET_EVBIT, EV_KEY) < 0) die(6, "error: ioctl");
  if (ioctl(fdo, UI_SET_EVBIT, EV_MSC) < 0) die(7, "error: ioctl");

  for (int i=0; i<KEY_MAX; ++i) {
    if (ioctl(fdo, UI_SET_KEYBIT, i) < 0) die(8, "error: ioctl");
  }
  
  uinput_user_dev uidev;
  memset(&uidev, 0, sizeof(uidev));
  snprintf(uidev.name, UINPUT_MAX_NAME_SIZE, "key_event_server");
  uidev.id.bustype = BUS_USB;
  uidev.id.vendor  = 0x1;
  uidev.id.product = 0x1;
  uidev.id.version = 1;

  if (write(fdo, &uidev, sizeof(uidev)) < 0) die(9, "error: write");
  if (ioctl(fdo, UI_DEV_CREATE) < 0) die(10, "error: ioctl");
  
  // const int pressed = 1;
  // const int released = 0;
  // const int repeated = 2;
  
  for (;;) {
    string line;
    std::getline(cin, line);

    if (line.length() == 0 && !cin) {
      break;
    }
      
    json j;
    try {
      j = json::parse(line);
    }
    catch (json::parse_error const& e) {
      cerr << "Bad input: parse error:" << e.what() << "\n"; cerr.flush();
      goto failed;
    }
    
    if (!j.is_array()) {
      cerr << "Bad input: must be array.\n"; cerr.flush();
      goto failed;
    }
    
    for (json jx : j) {
      if (!jx.is_object()) {
        cerr << "Bad input: array elements must be structs.\n"; cerr.flush();
        goto failed;
      }
      
      if (jx.count("code") == 0) {
        cerr << "Bad input: array elements must have a 'code' member.\n"; cerr.flush();
        goto failed;
      }
      
      if (jx.count("value") == 0) {
        cerr << "Bad input: array elements must have a 'value' member.\n"; cerr.flush();
        goto failed;
      }
    }
    
    for (json jx : j) {
      int code = jx["code"];
      int value = jx["value"];
      
      cerr << "Sending " << code << " " << value << "\n"; cerr.flush();
      
      sendFullSet(fdo, code,  value);
    }
    
    failed: ;
  }

  if (ioctl(fdo, UI_DEV_DESTROY) < 0) die(12, "error: ioctl");
  close(fdo);

  return 0;
}

