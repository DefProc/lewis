#include "Lewis.h"

Lewis Morse;

void setup() {
  // single recieve (RX) and transmit (TX) pin (simplex)
  // Lewis will switch the pin to OUTPUT as required to send
  Morse.begin(9);
}

void loop() {
  Morse.print("ok");
  delay(1000);
}
