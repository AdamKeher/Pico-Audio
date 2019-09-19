#include "graphics.h"
#include <WiFi.h>

Graphics::Graphics() {
    tft.init();
    tft.setRotation(1);

    buffer.setColorDepth(8);
    buffer.createSprite(tft.width(), tft.height());
    buffer.setRotation(1);
    buffer.setTextFont(2);
    buffer.setTextSize(2);
    buffer.setTextColor(TFT_RED);
    buffer.fillScreen(TFT_BLUE);    
}

void Graphics::drawMetadata(StreamMetadata metadata, uint32_t color) {
  // NOTE: append a space to streamTitle as tft.drawString clips last letter when it partially draws out of right bounds
  char title[65];
  strcpy(title, metadata.streamTitle);
  strcat(title, " ");
  int16_t correction = buffer.textWidth(title) - buffer.width();
  // Serial.printf("correction: %d | title: '%s' | titlewidth: %d | bufwidth: %d\r\n", correction, title, buffer.textWidth(title), buffer.width() );
  if (correction < 0) {
    pos = 0;
  } else {

    if (forward) {
      pos = pos + 1;
      if (pos > 0)
        forward = false;
    } 
    else {
      pos = pos - 1;
      if (pos < 0 - correction)
        forward = true;    
    }
  }

  buffer.setTextColor(color);
  buffer.drawString(title, pos + 5, 40);  
  buffer.drawString(metadata.siteName, 5, 70);
  buffer.drawString(metadata.genre, 5, 100);
  buffer.drawString(metadata.bitRate, 5, 130);
}

void Graphics::drawVu(int x, int width, int16_t vuLevel) {
  // vuLevel -32k..0..32k
  uint8_t vu = (vuLevel & 0x7FFF) >> 8;

  int vuBorder = 25;
  int vuBarHeight = 20;

  for (int i = 0; i < 10; i++) {
    uint32_t color = TFT_DARKGREY;

    if (vu * 2 > i * 25)
    switch (i)
    {
    case 0:
    case 1:
      color = TFT_GREEN;
      break;
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
      color = TFT_YELLOW;
      break;
    case 8:
      color = TFT_ORANGE;
      break;
    case 9:
      color = TFT_RED;
      break;
    default:
      color = TFT_DARKGREY;
      break;
    }

    buffer.fillRect(x, buffer.height() - (i * vuBarHeight) - vuBarHeight - vuBorder, width, vuBarHeight-1, color);
  }
}

void Graphics::drawLogo() {
    // TODO: Clean up calcs & convert to sprite for transparency
    uint16_t x = (sin(counter) * ((320 - LOGOWIDTH) / 2)) + (160 - (LOGOWIDTH/2));
    uint16_t y = (sin(counter/2) * ((240 - LOGOHEIGHT) / 2)) + (120 - (LOGOHEIGHT/2));
    buffer.pushImage(x, y, LOGOWIDTH, LOGOHEIGHT, logo);
    counter = counter + 0.1f;
}

void Graphics::drawCheckerBoard(uint16_t vu) {

    long color = tft.color565(0, 0, vu);
    for (int y = 0; y < 6; y++) {
      for (int x = 0; x < 9; x++)
      {
        if (box % 2 == 0)
          buffer.fillRect((x * 40) + box_offset, y * 40, 40, 40, color);
        else
          buffer.fillRect((x * 40) + box_offset, y * 40, 40, 40, TFT_WHITE);
        box++;
      }
    }
    box_offset = box_offset + 2;

    if (box_offset == 0) {
      box_offset = -40;
      box++;
    }
}

void Graphics::drawFFT(uint16_t sampleRate, double *buckets) {
    int barWidth = buffer.width() / (sampleRate / 2);
    int tftHeight = buffer.height();

    for (int i = 0; i < sampleRate / 2; i++)
    {
      int height = buckets[i] / 1024;
      buffer.fillRect(i * barWidth, tftHeight - height, barWidth, height, TFT_GREEN);
    }
}

void Graphics::drawDebug(int16_t bufferSize, uint16_t fillLevel) {
  // draw buffer fillrate
    int bfrWidth = buffer.width() - 60;
    int bfrBorder = 5;
    int bfrY = 160;
    int bfrHeight = 30;
    double bsize = bufferSize;
    buffer.drawRect(bfrBorder, bfrY, bfrWidth, bfrHeight, TFT_RED);
    buffer.fillRect(bfrBorder + 1, bfrY + 1, fillLevel == 0 ? 0 : bfrWidth * (fillLevel / bsize) - 2, bfrHeight - 2, TFT_ORANGE);
    buffer.drawString("BUFFER", 10, 160);

    // draw wireless strength
    bfrWidth = buffer.width() - 60;
    double rssi = abs(WiFi.RSSI());
    double fill = 0.0f;
    if (rssi <= 35)
      fill = 100.0f;
    else if (rssi > 35 && rssi <= 70)
      fill = (70-(rssi-35))/70;
    else if (rssi > 70)
      fill = 0.0f;
    buffer.drawRect(bfrBorder, bfrY + 35, bfrWidth, bfrHeight, TFT_RED);
    buffer.fillRect(bfrBorder + 1, bfrY + 35 + 1, (bfrWidth * fill) - 2, bfrHeight - 2, TFT_ORANGE);
    buffer.drawString(String(WiFi.RSSI()) + "db", 10, 195);
}

void Graphics::drawFrame() {
    buffer.pushSprite(0, 0);

    frameCount++;
    if (millis() - lastFPS > 1000) {
        lastFPS = millis();
        FPS = frameCount;
        frameCount = 0;
    }
}

// void drawAlpha(uint16_t _x, uint16_t _y, uint16_t _w, uint16_t _h, const uint16_t *data, TFT_eSprite *backBuffer) {
//   // uint16_t tx = _x;
//   // _x = _y;
//   // _y = _dheight - tx - 1;

//   for (int h = 0; h < _h; h++) {
//     for (int w = 0; w < _w; w++) {
//       backBuffer->drawPixel(_x + h, _y + w, logo[(h * w)+ w]);
//     }
//   }
// }