#include "lewis.h"
#include <TimerOne.h>

Lewis morse;

void setup() {
  morse.begin(2, 9, 20, true);
  Timer1.initialize(10000);
  Timer1.attachInterrupt(morseISR);
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  morse.print("ok");
  for (int i=0; i<4; i++) {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(500);
    digitalWrite(LED_BUILTIN, LOW);
    delay(500);
  }
}

void morseISR()
{
  morse.timerISR();
}
