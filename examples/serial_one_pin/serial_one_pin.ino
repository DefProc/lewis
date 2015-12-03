#include "Lewis.h"
#include <TimerOne.h>

Lewis Morse;

void setup() {
  // open the serial port
  Serial.begin(9600);
  // wait for serial port to connect on 32u4 boards (eg. Leonardo)
  while (!Serial){;}
  // use the long form of Morse.begin to allow interrupts:
  // Morse.begin(rx_pin, tx_pin, words_per_minute, use_interrupts)
  Morse.begin(9, 9, 20, true);
  // use the TimerOne library for a 100Hz timer interrupt
  Timer1.initialize(10000);
  Timer1.attachInterrupt(MorseISR);
}

void loop() {
  // send each serial byte to Morse output
  if (Serial.available()) {
    int inByte = Serial.read();
    Morse.write(inByte);
  }

  // send each Morse byte to serial port
  if (Morse.available()) {
    int inByte = Morse.read();
    Serial.write(inByte);
  }
  delay(500);
}

// your own defined interrupt function that includes the Morse.timerISR call
void MorseISR()
{
  Morse.timerISR();
}
