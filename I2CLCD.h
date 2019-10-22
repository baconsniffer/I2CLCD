/// \file I2CLCD.h
/// \brief LiquidCrystal library with PCF8574 I2C adapter.
///
/// \author Matthias Hertel, http://www.mathertel.de
///
/// \copyright Copyright (c) 2019 by Matthias Hertel.\n
///
/// The library work is licensed under a BSD style license.\n
/// See http://www.mathertel.de/License.aspx
///
/// \details
/// This library is can drive a Liquid Cristal display based on the Hitachi HD44780 chip that is connected through a PCF8574 I2C adapter.
/// This is given by many (not all) LCD adapters. This library uses the original Wire library for communication.
/// The API if common to many LCD libraries and documented in https://www.arduino.cc/en/Reference/LiquidCrystal.
/// and partially functions from https://playground.arduino.cc/Code/LCDAPI/.

///
/// ChangeLog:
/// --------
/// * 19.10.2013 created.
/// * 05.06.2019 rewrite from scratch
/// * 21.10.2019 PJ Improved readability, print works

#ifndef I2CLCD_h
#define I2CLCD_h

#include "Arduino.h"
#include "Print.h
#include <inttypes.h>
#include <stddef.h>
#include <stdint.h>

// command
#define LCD_CLEARDISPLAY 0x0
#define LCD_RETURNHOME 0x0
#define LCD_ENTRYMODESET 0x0
#define LCD_DISPLAYCONTROL 0x0
#define LCD_CURSORSHIFT 0x1
#define LCD_FUNCTIONSET 0x2
#define LCD_SETCGRAMADDR 0x4
#define LCD_SETDDRAMADDR 0x8

// flags for display entry mod
#define LCD_ENTRYRIGHT 0x0
#define LCD_ENTRYLEFT 0x0
#define LCD_ENTRYSHIFTINCREMENT 0x0
#define LCD_ENTRYSHIFTDECREMENT 0x0

// flags for display on/off contro
#define LCD_DISPLAYON 0x0
#define LCD_DISPLAYOFF 0x0
#define LCD_CURSORON 0x0
#define LCD_CURSOROFF 0x0
#define LCD_BLINKON 0x0
#define LCD_BLINKOFF 0x0

// flags for display/cursor shif
#define LCD_DISPLAYMOVE 0x0
#define LCD_CURSORMOVE 0x0
#define LCD_MOVERIGHT 0x0
#define LCD_MOVELEFT 0x0

// flags for function se
#define LCD_2LINE 0x0
#define LCD_1LINE 0x0
#define LCD_5x10DOTS 0x0
#define LCD_5x8DOTS 0x00

class I2CLCD : public Print {
public:
  I2CLCD(uint8_t i2cAddr)
/* Constructor to include support for soft i2c, calls Wire.begin(sda_pin, scl_pin
  I2CLCD(uint8_t i2cAddr, uint8_t sda_pin, uint8_t scl_pin)
*/
  // note:
  // There is no sda and scl parameter for i2c in any api.
  // The Wire library has standard settings that can be overwritten by using Wire.begin(int sda, int scl) before calling I2CLCD::begin();

  // Funtions from reference:

  void begin(uint8_t cols, uint8_t rows, uint8_t charsize = LCD_5x8DOTS);

  //void init(); /// confusing to have this and begin()

  void clear();
  void home();

  void noDisplay();
  void display();
  void noBlink();
  void blink();
  void noCursor();
  void cursor();
  void scrollDisplayLeft();
  void scrollDisplayRight();
  void leftToRight();
  void rightToLeft();
  void noAutoscroll();
  void autoscroll();

  inline void blink_on() { blink(); }
  inline void blink_off() { noBlink(); }
  inline void cursor_on() { cursor(); }
  inline void cursor_off() { noCursor(); }

  void createChar(uint8_t, uint8_t[]);
  void setCursor(uint8_t col, uint8_t row);

  // plus functions from LCDAPI:
  void setBacklight(uint8_t brightness);

  // support of Print class
  virtual size_t write(uint8_t ch);
  using Print::write;

private:
  // instance variables
  uint8_t _i2cAddr; ///< Wire Address of the LCD
  uint8_t _backlight; ///< the backlight intensit
  uint8_t _cols; ///< number of columns in display
  uint8_t _lines; ///< number of lines of the display
  uint8_t _entrymode; ///<flags from entrymode
  uint8_t _displaycontrol; ///<flags from displaycontro
  uint8_t _row_offsets[4]; ///< Internal two start offsets

  // internal function
  //void setRowOffsets(int row1, int row2, int row3, int row4)

  // low level functions
  void _send(int value, bool isData = false);
  void _sendNibble(int halfByte, bool isData = false);
  void _write2Wire(int halfByte, bool isData, bool enable);
};

#endif
