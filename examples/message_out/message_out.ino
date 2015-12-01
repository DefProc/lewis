#include "lewis.h"

Lewis morse;

void setup() {
  morse.begin(2, 9);
}

void loop() {
  morse.print("ok");
  delay(1000);
}
