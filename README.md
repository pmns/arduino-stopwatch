# arduino-nixie

![Nixie clocks](https://i.imgur.com/FemMWax.jpg)

**A digital clock with perpetual calendar, alarm, countdown timer/appliance timer, and day counter.** Written for the Arduino Nano at the heart of [RLB Designs'](http://rlb-designs.com/) Universal Nixie Driver Board (UNDB), featuring a DS3231 real-time clock, and driving up to 6 digits multiplexed in pairs via two SN74141 driver chips. Includes support for LED and relay control for UNDB v8+.

[The latest release can be downloaded here.](https://github.com/clockspot/arduino-nixie/releases/latest) Skip to [Hardware Configuration](#hardware-configuration) for details on how to tweak the sketch.

## Clock Operating Instructions

_Note: Many variations are possible, depending on your clock's hardware; but this covers most cases._

### Clock Functions

* Press **Select** to cycle through the clock's functions. Some return to Time after a few seconds.
* To set, hold **Select** 'til the display flashes; use **Adjust** to set, and **Select** to save.

| Function | Looks like | Notes |
| --- | --- | --- |
| **Time** | `12 34 56` | The time of day. You can choose 12h or 24h format in the options menu (1). When setting, it's in 24h format (so you can tell AM from PM) and the seconds will reset to :00 when you save. The clock keeps time during power outages and compensates for temperature effects. |
| **Date** | `_2 _4 _0`<br/>(for&nbsp;Sun&nbsp;2/4) | You can choose the date format in the options menu (2). Setting is done in three stages: first year, then month, then date.<br/>Weekdays are: 0=Sun, 1=Mon, 2=Tue, 3=Wed, 4=Thu, 5=Fri, 6=Sat |
| **Alarm** | `_7 00 1_` | Shows alarm time (always in 24hr format) and on/off status on 5th tube (1=on, 0=off) and by display brightness (bright=on, dim=off). Use **Adjust** to switch on/off. Hold **Select** to set time (same way as **Time**). When alarm sounds, press any button to snooze, or hold for 1sec (followed by a short beep) to silence the alarm for the day. Options menu lets you restrict the alarm to your workweek or weekend only. In a power outage, the alarm will remain set, but it will not sound if power is disconnected at alarm time. |
| **Timer** | `__ __ _0` | A countdown timer, in hours, minutes, and seconds; or `0` when stopped. Can be set to the minute, up to 18 hours. Begins running as soon as you set it, and will continue to run in the background if you change to a different function. To cancel while running, hold **Select**. When timer runs out, press **Select** to silence. If power is lost, the timer will reset to `0`. Can be configured to work as an interval timer in the options menu (10). |
| **Day counter** | `_1 23 __` | Shows the number of days until/since a date you specify. Set the same way as **Date.** |
| **Temperature** | `__ 38 25` | Shows the temperature of the onboard DS3231 chip (e.g. 38.25°C – I think). May not be very useful as it tends to read higher than ambient temperature and its tolerance is low. Negative temperatures indicated with leading zeroes. |
| **Tube tester** | `88 88 88` | (Disabled by default.) Cycles through all the digits on all the tubes. |

### LEDs and Relay

Later UNDBs (v8 with mods, or v9+) are equipped with controllable LEDs, as well as a relay, which can be enabled in the [hardware configuration](#hardware-configuration) in **switch mode** (such as for a radio) or **pulse mode** (such as for a bell striker). If you have one of these UNDBs:

* The LED behavior is configurable in option 7, and can be set to switch on and off with the relay if enabled (great for a radio!)
* The alarm, timer, and strike signals can be configured to use either the beeper or the relay if enabled (options 11, 21, and 31 – strike can only use the relay in pulse mode).
* With the relay in switch mode:
  * **Alt** will switch it on and off at any time (except in options menu). (Otherwise, **Alt** does nothing.)
  * If the alarm is set to use the relay, it will switch on the relay at alarm time, and switch off two hours later. If **Alt** is used to switch it off, the alarm will be silenced for the day (skipping snooze).
  * If the timer is set to use the relay, it will switch on while the timer is running, and switch off when it runs out, like a clock radio's sleep function. If **Alt** is used to switch it off, the timer will be cancelled. (The interval timer option cannot be used in this case.)

### Options Menu

* To access this, hold **Select** for 3 seconds until you see a single `1` on the hour tubes. This indicates option number 1.
* Use **Adjust** to go to the option number you want to set (see table below); press **Select** to open it for setting (display will flash); use **Adjust** to set; and **Select** to save.
* When all done, hold **Select** to exit the options menu.

|  | Option | Settings |
| --- | --- | --- |
|  | **General** |  |
| 1 | Time format | 1 = 12-hour<br/>2 = 24-hour<br/>(time-of-day display only; setting times is always done in 24h) |
| 2 | Date format | 1 = month/date/weekday<br/>2 = date/month/weekday<br/>3 = month/date/year<br/>4 = date/month/year<br/>5 = year/month/date<br/>Note that four-tube clocks will display only the first two values in each of these options. |
| 3 | Display date during time? | 0 = never<br/>1 = date instead of seconds<br/>2 = full date (as above) every minute at :30 seconds<br/>3 = same as 2, but scrolls in and out |
| 4 | Leading zero in hour, date, and month? | 0 = no<br/>1 = yes |
| 5 | Digit fade | 0–20 (in hundredths of a second) |
| 6 | Auto DST | Add 1h for daylight saving time between these dates (at 2am):<br/>0 = off<br/>1 = second Sunday in March to first Sunday in November (US/CA)<br/>2 = last Sunday in March to last Sunday in October (UK/EU)<br/>3 = first Sunday in April to last Sunday in October (MX)<br/>4 = last Sunday in September to first Sunday in April (NZ)<br/>5 = first Sunday in October to first Sunday in April (AU)<br/>6 = third Sunday in October to third Sunday in February (BZ) |
| 7 | LED behavior | 0 = always off<br/>1 = always on<br/>2 = on, but follow night/away modes if enabled<br/>3 = off, but on when alarm/timer sounds</br>4 = off, but on with switched relay (if equipped)<br/>(Clocks with LED control only, UNDB v8+) |
| 8 | Anti-cathode poisoning | Briefly cycles all digits to prevent [cathode poisoning](http://www.tube-tester.com/sites/nixie/different/cathode%20poisoning/cathode-poisoning.htm)<br/>0 = once a day, either at midnight or at night mode start time (if enabled)<br/>1 = at the top of every hour<br/>2 = at the top of every minute<br/>(Will not trigger during night/away modes) |
| 9 | Temperature format | 0 = Celsius<br/>1 = Fahrenheit<br/>(Clocks with temperature function enabled only) |
|  | **Alarm** |  |
| 10 | Alarm days | 0 = every day<br/>1 = work week only (per settings below)<br/>2 = weekend only |
| 11 | Alarm signal | 0 = beeper<br/>1 = relay (if in switch mode, will stay on for 2 hours)<br/>(Clocks with both beeper and relay only) |
| 12 | Alarm beeper pitch | [Note number on a piano keyboard](https://en.wikipedia.org/wiki/Piano_key_frequencies), from 49 (A4) to 88 (C8). Some are louder than others!<br/>(Clocks with beeper only) |
| 13 | Alarm snooze | 0–60 minutes. 0 disables snooze. |
|  | **Timer** |  |
| 20 | Timer interval mode | 0 = count down and stop<br/>1 = count down and restart (interval mode)<br/>(Clocks with beeper and/or pulse relay only) |
| 21 | Timer signal | 0 = beeper<br/>1 = relay (if in switch mode, will stay on until timer runs down)</br>(Clocks with both beeper and relay only) |
| 22 | Timer beeper pitch | Set the same way as the alarm pitch, above<br/>(Clocks with beeper only) |
|  | **Strike** |  |
| 30 | Strike | Make noise on the hour:<br/>0 = off<br/>1 = single beep<br/>2 = pips<br/>3 = strike the hour (1 to 12)<br/>4 = ship's bell (hour and half hour)<br/>Will not sound during night/away modes (except when off starts at top of hour)<br/>(Clocks with beeper or pulse relay only) |
| 31 | Strike signal | 0 = beeper<br/>1 = relay<br/>(Clocks with both beeper and pulse relay only) |
| 32 | Strike beeper pitch | Set the same way as the alarm signal pitch, above. If using the pips, 63 (987 Hz) is closest to the real BBC pips frequency (1000 Hz).<br/>(Clocks with beeper only) |
|  | **Night mode and away mode** |  |
| 40 | Night mode | To save tube life and/or preserve your sleep, dim or shut off tubes nightly when you're not around or sleeping.<br/>0 = none (tubes fully on at night)<br/>1 = dim tubes at night<br/>2 = shut off tubes at night<br/>When off, you can press **Select** to illuminate the tubes briefly. |
| 41 | Night starts at | Time of day. |
| 42 | Night ends at | Time of day. Set to 0:00 to use the alarm time. |
| 43 | Away mode | To further save tube life, shut off tubes during the day when you're not around.<br/>0 = none (tubes fully on during the day)<br/>1 = clock at work (shut off all day on weekends)<br/>2 = clock at home (shut off during work hours)<br/>When off, you can press **Select** to illuminuate the tubes briefly. |
| 44 | First day of work week | 0–6 (Sunday–Saturday) |
| 45 | Last day of work week | 0–6 (Sunday–Saturday) |
| 46 | Work starts at | Time of day. |
| 47 | Work ends at | Time of day. |

To reset the options menu settings to "factory" defaults, hold **Select** while connecting the clock to power.

## Hardware Configuration

A number of hardware-related settings are specified in config files, one of which is included at the top of the sketch (so you can easily maintain multiple clocks with different hardware). These settings include:

* **How many tubes** in the display module. Default is 6; small display adjustments are made for 4-tube clocks.
* **Which functions** are enabled. Default is all but temperature and tube tester.
* **Which input pins** are associated with the Select and Adjust controls.
* **What type of Adjust controls** are equipped: pushbuttons (default) or rotary encoder (unimplemented).
* **What type of signal outputs** are equipped: a piezo beeper (default) and/or a relay.
  * **Signal duration** (default 3min) and **piezo pulse duration** (default 500ms)
  * If relay is equipped, **relay mode**:
    * In switched mode (default), the relay will be switched to control an appliance like a radio or light fixture. If used with timer, it will switch on while timer is running (like a "sleep" function). If used with alarm, it will switch on when alarm trips; specify **relay switch duration** (default 2 hours).
    * In pulse mode, the relay will be pulsed, like the beeper is, to control an intermittent signaling device like a solenoid or indicator lamp; specify **relay pulse duration** (default 200ms).
* **Soft alarm switch** enabled: default is yes; it is switched with Adjust while viewing the alarm time. Change to no if the signal output/appliance has its own switch on this relay circuit; the clock's alarm will be permanently on.
* **Soft power switch** enabled (switched relay only): default is yes; appliance can be switched manually with Adjust while viewing the time of day. Change to no if the appliance has its own power switch (independent of this relay circuit) or does not need to be manually switched.
* **Various other durations** for things like scrolling speed, set mode timeouts, short and long button holds, "hold to set faster" thresholds, etc.

You can also set the **defaults for the options menu** (in main code, currently) to better suit the clock's intended use.

**To compile the edited sketch:** You will need to add the [ooPinChangeInt](https://code.google.com/archive/p/oopinchangeint/downloads) and [NorthernWidget DS3231](https://github.com/NorthernWidget/DS3231) libraries to your Arduino IDE.

**To upload the sketch to the UNDB:** if it doesn't appear in the IDE's Ports menu (as a USB port), your UNDB may be equipped with an Arduino clone that requires [drivers for the CH340 chipset](https://sparks.gogo.co.nz/ch340.html).