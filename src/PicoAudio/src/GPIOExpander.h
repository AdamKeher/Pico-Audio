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

#ifndef GPIOEXPANDER_H
#define GPIOEXPANDER_H

#include <Arduino.h>
#include <Wire.h>

#define BUTTON_UP   0x0
#define BUTTON_DOWN 0x1

#define P0          0x01
#define P1          0x02
#define P2          0x04
#define P3          0x08
#define P4          0x10
#define P5          0x20
#define P6          0x40
#define P7          0x80

#define GPIOEXPANDER_OK          0x00
#define GPIOEXPANDER_I2C_ERROR   0x82

class GPIOEvents
{
    public:
        GPIOEvents() { ClearCBs(); }
        void ClearCBs() { chngFn = NULL; };

        typedef void (*chngCBFn)(void *cbData, uint8_t ports, uint8_t button, bool state);
        bool RegisterChangeCB(chngCBFn f, void *cbData) { chngFn = f; chngData = cbData; return true; }
        inline void change(uint8_t ports, uint8_t button, bool state) { if (chngFn) chngFn(chngData, ports, button, state); }
    private:
        chngCBFn chngFn;
        void *chngData;
};

class GPIOExpander
{
    public:
        GPIOExpander(uint8_t address, uint8_t sda, uint8_t scl);
        
        void begin(uint8_t val=0xFF);

        static bool testInput(uint8_t inputs, uint8_t input);
        bool RegisterChangeCB(GPIOEvents::chngCBFn fn, void *data) { return cb.RegisterChangeCB(fn, data); }

        uint8_t read8();
        uint8_t read(uint8_t pin);
        void write8(const uint8_t value);
        void write(const uint8_t pin, const uint8_t value);

        void process();

        bool buttonStates[8] { false, false, false, false, false, false, false, false };

    private:
        GPIOEvents cb;
        uint8_t _address;
        uint8_t _sda;
        uint8_t _scl;
        uint8_t _dataIn;
        uint8_t _dataOut;
        int _error;
};

#endif // GPIOEXPANDER_H