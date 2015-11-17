
#include <inttypes.h>
#include "Arduino.h"

#include "lewis.h"

void Lewis::begin(rx_pin, tx_pin, words_per_minute)
{
  _rx_pin = rx_pin;
  _tx_pin = tx_pin;
  _baud = 1200/words_per_minute;
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

size_t Lewis::write()
{
  if (_tx_buffer_head == _tx_buffer_tail) {
    return 1;
  } else {
    //do the actual pulseout
    //...
    return 0;
  }
}

int Lewis::flush()
{
  // clear both buffers
  flushTX();
  flushRX();
}

int Lewis::flushTX()
{
  _tx_buffer_tail = _tx_buffer_head;
}

int Lewis::flushRX()
{
  _rx_buffer_tail = _rx_buffer_head;
}
