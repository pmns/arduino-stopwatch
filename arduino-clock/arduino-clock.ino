//Forked from https://github.com/clockspot/arduino-clock

#include <arduino.h>
#include "arduino-clock.h"

////////// Software version //////////
const byte vMajor = 0;
const byte vMinor = 0;
const byte vPatch = 1;
const bool vDev = 1;

////////// Includes //////////

// These modules are used per the available hardware and features enabled in the config file.
// The disp and rtc options are mutually exclusive and define the same functions.
#include "dispMAX7219.h" //active if DISPLAY_MAX7219 is defined in config - for a SPI MAX7219 8x8 LED array
#include "dispHT16K33.h" //active if DISPLAY_HT16K33 is defined in config - for an I2C 7-segment LED display
#include "rtcDS3231.h" //active if RTC_DS3231 is defined in config – for an I2C DS3231 RTC module
#include "rtcMillis.h" //active if RTC_MILLIS is defined in config – for a fake RTC based on millis
#include "input.h" //for Sel/Alt/Up/Dn - supports buttons, rotary control, and Nano 33 IoT IMU

#ifdef ENABLE_NEOPIXEL
  #include <Adafruit_NeoPixel.h>
  #define NUMPIXELS 1
  Adafruit_NeoPixel pixels(NUMPIXELS, PIN_NEOPIXEL, NEO_GRB + NEO_KHZ800);
#endif


////////// Variables and storage //////////

byte timerState = 0b10; //bit 0 is stop/run, bit 1 is down/up, bit 2 is runout repeat / short signal, bit 3 is runout chrono, bit 4 is lap display
unsigned long timerTime = 0; //timestamp of timer target / chrono origin (while running) or duration (while stopped)
//poop unsigned long timerLapTime = 0; 
const byte millisCorrectionInterval = 30; //used to calibrate millis() to RTC for timer/chrono purposes
unsigned long millisAtLastCheck = 0;

#define SHOW_SERIAL 0 //for debugging


////////// Main code control //////////

void setup(){
  if(SHOW_SERIAL) {
    Serial.begin(115200);
    #ifndef __AVR__ //SAMD only
    while(!Serial);
    #else
    delay(1);
    #endif
    Serial.println(F("Hello world"));
  }
  rtcInit();
  initDisplay();
  initOutputs();
  initInputs();
  updateDisplay();
  

#ifdef ENABLE_NEOPIXEL
  #if defined(NEOPIXEL_POWER)
    // If this board has a power control pin, we must set it to output and high
    // in order to enable the NeoPixels. We put this in an #if defined so it can
    // be reused for other boards without compilation errors
    pinMode(NEOPIXEL_POWER, OUTPUT);
    digitalWrite(NEOPIXEL_POWER, HIGH);
  #endif
  
  pixels.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
  pixels.setBrightness(20); // not so bright

  pixels.fill(0xFFFFFF);
  pixels.show();
#endif

}

void loop(){
  //Every loop cycle, check the RTC and inputs (previously polled, but works fine without and less flicker)
  checkRTC(false); //if clock has ticked, decrement timer if running, and updateDisplay
  millisApplyDrift();
  checkInputs(); //if inputs have changed, this will do things + updateDisplay as needed
  cycleTimer();
  cycleDisplay( //keeps the display hardware multiplexing cycle going
    2, //displayBrightness, //the display normal/dim/off state
    false,
    0,
    0 //if we are setting
  );
}



////////// Input handling and value setting //////////

void ctrlEvt(byte ctrl, byte evt, byte evtLast, bool velocity){
  //Handle control events from inputs, based on current fn and set state.
  //evt: 1=press, 2=short hold, 3=long hold, 4=verylong, 5=superlong, 0=release.
  //We only handle press evts for up/down ctrls, as that's the only evt encoders generate,
  //and input.cpp sends repeated presses if up/down buttons are held.
  //But for sel/alt (always buttons), we can handle different hold states here.

  //For the stopwatch, controls are greatly simplified.
  //There's only one control. If it is pressed, start the timer if not already running.
  if(ctrl==CTRL_SEL) {
    Serial.println(evt,DEC);
    if(evt==1){
      if(!(timerState&1)) timerStart(); //if stopped
    }
    if(evt==0){
      timerStop();
    }
  }
  
} //end ctrlEvt


