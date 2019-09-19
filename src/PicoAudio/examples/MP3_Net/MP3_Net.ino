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

#include <Arduino.h>
#include <WiFi.h>
#include <Wire.h>
#include "TinyPICO.h"
#include "PicoAudio.h"
#include "messaging.h"
#include "lcd.h"
#include "metadata.h"
#include "graphics.h"
#include <TFT_eSPI.h>

#define LCD_ADDR  0x27 // I2C Address
#define LCD_COL  16
#define LCD_ROW  2

// Enter your WiFi setup here:
const char *SSID = "GoAway";
const char *PASSWORD = "w00tw00tw00t";

int urlPos = 0;
const char *URLS[] = {
  "http://live-radio01.mediahubaustralia.com/2TJW/mp3/", 
  "http://dg-ais-eco-http-fra-eco-cdn.cast.addradio.de/hellwegradio/west/mp3/low",
  "http://jazz.streamr.ru/jazz-64.mp3" };

TinyPICO tinyPICO = TinyPICO();
PicoAudio picoAudio = PicoAudio();

LCD *lcd;

TaskHandle_t messageProcessorTask;
Messaging messaging = Messaging();
struct message *newMessage = NULL;

StreamMetadata streamMetadata;
double buckets[256];
double peak;
int16_t rVu;
int16_t lVu;

void GPIOChangeCallback(void *cbData, uint8_t ports, uint8_t button, bool state) {
  // Note: XOR ports to flip the bits for menu button (active low)
  Serial.printf("BUTTONS: Function %02d         | %s\r\n", button, state ? "DOWN" : "UP" );
  char buffer[64];

  switch (button) {    
    case 3:
      if (state) {
        Serial.printf("BUTTONS: Pause                | %s\r\n", picoAudio.isPlaying() ? "Resume" : "Pause");
        picoAudio.pauseResume();
        sprintf(buffer, "%s", picoAudio.isPlaying() ? "Resume" : "Pause");
        messaging.display(0, 2500, true, buffer);
        messaging.sendMessage(messaging.messageQueue, updateMetadata, &streamMetadata);
      }
      break;
    case 4:
      if (state) {
        Serial.printf("BUTTONS: Prev Track          |\r\n");
        messaging.sendMessage(messaging.mainQueue, prevTrack, NULL);
        messaging.display(0, 2500, true, "<< Prev Track");
      }
      break;
    case 5:
      if (state) {
        Serial.printf("BUTTONS: Next Track          |\r\n");
        messaging.sendMessage(messaging.mainQueue, nextTrack);
        messaging.display(0, 2500, true, "Next Track >>");
      }
      break;
    case 6:
      if (state) {
        picoAudio.volumeDown();
        Serial.printf("BUTTONS: Volume Down           | volume %d/10\r\n", picoAudio.getVolume() + 1);
        sprintf(buffer, "Volume: %d/10", picoAudio.getVolume() + 1);
        messaging.display(0, 2500, true, buffer);
        messaging.sendMessage(messaging.messageQueue, updateMetadata, &streamMetadata);
      }
      break;
    case 7:
      if (state) { 
        picoAudio.volumeUp();
        Serial.printf("BUTTONS: Volume Up             | volume %d/10\r\n", picoAudio.getVolume() + 1);
        sprintf(buffer, "Volume: %d/10", picoAudio.getVolume() + 1);
        messaging.display(0, 2500, true, buffer);
        messaging.sendMessage(messaging.messageQueue, updateMetadata, &streamMetadata);
      }
      break;
    default: break;
  }
}

void FFTCallback(void *cbData, int16_t _lVu, int16_t _rVu, double _peak, double *values) {
  // for (int i= 0; i < out->getSampleRate() / 2; i++) {
  //   buckets[i] = values[i];
  // }
  // peak = _peak;
  // Serial.printf("DEBUG::VU::%d:%d\r\n", _lVu >> 8, _rVu >> 8);
  lVu = _lVu;
  rVu = _rVu;
}

void MetadataCallback(void *cbData, StreamMetadata *metadata) {
  lcd->print(0, 0, true, metadata->siteName);
  lcd->print(0, 1, true, metadata->streamTitle);
  Serial.printf("DEBUG::MSGMDCB: metadata: '%s'\r\n", metadata->streamTitle);
}

void DisplayCallback(void *cbData, int line, char *text) {
  Serial.printf("DEBUG::DISPLAYCALLBACK - '%s'\r\n", text);
  lcd->print(0, line, true, text);
}

void setupWiFi() {
  Serial.printf("MAC Address: %s\r\n", WiFi.macAddress().c_str());
  messaging.display(0, 0, false, "WiFi: ");
  messaging.display(1, 0, false, "");
  WiFi.disconnect();
  WiFi.softAPdisconnect(true);
  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID, PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
  }
  long rssi = WiFi.RSSI();
  messaging.display(0, 0, false, "WiFi: Connected");
  char buffer[64];
  sprintf(buffer, "RSSI: %d dB", rssi);
  messaging.display(1, 2000, false, buffer);
  Serial.printf("MAC Address: %s\r\n", WiFi.macAddress().c_str());
}

