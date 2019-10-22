/// \file I2CLCD.cpp
/// \brief LiquidCrystal library with PCF8574 I2C adapter.
///
/// \author Matthias Hertel, http://www.mathertel.de
/// \copyright Copyright (c) 2019 by Matthias Hertel
/// \changelog Update 2019-10-21 by SnifferJ
///
/// ChangeLog see: I2CLCD.h

#include "I2CLCD.h"

#include <Wire.h>

/// Definitions on how the PCF8574 is connected to the LCD

/// These are Bit-Masks for the special signals and background light
#define PCF_RS 0x01
#define PCF_RW 0x02
#define PCF_EN 0x04
#define PCF_BACKLIGHT 0x08
// the 0xF0 bits are used for 4-bit data to the display.

// a nibble is a half Byte

I2CLCD::I2CLCD(uint8_t i2cAddr)
{
  _i2cAddr = i2cAddr;
  _backlight = 0;

  _entrymode = LCD_ENTRYLEFT; // like Initializing by Internal Reset Circuit
  _displaycontrol = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF; //0x04;
} // I2CLCD


void I2CLCD::begin(uint8_t cols, uint8_t lines, uint8_t dotsize) {
  _cols = cols;
  _lines = lines;

  _row_offsets[0] = 0x00; _row_offsets[1] = 0x40
  _row_offsets[2] = 0x00 + _cols; _row_offsets[3] = 0x40 + _cols

  uint8_t functionFlags = 0


  if (lines > 1) {
    functionFlags |= LCD_2LINE;
  

  // some 1 line displays can select a 10 pixel high fon
  if ((dotsize != LCD_5x8DOTS) && (_lines == 1)) 
    functionFlags |= LCD_5x10DOTS
  

  // initializing the display
  Wire.begin();
  _write2Wire(0x00, LOW, false)
  // according to datasheet, we need at least 40ms after power rises above 2.7
  // before sending commands. Arduino can turn on way before 4.5V so we'll wait 50
  delayMicroseconds(50000);

  // after reset the mode is this
  _displaycontrol = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
  _entrymode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;

  // sequence to reset. see "Initializing by Instruction" in datatsheet
  _sendNibble(0x03);
  delayMicroseconds(4500);
  _sendNibble(0x03);
  delayMicroseconds(200);
  _sendNibble(0x03);
  delayMicroseconds(200);
  _sendNibble(0x02);   // finally, set to 4-bit interface

  // Set # of lines, font size, etc
  _send(LCD_FUNCTIONSET | functionFlags);

  display();       // Display on
  clear();         // Display clea
  // Initialize to default text direction (for romance languages)
  _send(LCD_ENTRYMODESET | _entrymode)

} // begin()


void I2CLCD::clear() {
  _send(LCD_CLEARDISPLAY);
  delayMicroseconds(1600); // this command takes 1.5ms!
} // clear()

/*
void I2CLCD::init()
{
  clear();
} // init()
*/

void I2CLCD::home() {
  // Instruction: Return home = 0x02
  _send(LCD_RETURNHOME);
  delayMicroseconds(1600); // this command takes 1.5ms!
} // home()


/// Set the cursor to a new position.
void I2CLCD::setCursor(uint8_t col, uint8_t row) {
  //int row_offsets[] = {0x00, 0x40, 0x14, 0x54}
  const size_t max_lines = sizeof(_row_offsets) / sizeof(*_row_offsets)
  if ( row >= max_lines ) 
    row = max_lines - 1;    // we count rows starting w/
  
  if ( row >= _lines ) 
    row = _lines - 1;    // we count rows starting w/
  

  _send(LCD_SETDDRAMADDR | (_row_offsets[row] + col));
} // setCursor()


// Turn the display on/off (quickly)
void I2CLCD::noDisplay() {
  // Instruction: Display on/off control = 0x08
  _displaycontrol &= ~ LCD_DISPLAYON; // display
  _send(LCD_DISPLAYCONTROL | _displaycontrol);
} // noDisplay()


void I2CLCD::display() {
  _displaycontrol |= LCD_DISPLAYON; // display
  _send(LCD_DISPLAYCONTROL | _displaycontrol);
} // display()


// Turns the underline cursor on/off
void I2CLCD::cursor() {
  _displaycontrol |= LCD_CURSORON; // cursor
  _send(LCD_DISPLAYCONTROL | _displaycontrol);
} // cursor()


void I2CLCD::noCursor() {
  // Instruction: Display on/off control = 0x08
  _displaycontrol &= ~LCD_CURSORON; // cursor
  _send(LCD_DISPLAYCONTROL | _displaycontrol);
} // noCursor()


// Turn on and off the blinking cursor
void I2CLCD::blink() {
  _displaycontrol |= LCD_BLINKON; // blink
  _send(LCD_DISPLAYCONTROL | _displaycontrol);
} // blink()


void I2CLCD::noBlink() {
  _displaycontrol &= ~LCD_BLINKON; // blink
  _send(LCD_DISPLAYCONTROL | _displaycontrol);
} // noBlink()


// These commands scroll the display without changing the RAM
void I2CLCD::scrollDisplayLeft(void) {
  _send(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT);
} // scrollDisplayLeft()


void I2CLCD::scrollDisplayRight(void) {
  _send(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT);
} // scrollDisplayRight()


// == controlling the entrymode

// This is for text that flows Left to Right
void I2CLCD::leftToRight(void) {
  _entrymode |= LCD_ENTRYLEFT;
  _send(LCD_ENTRYMODESET | _entrymode);
} // leftToRight()

// This is for text that flows Right to Left
void I2CLCD::rightToLeft(void) {
  _entrymode &= ~LCD_ENTRYLEFT;
  _send(LCD_ENTRYMODESET | _entrymode);
} // rightToLeft()


// This will 'right justify' text from the cursor
void I2CLCD::autoscroll(void) {
  _entrymode |= LCD_ENTRYSHIFTINCREMENT;
  _send(LCD_ENTRYMODESET | _entrymode);
} // autoscroll()


// This will 'left justify' text from the cursor
void I2CLCD::noAutoscroll(void)
{
  // Instruction: Entry mode set, clear shift S=0x01
  _entrymode &= ~LCD_ENTRYSHIFTINCREMENT;
  _send(LCD_ENTRYMODESET | _entrymode);
} // noAutoscroll()


/// Setting the brightness of the background display light.
/// The backlight can be switched on and off.
/// The current brightness is stored in the private _backlight variable to have it available for further data transfers.
void I2CLCD::setBacklight(uint8_t brightness) {
  _backlight = brightness;
  // send no data but set the background-pin right;
  _write2Wire(0x00, true, false);
} // setBacklight()


// Allows us to fill the first 8 CGRAM locations
// with custom characters
void I2CLCD::createChar(uint8_t location, uint8_t charmap[]) {
  location &= 0x7; // we only have 8 locations 0-7
  // Set CGRAM address
  _send(LCD_SETCGRAMADDR | (location << 3));
  for (int i = 0; i < 8; i++) {
    write(charmap[i]);
  }
} // createChar()


/* The write function is needed for derivation from the Print class. */
inline size_t I2CLCD::write(uint8_t ch)
{
  _send(ch, true);
  return 1; // assume sucess
} // write()


// write either command or data
void I2CLCD::_send(int value, bool isData)
{
  // write high 4 bits
  _sendNibble((value >> 4 & 0x0F), isData);
  // write low 4 bits
  _sendNibble((value & 0x0F), isData);
} // _send()


// write a nibble / halfByte with handshake
void I2CLCD::_sendNibble(int halfByte, bool isData)
{
  _write2Wire(halfByte, isData, true);
  delayMicroseconds(1); // enable pulse must be >450ns
  _write2Wire(halfByte, isData, false);
  delayMicroseconds(37); // commands need > 37us to settle
} // _sendNibble


// private function to change the PCF8674 pins to the given value
// Note:
// you may change this function what the display is attached to the PCF8574 in a different wiring.
void I2CLCD::_write2Wire(int halfByte, bool isData, bool enable)
{
  // map the given values to the hardware of the I2C schema
  int i2cData = halfByte << 4;
  if (isData)
    i2cData |= PCF_RS;
  // PCF_RW is never used.
  if (enable)
    i2cData |= PCF_EN;
  if (_backlight > 0)
    i2cData |= PCF_BACKLIGHT;

  Wire.beginTransmission(_i2cAddr);
  Wire.write(i2cData);
  Wire.endTransmission();
} // write2Wire

// The End.
