#ifndef ARDUINO_CLOCK_H
#define ARDUINO_CLOCK_H

////////// Hardware configuration //////////
//Include the config file that matches your hardware setup. If needed, duplicate an existing one.

#include "configs/led7segHT-avr.h"

////////////////////////////////////////////


void setup();
void loop();
void ctrlEvt(byte ctrl, byte evt, byte evtLast, bool velocity=0);
// void fnScroll(byte dir);
// void fnOptScroll(byte dir);
// void goToFn(byte thefn, byte thefnPg=0);
// void switchAlarmState(byte dir);
// void setAlarmState(byte state);
// byte getAlarmState();
// void switchPower(byte dir);
// void startSet(int n, int m, int x, byte p);
// void doSet(int delta);
// void clearSet();
// bool initEEPROM(bool hard);
// void findFnAndPageNumbers();
void checkRTC(bool force);
// void fibonacci(byte h, byte m, byte s);
// void autoDST();
// bool isDST(int y, byte m, byte d);
// bool isDSTByHour(int y, byte m, byte d, byte h, bool setFlag);
// byte nthSunday(int y, byte m, byte nth);
// byte daysInMonth(word y, byte m);
// int daysInYear(word y);
// int dateToDayCount(word y, byte m, byte d);
// byte dayOfWeek(word y, byte m, byte d);
// int dateComp(int y, byte m, byte d, byte mt, byte dt, bool countUp);
// bool isTimeInRange(word tstart, word tend, word ttest);
// bool isDayInRange(byte dstart, byte dend, byte dtest);
void millisCheckDrift();
void millisApplyDrift();
void millisReset();
unsigned long ms();
void timerStart();
void timerStop();
void timerClear();
// void timerLap();
// void timerRunoutToggle();
void cycleTimer();
// void timerSleepSwitch(bool on);
byte getTimerState();
void setTimerState(char pos, bool val);
// void tempDisplay(int i0, int i1=0, int i2=0, int i3=0);
void updateDisplay();
// void calcSun();
// void displaySun(byte which, int d, int tod);
// void displayWeather(byte which);
void initOutputs();
// void signalStart(byte sigFn, byte sigDur);
// void signalStop();
// void cycleSignal();
// word getSignalPitch();
// word getHz(byte note);
// byte getSignalOutput();
// byte getSignalPattern();
// void quickBeep(int pitch);
// void quickBeepPattern(int source, int pattern);
// void updateBacklight();
// void cycleBacklight();
// byte getVersionPart(byte part);

#endif //ARDUINO_CLOCK_H