void setup() {
  Serial.begin(115200);

  picoAudio.begin();
  picoAudio.mute();

  messaging.RegisterMetadataCB(MetadataCallback, (void*)"metadata");
  messaging.RegisterDisplayCB(DisplayCallback, (void *)"display");


  // tinyPICO led yellow
  tinyPICO.DotStar_SetBrightness(255);
  tinyPICO.DotStar_SetPixelColor(0, 255, 255);

  // create task to process messages, LCD and TFT
  xTaskCreatePinnedToCore(
      messageProcessor,       /* Task function. */
      "messageProcessorTask", /* name of task. */
      10000,                  /* Stack size of task */
      NULL,                   /* parameter of the task */
      1,                      /* priority of the task */
      &messageProcessorTask,  /* Task handle to keep track of created task */
      0);                     /* pin task to core 0 */
  
  // connect to wifi and display RSSI
  setupWiFi();

  // tinyPICO led green
  tinyPICO.DotStar_SetPixelColor(0, 255, 0);
  tinyPICO.DotStar_Show();

  // connect to stream
  picoAudio.connecttoHost(URLS[0]);
  // picoAudio.RegisterFFTCB(FFTCallback, NULL);

  messaging.display(0, 0, false, "Pico Audio");
  
  picoAudio.setVolumeMax();
}

void loop() {
  picoAudio.process(true, false);

  if (uxQueueMessagesWaiting(messaging.mainQueue) > 0)
  {
    xQueueReceive(messaging.mainQueue, &newMessage, 0);
    Serial.printf("MESSAGE: Received (Core %d)   | ID %04d | function %d\r\n", xPortGetCoreID(), newMessage->messageId, newMessage->function);
    if (newMessage->function == nextTrack) {
      // clear metadata
      strcpy(streamMetadata.streamTitle, "");
      strcpy(streamMetadata.siteName, "");
      strcpy(streamMetadata.genre, "");
      strcpy(streamMetadata.bitRate, "");

      urlPos++;
      if (urlPos >= sizeof(URLS) / sizeof(URLS[0]))
        urlPos = 0;
      picoAudio.connecttoHost(URLS[urlPos]);
      Serial.printf("BUTTONS: Next URL            | urlPos (%d) | Url %s\r\n", urlPos, URLS[urlPos]);
    } else
    if (newMessage->function == prevTrack) {
      // clear metadata
      strcpy(streamMetadata.streamTitle, "");
      strcpy(streamMetadata.siteName, "");
      strcpy(streamMetadata.genre, "");
      strcpy(streamMetadata.bitRate, "");

      urlPos--;
      if (urlPos < 0)
        urlPos = (sizeof(URLS)/sizeof(URLS[0]))-1;
      picoAudio.connecttoHost(URLS[urlPos]);
      Serial.printf("BUTTONS: Prev URL            | urlPos (%d) | Url %s\r\n", urlPos, URLS[urlPos]);
    }

    vPortFree(newMessage);
  }
}

void messageProcessor(void * pvParameters) {
  // GPIO Extender / Buttons
  picoAudio.RegisterButtonCB(GPIOChangeCallback, (void*)"messageProcessor");

  // 16x2 LCD Screen
  lcd = new LCD(LCD_ADDR, LCD_COL, LCD_ROW, PA_SDA, PA_SCL);

  // 320x240 ILI9341 Screen
  Graphics graphics = Graphics();

  for (;;)
  {
    // draw checkerboard
    graphics.drawCheckerBoard(lVu);

    // draw logo
    graphics.drawLogo();

    // draw VU meter
    //TODO: Clean up / simplify variables passed in
    int vuBarWidth = 20;
    int vuBorder = 5;
    graphics.drawVu(graphics.getBuffer().width() - vuBarWidth - vuBorder, vuBarWidth, lVu);
    graphics.drawVu(graphics.getBuffer().width() - (vuBarWidth * 2) - (vuBorder * 2), vuBarWidth, rVu);

    // draw FFT equaliser
    // graphics.drawFFT(out->getSampleRate(), buckets);

    // draw Metadata
    graphics.drawMetadata(streamMetadata, TFT_RED);

    // draw FPS
    graphics.getBuffer().drawString("FPS: " + String(graphics.getFPS()), 5, 0);
    
    // send screen buffer to TFT
    graphics.drawFrame();

    messaging.loop();

    // scroll long metadata
    lcd->loop();

    // Process GPIO extender buttons
    picoAudio.process(false, true);

    // vTaskDelay(1000);
  }
}

// Audio metadata

void audio_showstation(const char *info){
    Serial.printf("AUDMETA: Station             | \033[32m%s\033[37m\r\n", info);
    strcpy(streamMetadata.siteName, info);
    messaging.sendMessage(messaging.messageQueue, updateMetadata, &streamMetadata);
}
void audio_showstreaminfo(const char *info){
    Serial.printf("AUDMETA: Streaminfo          | \033[32m%s\033[37m\r\n", info);
}
void audio_showstreamtitle(const char *info){
    Serial.printf("AUDMETA: Streamtitle         | \033[32m%s\033[37m\r\n", info);
    strcpy(streamMetadata.streamTitle, info);
    messaging.sendMessage(messaging.messageQueue, updateMetadata, &streamMetadata);
}