#include "lewis.h"

Lewis morse;

void setup() {
  // single recieve (RX) and transmit (TX) pin (simplex)
  // Lewis will switch the pin to OUTPUT as required to send
  morse.begin(9);
}

void loop() {
  morse.print("ok");
  delay(1000);
}
