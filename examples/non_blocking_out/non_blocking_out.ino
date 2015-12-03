#include "Lewis.h"
#include <TimerOne.h>

Lewis Morse;

void setup() {
  // use the long form of Morse.begin to allow interrupts:
  // Morse.begin(rx_pin, tx_pin, words_per_minute, use_interrupts)
  Morse.begin(2, 9, 20, true);
  Timer1.initialize(10000);
  Timer1.attachInterrupt(myISR);
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  Morse.print("ok");
  for (int i=0; i<4; i++) {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(250);
    digitalWrite(LED_BUILTIN, LOW);
    delay(250);
  }
}

void myISR()
{
  Morse.timerISR();
}
