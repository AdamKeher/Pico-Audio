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

#ifndef PICOAUDIO_H
#define PICOAUDIO_H

#include <Arduino.h>
#include "GPIOExpander.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2s.h"
#include "esp_system.h"
#include "Audio.h"
#include "LinkedList.h"
#include "SD.h"
#include "FS.h"

#define SAMPLE_RATE (44100)

#define PE_ADDR 0x20 // I2C Address
#define PE_SDA  32
#define PE_SCL  33

#define PA_SDA  32
#define PA_SCL  33

#define PA_SPI_MOSI 23
#define PA_SPI_MISO 19
#define PA_SPI_SCK  18

#define PA_DAC_BCLK 26
#define PA_DAC_LRC  25
#define PA_DAC_DIN  I2S_PIN_NO_CHANGE
#define PA_DAC_DOUT 22
#define PA_MIC_BCLK 14
#define PA_MIC_LRC  4
#define PA_MIC_DIN  15
#define PA_MIC_DOUT I2S_PIN_NO_CHANGE

class PicoAudio
{
    public:
        PicoAudio(bool initAudio = true);
        void begin();
        void process();
        void process(bool processAudio, bool processGPIO);

        GPIOExpander *getIoExpander() { return ioExpander; };
        Audio *getAudio() { return audio; }

        bool connecttoHost(String sdfile);

        bool connecttoSD(String sdfile);
        bool pauseResume();
        bool isPlaying();
        void setVolume(uint8_t vol); // vol 22 steps, 0...21
        int getVolume() { return volumeStep; }
        int getVolumeRaw() { return audio->getVolume(); }
        int volumeUp() {
            if (volumeStep < 9)
                setVolume(volumeSteps[++volumeStep]);
            return volumeStep;
        }
        int volumeDown() {
            if (volumeStep != 0)
                setVolume(volumeSteps[--volumeStep]);
            return volumeStep;
        }
        void setVolumeMax() {
            setVolume(21);
            volumeStep = 9;
        }
        void mute() { 
            setVolume(0);
            volumeStep = 0;
        }

        bool scanSD();
        void populate(fs::FS &fs, const char *dirname);
        LinkedList<char *>* FileList() { return fileList; }
        int playFile();
        int playNext();
        int playPrev();
        int getFileListPosition() { return filePos; }
        int getFileListSize() { return fileList->size(); }

        bool RegisterButtonCB(GPIOEvents::chngCBFn fn, void *data);
        uint8_t readIO(uint8_t pin);
        void writeIO(const uint8_t pin, const uint8_t value);

        void I2S_Init(i2s_mode_t MODE, i2s_bits_per_sample_t BPS, i2s_port_t PORT, i2s_channel_fmt_t FORMAT, int BCLK, int LRC, int DIN, int DOUT);
        void initDAC();
        void initMIC();
        int readMIC(char* data, int numData);
        void writeDAC(char* data, int numData);

    private:
        GPIOExpander *ioExpander;
        
        Audio *audio;
        uint8_t volumeStep = 0;
        uint8_t volumeSteps[10] {0, 1, 3, 5, 7, 10, 13, 16, 19, 21};

        LinkedList<char *> *fileList;
        int filePos = 0;
        uint32_t mp3FilePos = -0;
};

#endif // PICOAUDIO_H