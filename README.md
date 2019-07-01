# Pico Audio
The audio solution dedicated to the TinyPICO.
## About
The Pico Audio is a fully featured audio solution specially made for the TinyPICO ESP32 development board. The Pico Audio features an I2S Stereo DAC, I2S Mono MEMS Microphone and a builtin 3.7W Stereo Amplifier with built-in MicroSD card reader.

The Pico Audio utilises a low power NXP UDA1334A IC featuring a clear digital 16/20/24 bit, 44.1kHz, stereo audio DAC. Accompanying the DAC is a Knowles SPH0645 I2S digital mono microphone. This miniature low power MEMS microphone provides a low noise, 24 bit, 44.1kHz stream. Complimenting the DAC is the powerful Maxim MAX98306 3.7W stereo class D amplifier.

The Pico Audio has a mounting socket to host the TinyPICO development board and connects all of these great features together in a seamless and easy to utilise footprint measuring a tiny 32 mm x 47 mm (1.2in x 1.86in).
## PCB Layout
![XCopy Board Image](https://github.com/AdamKeher/Pico-Audio/blob/master/files/board.png)
![XCopy Board Image](https://github.com/AdamKeher/Pico-Audio/blob/master/files/route.png)
## Features
The software and hardware used in this project has been made possible by the principle of open hardware and software. Many of the samples have been collected together from a variety of authors and edited to work out of the box with the Pico Audio for instructional purposes. Please consult each sample project for relevant license and credit.
## Features
* TinyPICO Compatible
  * Simply plug your TinyPICO board into the Pico Audio and your ready to go
* Audio Out
  * Switching 3.5mm headphone jack. Plug in headphones or line out to an external device.
  * Stereo screw clamp audio terminals. The speaker terminals automatically mute when the 3.5mm jack is used.
* Built-in MicroSD Card
  * Quality Molex mechanism
  * Simple SPI interface
  * Activity LED light
  * Push-Push socket with overhanging edge mounted card for easy insertion
* UDA1334A Stereo DAC
  * Low power audio DAC
  * Integrated digital filter and switchable digital de-emphasis for 44.1 kHz frequency
  * Supports samples from 16 to 100 kHz
  * Simple I2S interface
  * 16/20/24 bit audio
  * High linearity, wide dynamic range and low distortion
* MAX98306 Stereo Amplifier
  * Low power efficient operation
  * 3.7W @ 3Ohm, 10% THD / 1.7W at 8Ohm, 10% THD
  * Spread Spectrum Modulation
  * Five selectable gain levels
  * Excellent click and pop suppression
* SPH0645LM4H MEMS Microphone
  * Low power design
  * Simple I2S interface
  * Flat frequency response
  * 24 bit, 44.1kHz audio
  * High SNR of 65dB(A)
  * RF shielded
  * Omnidirectional
## Sample Projects Modified for Pico Audio
* Stream net radio stations with MP3 support
* Play audio from SD card with MP3 support
* Simple Amazon Alexa
* Record audio to SD card
