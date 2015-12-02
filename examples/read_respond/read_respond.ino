#include "lewis.h"

Lewis morse;

void setup() {
  // single recieve (RX) and transmit (TX) pin (simplex)
  // Lewis will switch the pin to OUTPUT as required to send
  morse.begin(9);
}

void loop() {
  // call checkIncoming often enough to catch the incoming presses
  morse.checkIncoming();

  if (morse.available()) {
    char c = morse.read();
    if (c == 'k') {
      delay(100);
      morse.print("ok");
    }
  }
}