////////// Timing and timed events //////////

byte rtcSecLast = 61;
byte rtcDid = 0; //bitmask to indicate whether we just did things that would be bad to overdo, in case checkRTC is called multiple times during the trigger period (such as if fn is changed during second :00). bit 0 = timer drift correction. At this time other things are OK to overdo since they just restart the same action (alarm/chime signal triggering, date/anti-poisoning display routines) or have other means of canceling (DST change, per DST flag). TODO could get away from this if we move the force stuff into another method?
void checkRTC(bool force){
  //Checks display timeouts;
  //checks for new time-of-day second -> decrements timeouts and checks for timed events;
  //updates display for "running" functions.
  unsigned long now = millis();
  
  //Things to do every time this is called: timeouts to reset display. These may force a tick.
  //Stopwatch timeout: If the stopwatch has been stopped long enough, clear it
  if(timerTime && !(timerState&1) && (unsigned long)(now-getInputLast())>=STOPWATCH_TIMEOUT*1000){
    timerClear();
  }
  
  //Update things based on RTC
  rtcTakeSnap();
  
  if(rtcSecLast != rtcGetSecond() || force) { //If it's a new RTC second, or we are forcing it

    //Things to do at specific times
    word todmins = rtcGetHour()*60+rtcGetMinute();
    
    //Timer drift correction: per the millisCorrectionInterval
    if(rtcGetSecond()%millisCorrectionInterval==0){ //if time:
      if(!(rtcDid&1)) millisCheckDrift(); bitWrite(rtcDid,0,1); //do if not done, set as done
    } else bitWrite(rtcDid,0,0); //if not time: set as not done

    //Finally, update the display, whether natural tick or not
    updateDisplay();
    
    rtcSecLast = rtcGetSecond();
    
#ifdef ENABLE_NEOPIXEL
    switch(rtcSecLast%3) {
      case 0: pixels.fill(0xFF0000); pixels.show(); break;
      case 1: pixels.fill(0x00FF00); pixels.show(); break;
      case 2: pixels.fill(0x0000FF); pixels.show(); break;
      default: break;
    }
#endif
    
  } //end if force or new second
} //end checkRTC()


