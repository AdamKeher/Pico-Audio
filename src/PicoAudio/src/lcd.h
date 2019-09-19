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
#include <LiquidCrystal_I2C.h>

#ifndef LCD_H
#define LCD_H

class LCD
{
    public:
        LCD(int address, int columns, int rows, int sda, int scl);

        void print(int col, int row, bool clear, char *text);
        void loop();
        void setScrollDelay(int delay);

        LiquidCrystal_I2C* getLCD() { return lcd; }
        
    private:
        LiquidCrystal_I2C *lcd;
        int address;
        int columns;
        int rows;
        int sda;
        int scl;

        // TODO: convert to struct and use list for #rows
        // TODO: use columns for row clear etc

        char *row0Text;
        int row0Pos = 0;
        bool row0Forward = true;
        int row0ScrollDelay = 500;
        long row0lastUpdate = 0;
        
        char *row1Text;
        int row1Pos = 0;
        bool row1Forward = true;
        int row1ScrollDelay = 500;
        long row1lastUpdate = 0;
};

#endif
