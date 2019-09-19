#include "Arduino.h"
#include "WiFi.h"
#include "lcd.h"
#include "PicoAudio.h"

#define LCD_ADDR 0x27 // I2C Address
#define LCD_COL  16
#define LCD_ROW  2

PicoAudio *picoAudio;
LCD *lcd;

struct ID3Data {
  char Artist[128];
  char Album[128];
  char Title[128];
} id3data;

void playNext() {
  Serial.println("[Play Next]================================================================");
  strcpy(id3data.Album, "??");
  strcpy(id3data.Artist, "??");
  strcpy(id3data.Title, "??");
  picoAudio->playNext();
  updateLCD();
}

void playPrev() {
  Serial.println("[Play Prev]================================================================");
  strcpy(id3data.Album, "??");
  strcpy(id3data.Artist, "??");
  strcpy(id3data.Title, "??");
  picoAudio->playPrev();
  updateLCD();
}

void GPIOChangeCallback(void *cbData, uint8_t ports, uint8_t button, bool state) {
  switch (button) {
    case 3: {
      if (state == true) {
        picoAudio->pauseResume();
        Serial.printf("button      %s\r\n", picoAudio->isPlaying() ? "Play" : "Pause" );
      }
      break;
    }
    case 4: {
      if (state == true) {
        playPrev();
      }
      break;
    }
    case 5: {
      if (state == true) {
        playNext();
      }
      break;
    }
    case 6: {
      if (state == true) {
          Serial.printf("Volume: %d/10\r\n", picoAudio->volumeDown() + 1);
      }
      break;
    }
    case 7: {
      if (state == true) {
          Serial.printf("Volume: %d/10\r\n", picoAudio->volumeUp() + 1);
      }
      break;
    }
    default: break;
  }
}

void updateLCD() {
  char line1[256];
  strcpy(line1, id3data.Artist);
  strcat(line1, " - ");
  strcat(line1, id3data.Album);

  char line2[156];
  sprintf(line2, "%d/%d ", picoAudio->getFileListPosition() + 1, picoAudio->getFileListSize());
  strcat(line2, id3data.Title);
  lcd->print(0, 0, true, line1);
  lcd->print(0, 1, true, line2);
}

void setup() {
  Serial.begin(115200);

  picoAudio = new PicoAudio();
  picoAudio->begin();
  picoAudio->mute();
  picoAudio->RegisterButtonCB(GPIOChangeCallback, (void*)"messageProcessor");

  lcd = new LCD(LCD_ADDR, LCD_COL, LCD_ROW, PA_SDA, PA_SCL);
  lcd->print(0, 0, true, "picoAUDIO Player");
  lcd->print(0, 1, true, "2019 AKsevenFOUR");

  if (!picoAudio->scanSD()) {
    Serial.println("Error mounting SD Card");
    lcd->print(0, 0, true, "SD Card Error");
    for(;;) {}
  } else
      Serial.println("SD Card mounted");

  for (int i = 0; i < picoAudio->FileList()->size(); i++) {
      Serial.printf("file %d: %s\r\n", i, picoAudio->FileList()->get(i));
  }

  picoAudio->setVolumeMax();
  picoAudio->playFile();
}

void loop()
{
    picoAudio->process();
}

// optional functions called by audio library

void audio_info(const char *info){
    Serial.print("info        ");
    Serial.println(info);
}

void audio_id3data(const char *info) { //id3 metadata
  Serial.print("id3data     ");
  Serial.println(info);

  char *token;
  const char delim[2] = ":";
  token = strtok((char *)info, delim);
  for (int i = 0; i < strlen(token); i++)
    token[i] = tolower(token[i]);

  // artist
  if (strcmp(token, "artist") == 0)
  {
    strcpy(id3data.Artist, token + strlen(token) + 2);
    updateLCD();
  }

  // Album
  if (strcmp(token, "album") == 0)
  {
    strcpy(id3data.Album, token + strlen(token) + 2);
    updateLCD();
  }

  // title
  if (strcmp(token, "title") == 0)
  {
    strcpy(id3data.Title, token + strlen(token) + 2);
    updateLCD();
  }
}

void audio_eof_mp3(const char *info) {  //end of file
  Serial.print("eof_mp3     ");
  Serial.println(info);
  playNext();
}

void audio_showstation(const char *info) {
  Serial.print("station     ");
  Serial.println(info);
}

void audio_showstreaminfo(const char *info) {
  Serial.print("streaminfo  ");
  Serial.println(info);
}

void audio_showstreamtitle(const char *info) {
  Serial.print("streamtitle ");
  Serial.println(info);
}

void audio_bitrate(const char *info) {
  Serial.print("bitrate     ");
  Serial.println(info);
}

void audio_commercial(const char *info) {  //duration in sec
  Serial.print("commercial  ");
  Serial.println(info);
}

void audio_icyurl(const char *info) { //homepage
  Serial.print("icyurl      ");
  Serial.println(info);
}

void audio_lasthost(const char *info) { //stream URL played
  Serial.print("lasthost    ");
  Serial.println(info);
}

void audio_eof_speech(const char *info){
  Serial.print("eof_speech  ");
  Serial.println(info);
}