// Chrono/Timer
// There are potentially two timing sources – the Arduino itself (eg millis()), which gives subsecond precision but isn't very accurate, so it's only good for short-term timing and taking action in response to user activity (eg button press hold thresholds); and the rtc, which is very accurate but only gives seconds, so it's only good for long-term timing and taking action in response to time of day. The one place we need both short-term precision and long-term accuracy is in the chrono/timer – so I have based it on millis() but with an offset applied to correct for its drift, periodically adjusted per the rtc.
unsigned long millisDriftOffset = 0; //The cumulative running offset. Since it's circular, doesn't matter whether signed or not.
//unsigned long millisAtLastCheck (defined at top, so ctrlEvt can reset it when setting RTC). 0 when unreliable (at start and after RTC set).
//const byte millisCorrectionInterval (defined at top, so checkRTC can see it)
int millisDriftBuffer = 0; // Each time we calculate millis() drift, we add it to this signed buffer, which gets applied to millisDriftOffset slowly to smooth the correction and minimize (eliminate?) the chance of a discontinuity, which prevents unsightly glitches in the chrono/timer and the signal performance, and failures in eg detecting button presses.
// TODO the adjustments are a little noisy in the short term, because of a rolling offset between the loop cycle (slowed down by cycleDisplay's delays) and the rtc ticks. It's kind of academic since the variance is probably only around ±.02sec per adjustment at most (largely the duration of cycleDisplay's delays), which I'd say is within the tolerance of display/button/human limitations, but no point doing it as quickly as once per second I think.
void millisCheckDrift(){
  unsigned long now = millis();
  if(millisAtLastCheck){ // if this has a value, check to see how much millis has drifted since then. If this is 0, it means either the RTC was recently set (or the extremely unlikely case that the last sync occurred exactly at millis rollover) so we will hold off until next drift check.
    long millisDrift = now-(millisAtLastCheck+(millisCorrectionInterval*1000)); // Converting difference to a signed long.
    if(abs((long)(millisDrift+millisDriftBuffer))>32767){} // If adding drift to buffer would make it overflow, ignore it this time
    else {
      millisDriftBuffer -= millisDrift;
      // rtcTakeSnap();
      // if(rtcGetHour()<10) Serial.print(F("0"));
      // Serial.print(rtcGetHour(),DEC);
      // Serial.print(F(":"));
      // if(rtcGetMinute()<10) Serial.print(F("0"));
      // Serial.print(rtcGetMinute(),DEC);
      // Serial.print(F(":"));
      // if(rtcGetSecond()<10) Serial.print(F("0"));
      // Serial.print(rtcGetSecond(),DEC);
      // Serial.print(F("  millis: "));
      // Serial.print(now,DEC);
      // Serial.print(F("  drift: "));
      // Serial.print(millisDrift,DEC);
      // Serial.print(F("  new buffer: "));
      // Serial.print(millisDriftBuffer,DEC);
      // Serial.println();
    }
  }
  millisAtLastCheck = now;
}
void millisApplyDrift(){
  //Applies millisDriftBuffer to millisDriftOffset at the rate of 1ms per loop. See above for details.
  if(millisDriftBuffer){
    millisDriftOffset += (millisDriftBuffer>0? 1: -1);
    millisDriftBuffer -= (millisDriftBuffer>0? 1: -1);
    // tod = rtc.now();
    // if(rtcGetHour()<10) Serial.print(F("0"));
    // Serial.print(rtcGetHour(),DEC);
    // Serial.print(F(":"));
    // if(rtcGetMinute()<10) Serial.print(F("0"));
    // Serial.print(rtcGetMinute(),DEC);
    // Serial.print(F(":"));
    // if(rtcGetSecond()<10) Serial.print(F("0"));
    // Serial.print(rtcGetSecond(),DEC);
    // Serial.print(F("  new offset: "));
    // Serial.print(millisDriftOffset,DEC);
    // Serial.print(F("  new buffer: "));
    // Serial.print(millisDriftBuffer,DEC);
    // Serial.println();
  }
}
void millisReset(){
  millisAtLastCheck = 0; //because setting the RTC makes this unreliable
}
unsigned long ms(){
  // Returns millis() with the drift offset applied, for timer/chrono purposes.
  // WARNING: Since the offset is being periodically adjusted, there is the possibility of a discontinuity in ms() output – if we give out a timestamp and then effectively set the clock back, the next timestamp might possibly be earlier than the last one, which could mess up duration math. I tried to think of a way to monitor for that discontinuity – e.g. if now-then is greater than then-now, due to overflow – but it gets tricky since millis() is effectively circular, and that condition occurs naturally at rollover as well – so I think we would need a flag that millisCheckDrift sets when it sets the offset backward, and ms clears when the real time has caught up.... or something like that.
  // So for now we'll restrict use of ms() to the timer/chrono duration – the only place we really need it, and fortunately it's not a problem here, because that duration never exceeds 100 hours so we can easily detect that overflow. And the only time it might really be an issue is if it occurs immediately after the chrono starts, since it would briefly display the overflowed time – in that case we'll just reset the timer to 0 and keep going.
  return (unsigned long)(millis()+millisDriftOffset);
}
void timerStart(){
  timerTime = 0; //stopwatch should always start from zero
  bitWrite(timerState,0,1); //set timer running (bit 0) to on (1)
  if(timerTime==0) bitWrite(timerState,1,1); //set timer direction (bit 1) to up (1) if we were idle
  //When the timer is stopped, timerTime holds a duration, independent of any start/stop time.
  //Convert it to a timestamp:
  //If chrono (count up), timestamp is an origin in the past: now minus duration.
  //If timer (count down), timestamp is a destination in the future: now plus duration.
  timerTime = ((timerState>>1)&1? ms() - timerTime: ms() + timerTime);
} //end timerStart()
void timerStop(){
  bitWrite(timerState,0,0); //set timer running (bit 0) to off (0)
  //When the timer is running, timerTime holds a timestamp, which the current duration is continuously calculated from.
  //Convert it to a duration:
  //If chrono (count up), timestamp is an origin in the past: duration is now minus timestamp.
  //If timer (count down), timestamp is a destination in the future: duration is timestamp minus now.
  timerTime = ((timerState>>1)&1? ms() - timerTime: timerTime - ms());
  updateDisplay(); //since cycleTimer won't do it
}
void timerClear(){
  bitWrite(timerState,0,0); //set timer running (bit 0) to off (0)
  timerTime = 0; //set timer duration
  updateDisplay();
}
void cycleTimer(){
  if(timerState&1){ //If the timer is running
    //Check if we've hit a wall
    if((unsigned long)(ms()-timerTime)>=360000000){ //find out if now is over the amount we can display (100h, or 360M ms)
      //chrono has maxed out
      // if it has maxed out by a LOT, it was probably due to a rare overflow possibility right after starting (see ms()) – so just restart.
      if((unsigned long)(ms()-timerTime)>=370000000) timerTime = ms();
      else timerClear();
      //Serial.println(F("wat"));
    }
    updateDisplay();
  }
} //end cycleTimer()

