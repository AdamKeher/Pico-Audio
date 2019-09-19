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

#include "PicoAudio.h"
#include "TinyPICO.h"

const int bufferSize = 8000;
char audioData[bufferSize];
int* audioDataPtr = (int*)&audioData[0];
bool applyVocoder = false;
PicoAudio picoAudio = PicoAudio(false);
TinyPICO tinyPICO = TinyPICO();

void GPIOChangeCallback(void *cbData, uint8_t ports, uint8_t button, bool state) {
  switch (button) {
    case 3: {
      applyVocoder = state ? true : false;
      tinyPICO.DotStar_SetPixelColor(applyVocoder ? 0 : 255, applyVocoder ? 255 : 0, 0);
      break;
    }
    default:
      break;
  }
}

void setup() {
  Serial.begin(115200);
  Serial.flush();

  picoAudio.begin();
  picoAudio.initDAC();
  picoAudio.initMIC();
  picoAudio.RegisterButtonCB(GPIOChangeCallback, (void*)"messageProcessor");

  Serial.println("Hold push button to apply vocoder.");
  tinyPICO.DotStar_SetBrightness(255);
  tinyPICO.DotStar_SetPixelColor(255, 0, 0);
}

float counter;

void loop() {
  counter = 0.0f;

  picoAudio.readMIC(audioData, bufferSize);

  if (applyVocoder)
  {
    for (int i = 0; i < bufferSize; i++)
    {
      audioData[i] = (audioData[i]) + (audioData[i] * abs(sin(counter)));
      counter += 0.01f;
    }
  }

  for (int i = 0; i < bufferSize / 4; i+=2) {
    audioDataPtr[i] = audioDataPtr[i] * 1.5f; // gain
    audioDataPtr[i + 1] = audioDataPtr[i];    // duplicate mono recording for stereo output
  }

  picoAudio.writeDAC(audioData, bufferSize);
  picoAudio.process(false, true);
}
