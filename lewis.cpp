
#include <inttypes.h>
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
}

int Lewis::available(void)
{
  int num_chars = 0;

  if (_rx_buffer_head != _rx_buffer_tail) {
    // count the number of inter-letter spaces up to the head
    rx_buffer_index_t position = _rx_buffer_tail;
    while (position != _rx_buffer_head) {
      position = (position +1) % MORSE_RX_BUFFER_SIZE;
      // show the buffer for debugging:
      /*
      Serial.print(position);
      Serial.write(':');
      Serial.print(_rx_buffer[position]);
      Serial.write(',');
      */
      if (_rx_buffer[position] == INTERLETTER_SPACE || _rx_buffer[position] == INTERWORD_SPACE) num_chars++;
    }
    // show the buffer for debugging:
    /*
    Serial.println();
    Serial.println(num_chars);
    */
  }

  return num_chars;
}

int Lewis::peek(void)
{
  if (_rx_buffer_head == _rx_buffer_tail) {
    return -1;
  } else {
    return parseMorse(false);
  }
}

int Lewis::read(void)
{
  // if the head isn't ahead of the tail, we don't have any characters
  if (_rx_buffer_head == _rx_buffer_tail) {
    return -1;
  } else {
    return parseMorse(true);
  }
}

char Lewis::parseMorse(bool advance_position)
{
  // do the actual parsing
  // store the inital buffer tail position for later
  rx_buffer_index_t current_tail = _rx_buffer_tail;
  // and increment the position to find the next instruction
  current_tail = (current_tail + 1) % MORSE_RX_BUFFER_SIZE;

  // if we're at an inter-word space, send a space
  if (_rx_buffer[current_tail] == INTERWORD_SPACE) {
    // only record the new buffer position if we're reading
    if (advance_position == true) _rx_buffer_tail = current_tail;
    return ' ';
  }

  // otherwise lets start traversing the lookup table
  _morseIndex = START_INDEX;
  _indexJump = START_INDEX/2;

  while (_rx_buffer[current_tail] != _rx_buffer[_rx_buffer_head]) {
    // there should be something here to better handle a buffer overrun
    // as it is, we'll just take what ever result that gives us.
    if (_rx_buffer[current_tail] == DOT) {
      _morseIndex = _morseIndex - _indexJump;
      _indexJump = _indexJump/2;
    } else if (_rx_buffer[current_tail] == DASH) {
      _morseIndex = _morseIndex + _indexJump;
      _indexJump = _indexJump/2;
    } else {
      break;
    }
    current_tail = (current_tail + 1) % MORSE_RX_BUFFER_SIZE;
  }

  if (advance_position == true) _rx_buffer_tail = current_tail;

  // if there's an invalid character, this will return a tilde '~'
  return _morseLookup[_morseIndex];
}

size_t Lewis::write(uint8_t c)
{
  uint8_t targetLocation = 0;
  for (targetLocation=0; targetLocation<=START_INDEX*2; targetLocation++) {
    if (c == _morseLookup[targetLocation]) {
      break;
    }
  }

  if (targetLocation >= START_INDEX*2) {
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

  // debugging
  //Serial.print(targetLocation);

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

  // show the buffer for debugging:
  /*
  for (int i=_tx_buffer_tail; i != _tx_buffer_head; i = (i+1) % MORSE_TX_BUFFER_SIZE) {
    Serial.print(i);
    Serial.write(':');
    Serial.print(_tx_buffer[i]);
    Serial.write(',');
  }
  */

  return 0;
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
  if (_samePin) {
    // if using one pin:
    if (!_transmitting && _tx_buffer_head != _tx_buffer_tail) {
      // block recieving when we have something in the transmit buffer
      _transmitting = true;
      pinMode(_rx_pin, OUTPUT);
    } else if (_transmitting && _tx_buffer_head == _tx_buffer_tail){
      // go back to listening when the tx buffer is empty
      _transmitting = false;
      pinMode(_rx_pin, INPUT);
    }
  }

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

  if (!_transmitting) {
    checkIncoming();
  }
}

void Lewis::checkIncoming()
{
  uint32_t current_time = millis();
  uint8_t current_state = digitalRead(_rx_pin);
  // we only need to do something on change or a long duration low
  if (_rx_state == LOW) {
    // have we been low long enough for an inter-word/letter space?
    if (_rx_buffer[_rx_buffer_head] == INTERLETTER_SPACE && current_time - _last_rx >= _pulse_duration * (DOT + INTERLETTER_SPACE + INTERWORD_SPACE)) {
      // it's been an inter-word space (must follow an inter letter space)
      _rx_buffer_head = (_rx_buffer_head + 1) % MORSE_RX_BUFFER_SIZE;
      _rx_buffer[_rx_buffer_head] = INTERWORD_SPACE;
    } else if ((_rx_buffer[_rx_buffer_head] == DOT || _rx_buffer[_rx_buffer_head] == DASH) && current_time - _last_rx >= _pulse_duration * (DOT + INTERLETTER_SPACE)) {
      // it's been an inter-letter space (must follow a dot or dash)
      _rx_buffer_head = (_rx_buffer_head + 1) % MORSE_RX_BUFFER_SIZE;
      _rx_buffer[_rx_buffer_head] = INTERLETTER_SPACE;
    }

    if (current_state == HIGH) {
      // mark when the pin went high
      _last_rx = current_time;
      _rx_state = current_state;
    }
  } else if (_rx_state == HIGH && current_state == LOW) {
    // the falling end of a pulse
    uint32_t pulse_time = current_time - _last_rx;
    if (pulse_time >= _pulse_duration * DASH) {
      //record a dash
      _rx_buffer_head = (_rx_buffer_head + 1) % MORSE_RX_BUFFER_SIZE;
      _rx_buffer[_rx_buffer_head] = DASH;
      _last_rx = current_time;
      _rx_state = current_state;
    } else if (pulse_time >= _pulse_duration/2){
      // record a dot
      _rx_buffer_head = (_rx_buffer_head + 1) % MORSE_RX_BUFFER_SIZE;
      _rx_buffer[_rx_buffer_head] = DOT;
      _last_rx = current_time;
      _rx_state = current_state;
    }
  }
}
