/*
Copyright 2019 AKsevenFour / Adam Keher (adam@keher.com.au)
http://www.aksevenfour.com / https://github.com/AdamKeher

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation
files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy,
modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software
is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR
IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include "lcd.h"
#include "Arduino.h"

LCD::LCD(int address, int columns, int rows, int sda, int scl) {
    this->address = address;
    this->columns = columns;
    this-> rows = rows;
    this->sda = sda;
    this->scl = scl;

    lcd = new LiquidCrystal_I2C(address, columns, rows);
    // lcd->init(sda, scl);
    Wire.begin(sda, scl);
    lcd->init();
    lcd->begin(columns, rows);
    lcd->backlight();

    row1Text = (char *)malloc(128);
    row0Text = (char *)malloc(128);

    memset(row1Text, 0, 128);
    memset(row0Text, 0, 128);
};

void LCD::print(int col, int row, bool clear, char *text) {
    char displayText[64];

    lcd->setCursor(col, row);

    if (clear)
      sprintf(displayText, "%-16s", text);
    else
      strcpy(displayText, text);

    if (row == 0) {
        strcpy(row0Text, text);
        row0Pos = 0;
        row0Forward = true;
        lcd->print(displayText);
    }
    else
    if (row == 1) {
        strcpy(row1Text, text);
        row1Pos = 0;
        row1Forward = true;
        lcd->print(displayText);
    }
};

void LCD::setScrollDelay(int delay) {
    row1ScrollDelay = delay;
}

void LCD::loop() {
    char buffer[17];

    long now = millis();
    
    if ((now - row0lastUpdate > row0ScrollDelay) && (strlen(row0Text) > 16))
    {
      if (row0Forward)
        row0Pos++;
      else
        row0Pos--;

      if (row0Pos > strlen(row0Text) - columns)
      {
        row0Forward = false;
        row0Pos--;
      }

      if (row0Pos == 0)
      {
        row0Forward = true;
      }
      memcpy(buffer, row0Text + row0Pos, 16);
      lcd->setCursor(0, 0);
      lcd->print(buffer);

      row0lastUpdate = now;
    }

    if ((now - row1lastUpdate > row1ScrollDelay) && (strlen(row1Text) > 16))
    {
      if (row1Forward)
        row1Pos++;
      else
        row1Pos--;

      if (row1Pos > strlen(row1Text) - columns)
      {
        row1Forward = false;
        row1Pos--;
      }

      if (row1Pos == 0)
      {
        row1Forward = true;
      }
      memcpy(buffer, row1Text + row1Pos, 16);
      lcd->setCursor(0, 1);
      lcd->print(buffer);

      row1lastUpdate = now;
    }


};