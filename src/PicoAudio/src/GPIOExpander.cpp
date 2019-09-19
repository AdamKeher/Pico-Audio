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

#include "GPIOExpander.h"

GPIOExpander::GPIOExpander(uint8_t address, uint8_t sda, uint8_t scl) {
    _address = address;
    _sda = sda;
    _scl = scl;

    _error = GPIOEXPANDER_OK;
}

void GPIOExpander::begin(uint8_t val)
{
  Wire.begin(_sda, _scl);
  write8(val);
}

bool GPIOExpander::testInput(uint8_t inputs, uint8_t input) {
    return ((inputs >> input) ^ 0xFF) & 0x01;
}

uint8_t GPIOExpander::read8()
{
    if (Wire.requestFrom(_address, (uint8_t)1) != 1)
    {
        _error = GPIOEXPANDER_I2C_ERROR;
        return _dataIn; // previous value
    }
    _dataIn = Wire.read();
    return _dataIn;
}

uint8_t GPIOExpander::read(uint8_t pin) {
    if ((read8() & (1 << pin)) > 0)
        return HIGH;
    else
        return LOW;
}

void GPIOExpander::write8(const uint8_t value)
{
    _dataOut = value;
    Wire.beginTransmission(_address);
    Wire.write(_dataOut);
    _error = Wire.endTransmission();
}

void GPIOExpander::write(const uint8_t pin, const uint8_t value)
{
    if (value == LOW)
    {
        _dataOut &= ~(1 << pin);
    }
    else
    {
        _dataOut |= (1 << pin);
    }
    write8(_dataOut);
}

void GPIOExpander::process() {
    uint8_t value = read8();

    // Buttons start at P3 on GPIO Expander
    for (int i = 0; i < 8; i++) {
        // shift to button bit, flip it as it's a pull up to gnd and isolate the result
        bool curState = testInput(value, i);
        if (curState != buttonStates[i]) {
            cb.change(value, i, curState);
            buttonStates[i] = curState;
        }
    }
}