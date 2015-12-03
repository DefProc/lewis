Lewis
=====

A morse code interpreter: making your [Morse](https://en.wikipedia.org/wiki/Inspector_Morse_(TV_series)) easier to use and process.

Lewis is a Serial-type library for converting variable data to morse code output, or converting morse code input into strings and numbers. You can use Lewis to send and receive morse code from your Arduino, and even allow you to converse, send instructions, get status or transfer data using one pin (simplex) or two for full duplex communication.

Lewis can be used either as a set of blocking functions (pulsing out will stop any other commands until finished) or contains an interrupt routine for you to call or insert where ever suits your program best, using the timer, pin change or hardware interrupt that you have free.

Using Lewis
-----------

However you use Lewis, after installing the library (e.g. using the [library manager](https://www.arduino.cc/en/Guide/Libraries#toc2) to import it), you will have to included the library header file at the top of your sketch:

```
#include "Lewis.h"
```

And instantiate the Lewis class of functions with a name that you use later. I use `Morse` but you can choose whatever name you like, as long as you are consistent through the sketch:

```
Lewis Morse;
```

Using Lewis requires calling the begin function in `setup()`, to choose the input and output pins, the pulse rate, and if you will be using the interrupt function:

```
  Morse.begin(rx_pin, tx_pin, words_per_minute, use_interrupts);
  // e.g.
  Morse.begin(2, 9, 20, false);
  // you can specify the same input and output pin and Lewis will
  // switch modes as necessary
  Morse.begin(9, 9, 20, false);
  // or uses the short version that assumes 20WPM and no interrupts:
  Morse.begin(9);
  // if interrupts are required, use the long version:
  Morse.begin(9, 9, 20, true);
```

Sending morse code
------------------

Lewis uses the same function types as the [Serial library](https://www.arduino.cc/en/Reference/Serial) so data can be sent using the `print` and `write` commands:

```
  // from the message_out example
  Morse.print("ok"); // will send: ---/-.-
  Morse.write('s'); // will send: ...
```

in the default initiation, the output code will be “blocking” — that is, it will stop at the print line while until it has finished sending the full code. This is fine for small implementations, but for more complex code, you'll want to keep running your main code, and have the morse code sent in the background.

If you call `begin` with `use_interrupts` set as `true`, then you will need to call `Morse.timerISR()` frequently enough that it can check if any dots or dashes need sending out, without the listener hearing any inaccuracy in the tone length. 100Hz seems to work nicely.

Lewis does not include attach a timer interrupt when initialised (so as not to clash with anything you're already doing), so you will need to include one. The examples use the [TimerOne](https://github.com/PaulStoffregen/TimerOne) library for the interrupt.

```
// from the non_blocking_out example:

// additionally include the TimerOne library
#include <TimerOne.h>

void setup() {
  // start Lewis with interrupts
  Morse.begin(2, 9, 20, true);
  // start TimerOne running at 100Hz
  Timer1.initialize(10000);
  // call the MorseISR function from below, each time the timer fires
  Timer1.attachInterrupt(myISR);
}

// create a function that calls the Lewis ISR
void myISR()
{
  // inclued this function to actually output morse code, and check for input
  Morse.timerISR();
}
```

Receiving morse
---------------

Lewis needs to watch the receiving pin of your choice for it's LOW→HIGH changes often enough that it can see every change and accurately time the duration between each one to decode the dots, dashes, inter-letter spaces and inter-word spaces. To do so, your script will have do call the `checkIncoming` function rapidly enough (ideally 100 times or more per second) to pass decode the morse stream and pass them into the receiving buffer.

If you're using the interrupt initialisation of Lewis, then `checkIncoming` is already included in the `timerISR` function, so Lewis will already be watching your rx pin for button presses. If you're using the standard, non-interrupting initialisation then just putting `Morse.checkIncoming()` in a fast loop will be enough.

```
//from the read_respond example
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
```

With such a small loop, the checking function is not likely to miss any button press changes, but you could also run `checkIncoming` as part of a [hardware interrupt](https://www.arduino.cc/en/Reference/AttachInterrupt) or [pin change interrupt](http://www.geertlangereis.nl/Electronics/Pin_Change_Interrupts/PinChange_en.html) if necessary.

To process data that's in the receive buffer, you can check if there is data, and process that as you normally would with Serial input:

```
// first check if there's anything to process:
if (Morse.available();) { // available returns the number of characters available
  // read grabs the next character from the buffer:
  char c = Morse.read();
  // or peek checks the next character, but without moving the buffer index forward
  char p = Morse.peek();
  // any following call of peek (or the next read) will still return the same letter
}
```
