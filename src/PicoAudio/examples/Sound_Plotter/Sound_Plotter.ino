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

#include <PicoAudio.h>

PicoAudio picoAudio = PicoAudio();

void setup() {
  Serial.begin(115200);
  Serial.println("Configuring I2S...");

  picoAudio.begin();
  picoAudio.initMIC();

  Serial.println("I2S driver installed.");
}

int count;

void loop() {
  int32_t sample = 0;
  int bytes_read = picoAudio.readMIC((char *)&sample, 4);
  if (count % 2 == 0 && bytes_read == 4) {
    Serial.println(sample);
  }
  count++;
}