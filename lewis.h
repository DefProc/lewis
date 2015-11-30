#ifndef Lewis_h
#define Lewis_h

#include <inttypes.h>
#include <Arduino.h>
#include "Stream.h"

#if !(defined(MORSE_TX_BUFFER_SIZE) && defined(MORSE_RX_BUFFER_SIZE))
#if (RAMEND < 1000)
#define MORSE_TX_BUFFER_SIZE 16
#define MORSE_RX_BUFFER_SIZE 16
#else
#define MORSE_TX_BUFFER_SIZE 64
#define MORSE_RX_BUFFER_SIZE 64
#endif
#endif
#if (MORSE_TX_BUFFER_SIZE>256)
typedef uint16_t tx_buffer_index_t;
#else
typedef uint8_t tx_buffer_index_t;
#endif
#if  (MORSE_RX_BUFFER_SIZE>256)
typedef uint16_t rx_buffer_index_t;
#else
typedef uint8_t rx_buffer_index_t;
#endif

#define START_INDEX 32

class Lewis : public Stream
{
  protected:
    volatile rx_buffer_index_t _rx_buffer_head;
    volatile rx_buffer_index_t _rx_buffer_tail;
    volatile tx_buffer_index_t _tx_buffer_head;
    volatile tx_buffer_index_t _tx_buffer_tail;

    uint8_t _rx_pin;
    uint8_t _tx_pin;
    uint8_t _samePin = true;
    uint8_t _baud;

    char parseMorse();
    uint8_t _morseIndex = START_INDEX;
    uint8_t _indexJump = START_INDEX/2;
    //                   |0                         start:↓                              63|
    char* _morseLookup = "~5h4s~v3I~f~u~~2e~l~r+~~a~p~w~j1~6b=d/x~n~c~k~y~t7z~g~q~m8~~o9~0";

    // Don't put any members after these buffers, since only the first
    // 32 bytes of this struct can be accessed quickly using the ldd
    // instruction.
    unsigned long _rx_buffer[MORSE_RX_BUFFER_SIZE];
    unsigned char _tx_buffer[MORSE_TX_BUFFER_SIZE];

  public:
    void begin(int rx_pin) { return begin(rx_pin, rx_pin); }
    void begin(uint8_t rx_pin, uint8_t tx_pin, uint8_t words_per_minute = 20);
    int available(void);
    int read(void);
    int peek();
    void flush();
    void flushTX();
    void flushRX();
    size_t write(uint8_t);

    //Stream() {_timeout=2000;}
};

#endif