void updateDisplay(){
  //Run as needed to update display when the value being shown on it has changed
  //This formats the new value and puts it in displayNext[] for cycleDisplay() to pick up

  unsigned long td; td = (!(timerState&1)? timerTime: //If stopped, use stored duration
    //If running, use same math timerStop() does to calculate duration
    ((timerState>>1)&1? ms() - timerTime: //count up
      timerTime - ms() //count down
    )
  );
  byte tdc; tdc = (td%1000)/10; //capture hundredths (centiseconds)
  td = td/1000+(!((timerState>>1)&1)&&tdc!=0?1:0); //remove mils, and if countdown, round up
  //Countdown shows H:M:S, but on DISPLAY_SIZE<6 and H<1, M:S
  //Countup shows H:M:S, but if H<1, M:S:C, but if DISPLAY_SIZE<6 and M<1, S:C
  bool lz; lz = 1; //leading zeroes
  if((timerState>>1)&1){ //count up
    if(DISPLAY_SIZE<6 && td<60){ //under 1 min, 4-digit displays: [SS]CC--
      if(td>=1||lz) editDisplay(td,0,1,lz,true); else blankDisplay(0,1,true); //secs, leading per lz, fade
      editDisplay(tdc,2,3,td>=1||lz,false); //cents, leading if >=1sec or lz, no fade
      blankDisplay(4,5,true); //just in case 4-digit code's running on a 6-digit display, don't look ugly
    } else if(td<3600){ //under 1 hr: [MM][SS]CC
      if(td>=60||lz) editDisplay(td/60,0,1,lz,true); else blankDisplay(0,1,true); //mins, leading per lz, fade
      if(td>=1||lz) editDisplay(td%60,2,3,td>=60||lz,true); else blankDisplay(2,3,true); //secs, leading if >=1min or lz, fade
      editDisplay(tdc,4,5,td>=1||lz,false); //cents, leading if >=1sec or lz, no fade - hidden on 4-digit display
    } else { //over 1 hr: HHMMSS
      editDisplay(td/3600,0,1,lz,true); //hrs, leading per lz, fade
      editDisplay((td%3600)/60,2,3,true,true); //mins, leading, fade
      editDisplay(td%60,4,5,true,true); //secs, leading, fade
    }
  } else { //count down
    if(DISPLAY_SIZE<6 && td<3600){ //under 1 hr, 4-digit displays: [MM]SS--
      if(td>=60||lz) editDisplay(td/60,0,1,lz,true); else blankDisplay(0,1,true); //mins, leading per lz, fade
      if(td>=1||lz) editDisplay(td%60,2,3,td>=60||lz,true); else blankDisplay(2,3,true); //secs, leading if >=1min or lz, fade
      blankDisplay(4,5,true); //just in case 4-digit code's running on a 6-digit display, don't look ugly
    } else { //[HH][MM]SS
      if(td>=3600||lz) editDisplay(td/3600,0,1,lz,true); else blankDisplay(0,1,true); //hrs, leading per lz, fade
      if(td>=60||lz) editDisplay((td%3600)/60,2,3,td>=3600||lz,true); else blankDisplay(2,3,true); //mins, leading if >=1h or lz, fade
      editDisplay(td%60,4,5,td>=60||lz,true); //secs, leading if >=1m or lz, fade
    }
  }
       
} //end updateDisplay()


////////// Hardware outputs //////////

void initOutputs() {
}