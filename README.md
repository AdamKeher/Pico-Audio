# picoAUDIO
The audio solution dedicated to the TinyPICO.

Arduino Support Library: https://github.com/AdamKeher/Pico-Audio-Arduino
3d printable base: https://github.com/AdamKeher/Pico-Audio/tree/master/base
## About
The picoAUDIO is a fully featured audio solution specially made for the TinyPICO ESP32 development board, featuring an I2S Stereo DAC, I2S Mono MEMS Microphone, builtin 3.7W Stereo Amplifier, dedicated Headphone Amplifier, I2C GPIO Extender and built-in MicroSD card reader.

We utilise a low power NXP UDA1334A IC featuring a clear digital 16/20/24 bit, 44.1kHz, stereo audio DAC. Accompanying the DAC is a Knowles SPH0645 I2S digital mono microphone. This miniature low power MEMS microphone provides a low noise, 24 bit, 44.1kHz stream. Complimenting the DAC is the powerful Maxim MAX98306 3.7W stereo class D amplifier with stero speaker terminals and a Texas Instrumets TPA6132A2 stereo headphone amplifier with 3.5mm audio jack.

A simple mounting socket hosts the TinyPICO development board and connects all of these great features together in a seamless and easy to utilise footprint measuring a tiny 33 mm x 57 mm (1.29in x 2.24in). To provide maximum connectivity the board features a built-in MicroSD card reader and GPIO extender to hookup 8 inputs and outputs for all of your button and LED needs.
## PCB Layout
![XCopy Board Image](https://github.com/AdamKeher/Pico-Audio/blob/master/files/board.png)
![XCopy Board Image](https://github.com/AdamKeher/Pico-Audio/blob/master/files/route.png)
## Features
The software and hardware used in this project have been made possible by the principle of open hardware and software. Many of the samples have been collected together from a variety of authors and modified to work out of the box with the picoAUDIO for instructional purposes. Please consult each sample project for relevant license and credit.
## Features
* TinyPICO Compatible
  * Simply plug your TinyPICO board into the picoAUDIO and your ready to go
* Audio Out
  * Dedicated Texas Instruments headphone amplifier for distortion free audio.
  * 3.5mm headphone jack. Plug in headphones or line out to an external device.
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
* TPA6132A2 Headphone Amplifier
  * 25mW Headphone Amplifier for 16/32Ω Headphones with 3.5mm Audio Jack  
  * Unlike most audioboards, the picoAUDIO features a dedicated headphone amplifer
  * Doesn't use caps to drive headphone audio, reducing unwanted noise and distortion
  * Click and pop supression
  * Low power amplifier @ 2.1mA typical supply current
  * Four selectable gain levels
* SPH0645LM4H MEMS Microphone
  * Low power design
  * Simple I2S interface
  * Flat frequency response
  * 24 bit, 44.1kHz audio
  * High SNR of 65dB(A)
  * RF shielded
  * Omnidirectional
* PCF8574 GPIO Extender
  * Low standby current
  * I2C programmable
  * Latched outputs with enough current to drive LEDs
  * 8 Input and Output ports wired to a header with 3 outputs also pre wired to onboard buttons.
## Sample Projects Modified for picoAUDIO
* Stream net radio stations with MP3 support
  * I2C 2x16 character LCD and ILI93411 320x240 TFT display samples provided
* Play audio from SD card with MP3 support
* Record audio to SD card
* Realtime full duplex audio. Vocoder example receives from mic and playback from DAC.