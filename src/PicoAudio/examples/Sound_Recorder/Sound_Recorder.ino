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
#include <FS.h>
#include <SD.h>
#include <TinyPICO.h>
#include <PicoAudio.h>
#include "Wav.h"

const int record_time = 10;  // seconds
const char filename[] = "/sound.wav";

const int headerSize = 44;                    // static size of a wave file header
const int waveDataSize = record_time * 88200; // mono wav file = 44,100 x 16 bits = 88200 bytes
const int micBufferSize = 8000;               // nominal buffer size for i2s mic recording left/right channels in 32 bit
                                              // as mic is mono one channel is discarded. Buffer size is ~22.7 ms)
                                              // 4 bytes (32bit sample) x 44100 sample rate x 2 chanels = 352800 bytes / seconds
                                              // 352800 bytes per secon  / 8000 byte buffer = 22.67573696 ms
const int wavBufferSize = micBufferSize/4;    // wav buffer size is 1/4 of mic as wav is mono (half) and 16 bit as apposed to the mic's 32 bit (half)

// file size on SD card will be: 88,200 bytes per second * 10 seconds + 44 byte wav header = 861k / 882,044 bytes

byte header[headerSize];
char micData[micBufferSize];
char wavData[wavBufferSize];

File file;

TinyPICO tinyPICO = TinyPICO();
PicoAudio picoAudio = PicoAudio();

void setup() {  
  Serial.begin(115200);
  picoAudio.begin();

  if (!SD.begin())
  {
    Serial.println("SD begin failed");
    return;
  }

  // create wav file header, remove existing file and write header to file on SD card
  CreateWavHeader(header, waveDataSize);
  SD.remove(filename);
  file = SD.open(filename, FILE_WRITE);
  if (!file) return;
  file.write(header, headerSize);

  // initialise i2s microphone
  picoAudio.initMIC();

  // set tinyPICO LED to green for 5 seconds before turning red and commencing recording
  Serial.println("get ready for recording in 5 seconds");
  tinyPICO.DotStar_SetBrightness(255);
  tinyPICO.DotStar_SetPixelColor(0, 255, 0);
  delay(5000);
  tinyPICO.DotStar_SetPixelColor(255, 0, 0);
  Serial.println("start recording"); 

  // record audio and write to file
  for (int j = 0; j < waveDataSize/wavBufferSize; ++j) {
    picoAudio.readMIC(micData, micBufferSize);

    // convert 2 channel 32 bit to mono 16 bit
    for (int i = 0; i < micBufferSize / 8; ++i) {
      // wavdata counts 2 bytes per iteration (16bit (2 bytes) x 1 mono channel ) 
      // micData counts 8 bytes per iteration (32bit (4 bytes) / 2 channel = 8 bytes)
      wavData[2 * i] = micData[8 * i + 2];
      wavData[2 * i + 1] = micData[8 * i + 3];
    }

    // write converted data to wav file
    file.write((const byte*)wavData, wavBufferSize);
  }

  file.close();
  Serial.println("finish");
}

void loop() {
  // flash LED once recording finished
  tinyPICO.DotStar_SetPixelColor(0, 0, 255);
  delay(1000);
  tinyPICO.DotStar_SetPixelColor(0, 0, 0);
  delay(1000);  
}
