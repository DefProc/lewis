#include "Lewis.h"

Lewis Morse;

void setup() {
  // single recieve (RX) and transmit (TX) pin (simplex)
  // Lewis will switch the pin to OUTPUT as required to send
  Morse.begin(9);
}

void loop() {
  // call checkIncoming often enough to catch the incoming presses
  Morse.checkIncoming();

  if (Morse.available()) {
    char c = Morse.read();
    if (c == 'k') {
      delay(100);
      Morse.print("ok");
    }
  }
}
