#include <I2CLCD.h>
#include <Wire.h>

uint8_t addr = 0x27;

I2CLCD lcd(addr); // set the LCD address to 0x27 for a 16 chars and 2 line display

int show = -1;

void setup()
{
  int error;

  Serial.begin(115200);
  Serial.println("LCD...");

  // wait on Serial to be available on Leonardo
  while (!Serial)
    ;

  Serial.print("LCD ");

  // See http://playground.arduino.cc/Main/I2cScanner how to test for a I2C device.
  Wire.begin();
  Wire.beginTransmission(addr);
  error = Wire.endTransmission();

  if (error == 0) {
    Serial.println("found.");
    show = 0;
    lcd.begin(16, 2); // initialize the lcd

  } else {
    Serial.println("not found.");
    Serial.print("Error: ");
    Serial.println(error);
    while(1);
  } // if

} // setup()


void loop()
{
  Serial.print(show);
  Serial.print(".");
  if (show == 0) {
    lcd.setBacklight(255);
    lcd.home();
    lcd.clear();
    lcd.print("Hello LCD");
    delay(1000);

    lcd.setBacklight(0);
    delay(400);
    lcd.setBacklight(255);

  } else if (show == 1) {
    lcd.clear();
    lcd.print("Cursor On");
    lcd.cursor();

  } else if (show == 2) {
    lcd.clear();
    lcd.print("Cursor Blink");
    lcd.blink();

  } else if (show == 3) {
    lcd.clear();
    lcd.print("Cursor OFF");
    lcd.noBlink();
    lcd.noCursor();

  } else if (show == 4) {
    lcd.clear();
    lcd.print("Display Off");
    lcd.noDisplay();

  } else if (show == 5) {
    lcd.clear();
    lcd.print("Display On");
    lcd.display();

  } else if (show == 7) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("*** first line.");
    lcd.setCursor(0, 1);
    lcd.print("*** second line.");

  } else if (show == 8) {
    lcd.scrollDisplayLeft();
  } else if (show == 9) {
    lcd.scrollDisplayLeft();
  } else if (show == 10) {
    lcd.scrollDisplayLeft();
  } else if (show == 11) {
    lcd.scrollDisplayRight();

  } else if (show == 12) {
    lcd.clear();
    lcd.print("write-");

  } else if (show > 12) {
    lcd.print(show - 13);
  } else if (show == 16) {
    Serial.println();
  }// if

  delay(1000);
  show = (show + 1) % 16;
} // loop()
