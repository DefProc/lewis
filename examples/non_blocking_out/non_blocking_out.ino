#include "lewis.h"
#include <TimerOne.h>

Lewis morse;

void setup() {
  // use the long form of morse.begin to allow interrupts:
  // morse.begin(rx_pin, tx_pin, words_per_minute, use_interrupts)
  morse.begin(2, 9, 20, true);
  Timer1.initialize(10000);
  Timer1.attachInterrupt(morseISR);
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  morse.print("ok");
  for (int i=0; i<4; i++) {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(250);
    digitalWrite(LED_BUILTIN, LOW);
    delay(250);
  }
}

void morseISR()
{
  morse.timerISR();
}
