
#include <inttypes.h>
#include "Arduino.h"

#include "lewis.h"

void Lewis::begin(uint8_t rx_pin, uint8_t tx_pin, uint8_t words_per_minute)
{
  _rx_pin = rx_pin;
  _tx_pin = tx_pin;
  _baud = 1200/words_per_minute;

  if (_rx_pin != _tx_pin) {
    pinMode(_tx_pin, OUTPUT);
    digitalWrite(_tx_pin, LOW);
    _samePin = false;
  }
  pinMode(_rx_pin, INPUT);
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
        Serial.print(targetLocation);
        break;
      }
    }

    if (targetLocation == START_INDEX*2) {
      // the selected character was not found in the lookup table
      return 1;
    }

    _morseIndex = START_INDEX;
    _indexJump = START_INDEX/2;

    if (_samePin == true) pinMode(_tx_pin, OUTPUT);

    while (_morseIndex != targetLocation) {
      if (_morseIndex < targetLocation) {
        // do a dash
        digitalWrite(_tx_pin, HIGH);
        delay(_baud*3);
        digitalWrite(_tx_pin, LOW);
        delay(_baud);
        // move the pointer right
        _morseIndex = _morseIndex + _indexJump;
      } else {
        // do a dot
        digitalWrite(_tx_pin, HIGH);
        delay(_baud);
        digitalWrite(_tx_pin, LOW);
        delay(_baud);
        // move the pointer left
        _morseIndex = _morseIndex - _indexJump;
      }
      if (_indexJump == 1) {
        break;
      } else {
        _indexJump = _indexJump/2;
      }
    }
    delay(_baud);

    if (_samePin == true) pinMode(_rx_pin, INPUT);

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
