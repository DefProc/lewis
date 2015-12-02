#include "lewis.h"
#include <TimerOne.h>

Lewis morse;

void setup() {
  // open the serial port
  Serial.begin(9600);
  // wait for serial port to connect on 32u4 boards (eg. Leonardo)
  while (!Serial){;}
  // use the long form of morse.begin to allow interrupts:
  // morse.begin(rx_pin, tx_pin, words_per_minute, use_interrupts)
  morse.begin(2, 9, 20, true);
  // use the TimerOne library for a 100Hz timer interrupt
  Timer1.initialize(10000);
  Timer1.attachInterrupt(morseISR);
}

void loop() {
  // send each serial byte to morse output
  if (Serial.available()) {
    int inByte = Serial.read();
    morse.write(inByte);
  }

  // send each morse byte to serial port
  if (morse.available()) {
    int inByte = morse.read();
    Serial.write(inByte);
  }
  delay(500);
}

// your own defined interrupt function that includes the morse.timerISR call
void morseISR()
{
  morse.timerISR();
}
