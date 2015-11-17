#ifndef Lewis_h
#define Lewis_h

#include <inttypes.h>
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

class Lewis : public Stream
{
  protected:
    volatile rx_buffer_index_t _rx_buffer_head;
    volatile rx_buffer_index_t _rx_buffer_tail;
    volatile tx_buffer_index_t _tx_buffer_head;
    volatile tx_buffer_index_t _tx_buffer_tail;

    // Don't put any members after these buffers, since only the first
    // 32 bytes of this struct can be accessed quickly using the ldd
    // instruction.
    unsigned long _rx_buffer[SERIAL_RX_BUFFER_SIZE];
    unsigned char _tx_buffer[SERIAL_TX_BUFFER_SIZE];

    int _rx_pin;
    int _tx_pin;
    int _baud;

    char parseMorse();

  public:
    void begin(int rx_pin) { begin(x_pin, rx_pin); }
    void begin(uint8_t rx_pin, uint8_t tx_pin, uint8_t words_per_minute = 20);
    int available(void);
    int read(void);
    int peak(void);
    int flush(void);
    int flushTX(void);
    int flushRX(void);
    size_t write();

    Stream() {_timeout=2000;}
}
