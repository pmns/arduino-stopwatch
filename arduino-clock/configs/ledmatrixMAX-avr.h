//AVR Arduino on UNDB but with LEDs and the light sensor attached

#ifndef CONFIG
#define CONFIG


///// Real-Time Clock /////
//If using no RTC (a fake RTC based on millis()):
#define RTC_MILLIS
#define ANTI_DRIFT 0 //msec to add/remove per second - or seconds to add/remove per day divided by 86.4 - to compensate for natural drift. Ifusing wifinina, it really only needs to be good enough for a decent timekeeping display until the next ntp sync. TIP: setting to a superhigh value is helpful for testing! e.g. 9000 will make it run 10x speed

//If using DS3231 (via I2C):
//Requires Wire library (standard Arduino)
//Requires DS3231 library by NorthernWidget to be installed in your IDE.
// #define RTC_DS3231


///// Inputs /////
//If using buttons for Select and optionally Alt:
#define INPUT_BUTTONS
#define CTRL_SEL A1
#define CTRL_ALT -1

#define INPUT_UPDN_BUTTONS
#define CTRL_UP -1
#define CTRL_DN -1

//For all input types:
//How long (in ms) are the hold durations?
#define CTRL_HOLD_SHORT_DUR 1000 //for entering setting mode, or hold-setting at low velocity (x1)
#define CTRL_HOLD_LONG_DUR 3000 //for entering settings menu, or hold-setting at high velocity (x10)
#define CTRL_HOLD_VERYLONG_DUR 5000 //for wifi info / admin start (Nano IoT without Alt only)
#define CTRL_HOLD_SUPERLONG_DUR 10000 //for wifi disconnect (Nano IoT) or EEPROM reset on startup
//What are the timeouts for setting and temporarily-displayed functions? up to 65535 sec
#define STOPWATCH_TIMEOUT 10


///// Display /////
//If using 4/6-digit 7-segment LED display with HT16K33 (I2C on SDA/SCL pins)
//Requires Adafruit libraries LED Backpack, GFX, and BusIO
//If 6 digits, edit Adafruit_LEDBackpack.cpp to replace "if (d > 4)" with "if (d > 6)"
//and, if desired, in numbertable[], replace 0x7D with 0x7C and 0x6F with 0x67 to remove
//the serifs from 6 and 9 for legibility (see http://www.harold.thimbleby.net/cv/files/seven-segment.pdf)
// #define DISPLAY_HT16K33
// //#define NUM_MAX 4 //How many digits?
// #define BRIGHTNESS_FULL 15 //out of 0-15
// #define BRIGHTNESS_DIM 0
// #define DISPLAY_ADDR 0x70 //0x70 is the default

//If using 8x32 LED matrix:
//Requires LedControl library by Eberhard Farle to be installed in IDE. (http://wayoda.github.io/LedControl)
#define DISPLAY_MAX7219
#define NUM_MAX 4 //How many modules? 3 for 8x24 (4 digit, untested) or 4 for 8x32 (6 digit)
#define ROTATE 90
#define BRIGHTNESS_FULL 7 //out of 0-15
#define BRIGHTNESS_DIM 0
//I've found that 7 (or 15?) and 0 make the least noise
//Which output pins?
#define CLK_PIN 2 //D2, pin 20
#define CS_PIN 3 //D3, pin 21
#define DIN_PIN 4 //D4, pin 22
//and GND and VCC 5V

//For all display types:
#define DISPLAY_SIZE 4 //number of digits in display module: 6 or 4

#endif