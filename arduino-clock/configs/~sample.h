//This config includes all options this project supports (displays, inputs, etc).
//You can make a copy and include only the portions relevant to your clock's hardware.

#ifndef CONFIG
#define CONFIG


///// Functionality /////

// Which functionality is enabled in this clock?
// Related settings will also be enabled in the settings menu.
// The operating instructions assume all of these are enabled except temp and tubetest.
#define ENABLE_DATE_FN true // Date function, optionally including pages below
#define ENABLE_DATE_COUNTER true // Adds date page with an anniversary counter
#define ENABLE_DATE_RISESET true // Adds date pages with sunrise/sunset times. Requires Dusk2Dawn library by DM Kichi to be installed in IDE.
#define ENABLE_ALARM_FN true
#define ENABLE_ALARM_AUTOSKIP true
#define ENABLE_ALARM_FIBONACCI true
#define ENABLE_TIMER_FN true
#define ENABLE_TIME_CHIME true
#define ENABLE_DIMMING true
#define ENABLE_AWAYMODE true
#define ENABLE_TEMP_FN false //Temperature per DS3231 - will read high – leave false for production
#define ENABLE_TUBETEST_FN false //Cycles through all tubes – leave false for production


///// Real-Time Clock /////
//These are mutually exclusive

//If using DS3231 (via I2C):
//Requires Wire library (standard Arduino)
//Requires DS3231 library by NorthernWidget to be installed in your IDE.
// #define RTC_DS3231

//If using RTCZero on Nano 33 IoT: //TODO
// #define RTC_ZERO

//If using no RTC (a fake RTC based on millis()):
#define RTC_MILLIS
#define ANTI_DRIFT 0 //msec to add/remove per second - or seconds to add/remove per day divided by 86.4 - to compensate for natural drift. If using wifinina, it really only needs to be good enough for a decent timekeeping display until the next ntp sync. TIP: setting to a superhigh value is helpful for testing! e.g. 9000 will make it run 10x speed


///// Inputs /////

//If using buttons for Select and optionally Alt:
#define INPUT_BUTTONS
#define CTRL_SEL A6 //UNDB S4/PL7
#define CTRL_ALT A7 //UNDB S7/PL14 - if not using Alt, set to -1

//Up and Down can be buttons OR a rotary control:

//If using buttons for Up and Down:
#define INPUT_UPDN_BUTTONS
#define CTRL_UP A0 //UNDB S3/PL6
#define CTRL_DN A1 //UNDB S2/PL5

//If using rotary control for Up and Down:
//Requires Encoder library by Paul Stoffregen to be installed in IDE.
// #define INPUT_UPDN_ROTARY
// #define CTRL_R1 A2
// #define CTRL_R2 A3
// #define ROT_VEL_START 80 //If step rate falls below this, kick into high velocity set (x10)
// #define ROT_VEL_STOP 500 //If step rate rises above this, drop into low velocity set (x1)

//If using IMU motion sensor on Nano 33 IoT:
//To use, tilt clock: backward=Sel, forward=Alt, left=Down, right=Up
//This is mutually exclusive with the button/rotary controls.
// #define INPUT_IMU
// #define IMU_DEBOUNCING 150 //ms
//How is the Arduino oriented inside the clock? Include one each of USB_DIR and IC_DIR to indicate which way the USB port and IC (front side) are oriented, respectively. For UNDB clocks, it's USB_DIR_UP and IC_DIR_BACK.
// #define USB_DIR_UP
// #define USB_DIR_DOWN
// #define USB_DIR_LEFT
// #define USB_DIR_RIGHT
// #define USB_DIR_FRONT
// #define USB_DIR_BACK
// #define IC_DIR_UP
// #define IC_DIR_DOWN
// #define IC_DIR_LEFT
// #define IC_DIR_RIGHT
// #define IC_DIR_FRONT
// #define IC_DIR_BACK

//For all input types:
//How long (in ms) are the hold durations?
#define CTRL_HOLD_SHORT_DUR 1000 //for entering setting mode, or hold-setting at low velocity (x1)
#define CTRL_HOLD_LONG_DUR 3000 //for entering settings menu, or hold-setting at high velocity (x10)
#define CTRL_HOLD_VERYLONG_DUR 5000 //for wifi info / admin start (Nano IoT without Alt only)
#define CTRL_HOLD_SUPERLONG_DUR 10000 //for wifi disconnect (Nano IoT) or EEPROM reset on startup
//What are the timeouts for setting and temporarily-displayed functions? up to 65535 sec
#define SETTING_TIMEOUT 300 //sec
#define FN_TEMP_TIMEOUT 5 //sec
#define FN_PAGE_TIMEOUT 3 //sec

//Unused inputs
//A3 //UNDB S5/PL8
//A2 //UNDB S6/PL9


///// Display /////
//These are mutually exclusive

//If using nixie array:
// #define DISP_NIXIE
// #define CLEAN_SPEED 200 //ms - "frame rate" of tube cleaning
// //Which output pins?
// //This clock is 2x3 multiplexed: two tubes powered at a time.
// //The anode channel determines which two tubes are powered,
// //and the two SN74141 cathode driver chips determine which digits are lit.
// //4 pins out to each SN74141, representing a binary number with values [1,2,4,8]
// #define OUT_A1 2
// #define OUT_A2 3
// #define OUT_A3 4
// #define OUT_A4 5
// #define OUT_B1 6
// #define OUT_B2 7
// #define OUT_B3 8
// #define OUT_B4 16 //aka A2
// //3 pins out to anode channel switches
// #define ANODE_1 11
// #define ANODE_2 12
// #define ANODE_3 13

