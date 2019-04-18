/*
MIT License

Copyright (c) 2019 Pavel Slama

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef DS2482_H
#define DS2482_H

#include <climits>
#include "mbed.h"

#define DS2482_DEFAULT_ADDRESS (0x18 << 1)

#define DS2482_COMMAND_SINGLEBIT   0x87
#define DS2482_COMMAND_RESETWIRE   0xB4
#define DS2482_COMMAND_CHSL        0xC3
#define DS2482_COMMAND_WRITECONFIG 0xD2
#define DS2482_COMMAND_SRP         0xE1
#define DS2482_COMMAND_RESET       0xF0

#define DS2482_POINTER_CONFIG 0xC3
#define DS2482_POINTER_STATUS 0xF0

#define DS2482_CONFIG_APU (1<<0)
#define DS2482_CONFIG_PPM (1<<1)
#define DS2482_CONFIG_SPU (1<<2)
#define DS2484_CONFIG_WS  (1<<3)

#define DS2482_STATUS_BUSY (1<<0)
#define DS2482_STATUS_PPD  (1<<1)
#define DS2482_STATUS_SD   (1<<2)
#define DS2482_STATUS_LL   (1<<3)
#define DS2482_STATUS_RST  (1<<4)
#define DS2482_STATUS_SBR  (1<<5)
#define DS2482_STATUS_TSB  (1<<6)
#define DS2482_STATUS_DIR  (1<<7)



#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)  \
  (byte & 0x80 ? '1' : '0'), \
  (byte & 0x40 ? '1' : '0'), \
  (byte & 0x20 ? '1' : '0'), \
  (byte & 0x10 ? '1' : '0'), \
  (byte & 0x08 ? '1' : '0'), \
  (byte & 0x04 ? '1' : '0'), \
  (byte & 0x02 ? '1' : '0'), \
  (byte & 0x01 ? '1' : '0')

class DS2482 {
 public:
  typedef enum {
    ActivePullUp = DS2482_CONFIG_APU,
    StrongPullUp = DS2482_CONFIG_SPU,
    OverdriveSpeed = DS2484_CONFIG_WS  // perform reset() after setting this
  } DS2482_config;

  explicit DS2482(I2C * i2c_obj, char address = DS2482_DEFAULT_ADDRESS);
  DS2482(PinName sda, PinName scl, char address = DS2482_DEFAULT_ADDRESS, uint32_t frequency = 400000);
  virtual ~DS2482(void);

  // Device commands
  bool init();
  void device_reset();
  char get_config();
  bool set_config(DS2482_config type);
  bool clear_config(DS2482_config type);
  bool select_channel(char channel);

  // 1-Wire commands
  bool reset();
  char read();
  bool read_bit();
  bool read_bytes(char* data, uint16_t len);
  bool write(char data);
  bool write_bit(bool data);
  bool write_bytes(const char* data, uint16_t len);
  char search(char* addr);
  void reset_search();
  void select(const char rom[8]);

  static uint8_t crc8(const char *addr, uint8_t len);
  void attach(Callback<void(uint8_t)> function);

 private:
  I2C * _i2c;
  uint32_t _i2c_buffer[sizeof(I2C) / sizeof(uint32_t)];
  const char _address;
  char _config;
  Callback<void(uint8_t)> _callback;

  char searchAddress[8];
  char searchLastDiscrepancy;
  char searchLastDeviceFlag;

  char wait_busy();
  bool send_config();
  bool set_read_pointer(char read_pointer);
};

#endif  // DS2482_H