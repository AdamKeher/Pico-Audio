#include "PicoAudio.h"

// uncomment when using AK74 GPIO Expander Board module. It will cycle the LED's for testing
// #define EXPANDERBOARD 1

PicoAudio picoAudio = PicoAudio();

int outpin = 0;

void printGPIO(uint8_t ports) {
  // echos the lines
  Serial.printf("\033[2J\033[H");
  Serial.printf("Value: %d | Bin: ", ports);
  Serial.print(ports, BIN);
  Serial.printf(" | P0: %-5s | P1: %-5s | P2: %-5s | P3: %-5s | P4: %-5s | P5: %-5s | P6: %-5s | P7: %-5s |\r\n", 
    (ports & P0) == 0 ? "\033[32mTRUE \033[37m" : "\033[31mFALSE\033[37m", 
    (ports & P1) == 0 ? "\033[32mTRUE \033[37m" : "\033[31mFALSE\033[37m",
    (ports & P2) == 0 ? "\033[32mTRUE \033[37m" : "\033[31mFALSE\033[37m",
    (ports & P3) == 0 ? "\033[32mTRUE \033[37m" : "\033[31mFALSE\033[37m",
    (ports & P4) == 0 ? "\033[32mTRUE \033[37m" : "\033[31mFALSE\033[37m",
    (ports & P5) == 0 ? "\033[32mTRUE \033[37m" : "\033[31mFALSE\033[37m",
    (ports & P6) == 0 ? "\033[32mTRUE \033[37m" : "\033[31mFALSE\033[37m",
    (ports & P7) == 0 ? "\033[32mTRUE \033[37m" : "\033[31mFALSE\033[37m");
}

void GPIOChangeCallback(void *cbData, uint8_t ports, uint8_t button, bool state) {
  printGPIO(ports);
}

void setup()
{
  Serial.begin(115200);
  Serial.println("\nTEST PCF8574\n");

  picoAudio.begin();
  picoAudio.RegisterButtonCB(GPIOChangeCallback, (void*)"messageProcessor");

  // print intial screen with all buttons as false
  printGPIO(0xFF);
}

unsigned long lastBlink = 0;

void loop()
{
  picoAudio.process();

  #ifdef EXPANDERBOARD
  if (millis() - lastBlink > 50) {
    picoAudio.writeIO(outpin, LOW);
    int prevpin = outpin - 1;
    if (prevpin < 0)
      prevpin = 7;
    picoAudio.writeIO(prevpin, HIGH);
    outpin++;
    if (outpin > 7)
      outpin = 0;
    lastBlink = millis();
  }
  #endif
}