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
#ifndef GRAPHICS_H
#define GRAPHICS_H

// TFT
// #define TFT_DC   27
// #define TFT_CS   21
// #define TFT_MOSI 23
// #define TFT_CLK  18
// #define TFT_RST  -1 // tie RST to 3.3v
// #define TFT_MISO 19

#include <TFT_eSPI.h>
#include "metadata.h"
#include "ak74_logo.h"

class Graphics
{
    public:
        Graphics();

        void drawMetadata(StreamMetadata metadata, uint32_t color);
        void drawVu(int x, int width, int16_t vuLevel);
        void drawLogo();
        void drawCheckerBoard(uint16_t vu);
        void drawFFT(uint16_t sampleRate, double *buckets);
        void drawDebug(int16_t bufferSize, uint16_t fillLevel);

        void drawFrame();

        TFT_eSPI getTFT() { return tft; }
        TFT_eSprite getBuffer() { return buffer; }
        int getFPS() { return FPS; }

    private:
        TFT_eSPI tft = TFT_eSPI();
        TFT_eSprite buffer = TFT_eSprite(&tft);
        uint16_t box;
        int box_offset = -40;
        double counter = 0;
        int frameCount = 0;
        int FPS = 0;
        uint32_t lastFPS = 0;
        int32_t pos = 0;
        bool forward = false;
};

#endif