//If using 8x32 LED matrix:
//Requires LedControl library by Eberhard Farle to be installed in IDE. (http://wayoda.github.io/LedControl)
#define DISP_MAX7219
#define NUM_MAX 4 //How many modules? 3 for 8x24 (4 digit, untested) or 4 for 8x32 (6 digit)
#define ROTATE 90
#define BRIGHTNESS_FULL 7 //out of 0-15
#define BRIGHTNESS_DIM 0
//I've found that 7 (or 15?) and 0 make the least noise
//Which output pins?
#define CLK_PIN 2 //D2, pin 20
#define CS_PIN 3 //D3, pin 21
#define DIN_PIN 4 //D4, pin 22

///// Display /////
//If using 4/6-digit 7-segment LED display with HT16K33 (I2C on SDA/SCL pins)
//Requires Adafruit libraries LED Backpack, GFX, and BusIO
//If 6 digits, edit Adafruit_LEDBackpack.cpp to replace "if (d > 4)" with "if (d > 6)"
//and, if desired, in numbertable[], replace 0x7D with 0x7C and 0x6F with 0x67 to remove
//the serifs from 6 and 9 for legibility (see http://www.harold.thimbleby.net/cv/files/seven-segment.pdf)
#define DISP_HT16K33
//#define NUM_MAX 4 //How many digits?
#define BRIGHTNESS_FULL 15 //out of 0-15
#define BRIGHTNESS_DIM 0
#define DISP_ADDR 0x70 //0x70 is the default

//For all display types:
#define DISPLAY_SIZE 6 //number of digits in display module: 6 or 4
#define UNOFF_DUR 10 //sec - when display is off, an input will illuminate for how long?
#define SCROLL_SPEED 100 //ms - "frame rate" of digit scrolling, e.g. date at :30 option


///// Ambient Light Sensor /////
//If using VEML 7700 Lux sensor (I2C on SDA/SCL pins)
//Requires Adafruit library VEML7700
#define LIGHTSENSOR_VEML7700
#define LUX_FULL 400 //lux at/above which display should be at its brightest (per config)
#define LUX_DIM 30 //lux at/below which display should be at its dimmest (per config)

//If any type of light sensor is in use:
#define LIGHTSENSOR


///// Other Outputs /////

//What are the pins for each signal type? -1 to disable that signal type
#define PIEZO_PIN -1 //Drives a piezo beeper
#define SWITCH_PIN -1 //Switched to control an appliance like a radio or light fixture. If used with timer, it will switch on while timer is running (like a "sleep" function). If used with alarm, it will switch on when alarm trips; specify duration of this in SWITCH_DUR. (A3 for UNDB v9)
#define PULSE_PIN -1 //Simple pulses to control an intermittent signaling device like a solenoid or indicator lamp. Specify pulse duration in RELAY_PULSE. Pulse frequency behaves like the piezo signal.
//Default signal type for each function:
//0=piezo, 1=switch, 2=pulse
#define ALARM_SIGNAL 0
#define TIMER_SIGNAL 0
#define CHIME_SIGNAL 0
#define SIGNAL_DUR 180 //sec - when piezo/pulse signal is going, it's pulsed once/sec for this period (e.g. 180 = 3min)
#define SWITCH_DUR 7200 //sec - when alarm triggers switch signal, it's switched on for this period (e.g. 7200 = 2hr)
#define PULSE_LENGTH 200 //ms - length of pulse signal's individual pulses (e.g. to drive a solenoid to ring a bell)

//Soft power switches
#define ENABLE_SOFT_ALARM_SWITCH 1
// 1 = yes. Alarm can be switched on and off when clock is displaying the alarm time (FN_ALARM).
// 0 = no. Alarm will be permanently on. Use with switch signal if the appliance has its own switch on this circuit (and note that, if another signal type(s) is available and selected for the alarm, the user won't be able to switch it off). Also disables skip feature. Note that the instructions do not reflect this option.
#define ENABLE_SOFT_POWER_SWITCH 1 //switch signal only
// 1 = yes. Switch signal can be toggled on and off directly with Alt button at any time (except in settings menu). This is useful if connecting an appliance (e.g. radio) that doesn't have its own switch, or if replacing the clock unit in a clock radio where the clock does all the switching (e.g. Telechron).
// 0 = no. Use if the connected appliance has its own power switch (independent of this circuit, e.g. some Sony Digimatic clock radios) or does not need to be manually switched. In this case (and/or if there is no switch signal option, and if no Wi-Fi support) Alt will act as a function preset. Note that the instructions do not reflect this option.

//Backlighting control
#define BACKLIGHT_PIN -1 // -1 to disable feature; 9 if equipped (UNDB v9)
#define BACKLIGHT_FADE 0 // 1 to fade via PWM (must use PWM pin and PWM-supportive lighting); 0 to simply switch on and off


#endif