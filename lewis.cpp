
#include <inttypes.h>
//#include "TimerOne.h"
#include "Arduino.h"

#include "lewis.h"

void Lewis::begin(uint8_t rx_pin, uint8_t tx_pin, uint8_t words_per_minute, uint8_t use_interrupts)
{
  _rx_pin = rx_pin;
  _tx_pin = tx_pin;
  _pulse_duration = 1200/words_per_minute;
  _use_interrupts = use_interrupts;

  if (_rx_pin != _tx_pin) {
    pinMode(_tx_pin, OUTPUT);
    digitalWrite(_tx_pin, LOW);
    _samePin = false;
  }
  pinMode(_rx_pin, INPUT);

  if (_use_interrupts == true) {
    //Timer1.initialize(10000);
    //Timer1.attachInterrupt(timerISR());
  }
}

int Lewis::available(void)
{
  return ((unsigned int)(MORSE_RX_BUFFER_SIZE + _rx_buffer_head - _rx_buffer_tail)) % MORSE_RX_BUFFER_SIZE;
}

int Lewis::peek(void)
{
  if (_rx_buffer_head == _rx_buffer_tail) {
    return -1;
  } else {
    return parseMorse();
  }
}

int Lewis::read(void)
{
  // if the head isn't ahead of the tail, we don't have any characters
  if (_rx_buffer_head == _rx_buffer_tail) {
    return -1;
  } else {
    unsigned char c = parseMorse();
    _rx_buffer_tail = (rx_buffer_index_t)(_rx_buffer_tail + 1) % MORSE_RX_BUFFER_SIZE;
    return c;
  }
}

char Lewis::parseMorse()
{
  // do the actual parsing
  return 'a';
}

size_t Lewis::write(uint8_t c)
{
  //if (_tx_buffer_head == _tx_buffer_tail) {
    //return 1;
  //} else {
    //do the actual pulseout
    //...
    uint8_t targetLocation = 0;
    for (targetLocation=0; targetLocation<=START_INDEX*2; targetLocation++) {
      if (c == _morseLookup[targetLocation]) {
        //Serial.print(targetLocation);
        break;
      }
    }

    if (targetLocation == START_INDEX*2) {
      // deal with a few extra characters without having to double the lookup table length
      if (c == ' ' || c == '\n' || c == '\r') {
        // translate spaces or newlines to an interword space
        interwordSpace();
        return 0;
      } else if (c == '.') {
        // use the six pulse .
        dot(); dash(); dot(); dash(); dot(); dash(); interletterSpace();
      } else {
        // the selected character was not found in the lookup table
        return 1;
      }
    }

    _morseIndex = START_INDEX;
    _indexJump = START_INDEX/2;

    if (_use_interrupts == false) {
      // if outputting the letter directly (blocking)
      if (_samePin == true) pinMode(_tx_pin, OUTPUT);
    }

    while (_morseIndex != targetLocation) {
      if (_morseIndex < targetLocation) {
        // do a dash
        dash();
        // move the pointer right
        _morseIndex = _morseIndex + _indexJump;
      } else {
        // do a dot
        dot();
        // move the pointer left
        _morseIndex = _morseIndex - _indexJump;
      }
      if (_indexJump == 1) {
        break;
      } else {
        _indexJump = _indexJump/2;
      }
    }
    interletterSpace();

    if (_use_interrupts == false) {
      // if outputting the letter directly (blocking)
      if (_samePin == true) pinMode(_rx_pin, INPUT);
    }

    return 0;
  //}
}

void Lewis::flush()
{
  // clear both buffers
  flushTX();
  flushRX();
}

void Lewis::flushTX()
{
  _tx_buffer_tail = _tx_buffer_head;
}

void Lewis::flushRX()
{
  _rx_buffer_tail = _rx_buffer_head;
}

void Lewis::dot()
{
  if (_use_interrupts == true) {
    _tx_buffer_head = (_tx_buffer_head + 1) % MORSE_TX_BUFFER_SIZE;
    _tx_buffer[_tx_buffer_head] = DOT;
  } else {
    digitalWrite(_tx_pin, HIGH);
    delay(_pulse_duration*DOT);
    digitalWrite(_tx_pin, LOW);
    delay(_pulse_duration);
  }
}

void Lewis::dash()
{
  if (_use_interrupts == true) {
    _tx_buffer_head = (_tx_buffer_head + 1) % MORSE_TX_BUFFER_SIZE;
    _tx_buffer[_tx_buffer_head] = DASH;
  } else {
    digitalWrite(_tx_pin, HIGH);
    delay(_pulse_duration*DASH);
    digitalWrite(_tx_pin, LOW);
    delay(_pulse_duration);
  }
}

void Lewis::interletterSpace()
{
  if (_use_interrupts == true) {
    _tx_buffer_head = (_tx_buffer_head + 1) % MORSE_TX_BUFFER_SIZE;
    _tx_buffer[_tx_buffer_head] = INTERLETTER_SPACE;
  } else {
    digitalWrite(_tx_pin, LOW);
    delay(_pulse_duration*INTERLETTER_SPACE);
  }
}

void Lewis::interwordSpace()
{
  if (_use_interrupts == true) {
    _tx_buffer_head = (_tx_buffer_head + 1) % MORSE_TX_BUFFER_SIZE;
    _tx_buffer[_tx_buffer_head] = INTERWORD_SPACE;
  } else {
    digitalWrite (_tx_pin, LOW);
    delay(_pulse_duration*INTERWORD_SPACE);
  }
}

void Lewis::timerISR()
{
  volatile unsigned long time_now = millis();
  if (_tx_buffer_tail != _tx_buffer_head && (time_now >= _next_tx)) {//} || time_now - _next_tx >= time_now)) {
    // there is data to send out (the tail hasn't caught up with the head)
    // AND the time of next transmit change has passed
    if (_tx_state == LOW) {
      if (_tx_buffer[_tx_buffer_tail] == DOT || _tx_buffer[_tx_buffer_tail] == DASH) {
        // change to high if DOT or DASH
        // and set the _next_tx depending on length
        _tx_state = HIGH;
        digitalWrite(_tx_pin, _tx_state);
        _next_tx = (time_now + (_pulse_duration * _tx_buffer[_tx_buffer_tail]));
        // but don't move the buffer position, we need the trailing rest first
      } else {
        // it's a rest, keep low and wait for the next character
        _tx_state = LOW;
        digitalWrite(_tx_pin, _tx_state);
        _next_tx = (time_now + (_pulse_duration * _tx_buffer[_tx_buffer_tail]));
        // and increment the buffer tail position
        _tx_buffer_tail = (_tx_buffer_tail + 1) % MORSE_TX_BUFFER_SIZE;
      }
    } else {
      // it's a high state, and we need to make the trailing rest
      _tx_state = LOW;
      digitalWrite(_tx_pin, _tx_state);
      _next_tx = (time_now + _pulse_duration);
      // and increment the buffer tail position
      _tx_buffer_tail = (_tx_buffer_tail + 1) % MORSE_TX_BUFFER_SIZE;
    }
  }
}
