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

PicoAudio::PicoAudio(bool initAudio) {
  ioExpander = new GPIOExpander(PE_ADDR, PE_SDA, PE_SCL);
  if (initAudio) {
    audio = new Audio();
    audio->setVolume(21); // 0...21
  }
  fileList = new LinkedList<char *>();
}

void PicoAudio::begin() {
  ioExpander->begin();
}

void PicoAudio::process() {
  ioExpander->process();
  audio->loop();
}

void PicoAudio::process(bool processAudio, bool processGPIO) {
  if (processAudio)
    audio->loop();
  if (processGPIO)
    ioExpander->process();
}

bool PicoAudio::connecttoHost(String sdfile) {
  return audio->connecttohost(sdfile);
}

bool PicoAudio::connecttoSD(String sdfile) {
  return audio->connecttoSD(sdfile);
}

bool PicoAudio::pauseResume() {
  return audio->pauseResume();
}

bool PicoAudio::isPlaying() {
  return audio->isPlaying();
}

void PicoAudio::setVolume(uint8_t vol){ // vol 22 steps, 0...21
  audio->setVolume(vol);
}

bool PicoAudio::scanSD() {
  if (!SD.begin())
    return false;

  populate(SD, "/mp3");
}

void PicoAudio::populate(fs::FS &fs, const char* dirname) {
    File root = fs.open(dirname);

    if(!root) {
        return;
    }
    if(!root.isDirectory()){
        return;
    }

    File file = root.openNextFile();
    char extension[5] { 0x0, 0x0, 0x0, 0x0, 0x0 };
    while(file){
        if(file.isDirectory()){
            populate(fs, file.name());
        } else {
          char *temp = (char *)malloc(strlen(file.name()) + 1);
          strcpy(temp, file.name());
          strncpy(extension, temp+(strlen(temp)-4), 4);
          extension[1] = toupper(extension[1]);
          extension[2] = toupper(extension[2]);
          extension[3] = toupper(extension[3]);
          if (strcmp(extension, ".MP3") == 0)
            fileList->add(temp);
          else
            delete temp;
        }
        file = root.openNextFile();
    }
}

int PicoAudio::playFile() {
  if (fileList->size() == 0)
    return -1;

  connecttoSD(fileList->get(filePos));
  return filePos;
}

int PicoAudio::playNext() {
  if (fileList->size() == 0)
    return -1;

  filePos++;
  if (filePos >= fileList->size()) {
    filePos = 0;
  }
  
  connecttoSD(fileList->get(filePos));
  return filePos;
}

int PicoAudio::playPrev() {
  if (fileList->size() == 0)
    return -1;

  filePos--;
  if (filePos < 0) {
    filePos = fileList->size()-1;
  }

  connecttoSD(fileList->get(filePos));
  return filePos;
}

bool PicoAudio::RegisterButtonCB(GPIOEvents::chngCBFn fn, void *data) {
  if (ioExpander == NULL)
    return false;

  ioExpander->RegisterChangeCB(fn, data);
}

uint8_t PicoAudio::readIO(uint8_t pin) {
  if (ioExpander == NULL)
    return -1;

  return ioExpander->read(pin);
}

void PicoAudio::writeIO(const uint8_t pin, const uint8_t value) {
  if (ioExpander == NULL)
    return;

  ioExpander->write(pin, value);
}

void PicoAudio::I2S_Init(i2s_mode_t MODE, i2s_bits_per_sample_t BPS, i2s_port_t PORT, i2s_channel_fmt_t FORMAT, int BCLK, int LRC, int DIN, int DOUT) {
  i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | MODE),
    .sample_rate = SAMPLE_RATE,
    .bits_per_sample = BPS,
    .channel_format = FORMAT,
    .communication_format = (i2s_comm_format_t)(I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_MSB),
    .intr_alloc_flags = 0,
    .dma_buf_count = 16,
    .dma_buf_len = 60
  };
  i2s_pin_config_t pin_config = {
    .bck_io_num = BCLK,
    .ws_io_num = LRC,
    .data_out_num = DOUT,
    .data_in_num = DIN
  };
  i2s_driver_install(PORT, &i2s_config, 0, NULL);
  i2s_set_pin(PORT, &pin_config);
  i2s_set_clk(PORT, SAMPLE_RATE, BPS, I2S_CHANNEL_STEREO);
}

void PicoAudio::initDAC() {
  I2S_Init(I2S_MODE_TX, I2S_BITS_PER_SAMPLE_32BIT, I2S_NUM_0, I2S_CHANNEL_FMT_RIGHT_LEFT, PA_DAC_BCLK, PA_DAC_LRC, PA_DAC_DIN, PA_DAC_DOUT);
}

void PicoAudio::initMIC() {
  I2S_Init(I2S_MODE_RX, I2S_BITS_PER_SAMPLE_32BIT, I2S_NUM_1, I2S_CHANNEL_FMT_RIGHT_LEFT, PA_MIC_BCLK, PA_MIC_LRC, PA_MIC_DIN, PA_MIC_DOUT);
}

int PicoAudio::readMIC(char* data, int numData) {
  return i2s_read_bytes(I2S_NUM_1, (char *)data, numData, portMAX_DELAY);
}

void PicoAudio::writeDAC(char* data, int numData) {
  i2s_write_bytes(I2S_NUM_0, (const char *)data, numData, portMAX_DELAY);
}

