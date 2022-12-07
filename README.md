# arduino-stopwatch

For Being Human Hall "hang time" exhibit

Forked from [clockspot/arduino-clock](https://github.com/clockspot/arduino-clock)

# Configuration, compilation, and upload

Various options, such as enabled functionality, RTC, display, I/O pins, timeouts, and control behaviors, are specified in a config file. This allows you to maintain configs for multiple clock hardware profiles, and simply include the relevant config at the top of `arduino-clock.h` before compiling.

I use the Arduino IDE to compile and upload, due to the use of various Arduino and Arduino-oriented libraries. Make sure the relevant libraries are installed in the Library Manager, per the config in use.

* EEPROM (Arduino) for AVR Arduinos (e.g. classic Nano)
* SPI (Ardunio) and [LedControl](http://wayoda.github.io/LedControl) for MAX7219-based matrix displays
* GFX and LEDBackpack (Adafruit) for HT16K33-based 7-segment displays

Before compiling and uploading, you will need to select the correct board, port, and (for AVR) processor in the IDE’s Tools menu.

* If your Arduino does not appear as a port option, you may have a clone that requires [drivers for the CH340 chipset](https://sparks.gogo.co.nz/ch340.html).
* If upload fails for an ATMega328P Arduino (e.g. classic Nano), try selecting/unselecting “Old Bootloader” in the processor menu.