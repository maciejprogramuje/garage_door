#include <virtuabotixRTC.h>

int doorSwitch = 4;
int doorRelay = 5;

virtuabotixRTC rtClock(A0, 2, 3);

bool isDoorOpen;
int realayRunTime[] = {99, 99, 99};
int realayStopTime[] = {99, 99, 99};
bool isRelayRunning;
bool isRelayStopping;

// ******************************************* SETUP *******************************************
// *********************************************************************************************
// if door is open  -> doorSwitch == LOW
// if door is close -> doorSwitch == HIGH
// *********************************************************************************************
boolean showOnMonitor = true;
int pauseDuration = 30;
// *********************************************************************************************

void setup() {
  if (showOnMonitor) {
    Serial.begin (9600);
  }

  //                      seconds, minutes, hours, day of the week, day of the month, month, year
  //rtClock.setDS1302Time(      0,      32,    12,               1,               21,     2, 2022);

  pinMode(doorSwitch, INPUT);

  pinMode(doorRelay, OUTPUT);
  digitalWrite(doorRelay, HIGH);

  isDoorOpen = false;
  isRelayRunning = false;
  isRelayStopping = false;
}

void loop() {
  // This allows for the update of variables for time or accessing the individual elements.
  rtClock.updateTime();

  int tHours = rtClock.hours;
  int tMinutes = rtClock.minutes;
  int tSeconds = rtClock.seconds;

  Serial.print(String(tHours) + ":" + String(tMinutes) + "/" + String(tSeconds));

  if (digitalRead(doorSwitch) == LOW) {
    if (!isDoorOpen) {
      isDoorOpen = true;

      setTimeToRunRelay(tHours, tMinutes, tSeconds);
      setTimeToStopRelay(tHours, tMinutes, tSeconds);
    }

    if (isTimeToRunRelay(tHours, tMinutes, tSeconds)) {
      digitalWrite(doorRelay, LOW);
      isDoorOpen = false;
    }

    if (isTimeToStopRelay(tHours, tMinutes, tSeconds)) {
      digitalWrite(doorRelay, HIGH);
    }

    if (isTimeAfterRelayWork(tHours, tMinutes, tSeconds)) {
      resetRelayFlags();
    }
  }

  if (digitalRead(doorSwitch) == HIGH) {
    isDoorOpen = false;
    isRelayRunning = false;
    isRelayStopping = false;
  }

  Serial.print("realayRunTime=" + String(realayRunTime[0]) + ":" + String(realayRunTime[1]) + "/" + String(realayRunTime[2]));
  Serial.print("realayStopTime=" + String(realayStopTime[0]) + ":" + String(realayStopTime[1]) + "/" + String(realayStopTime[2]));

  //delay(50);
}

bool isTimeToRunRelay(int h, int m, int s) {
  if (!isRelayRunning && h == realayRunTime[0] && m == realayRunTime[1] && s == realayRunTime[2]) {
    isRelayRunning = true;
    return true;
  }
  return false;
}

bool isTimeToStopRelay(int h, int m, int s) {
  if (!isRelayStopping && h == realayStopTime[0] && m == realayStopTime[1] && s == realayStopTime[2]) {
    isRelayStopping = true;
    return true;
  }
  return false;
}

bool isTimeAfterRelayWork(int h, int m, int s) {
  if (isRelayRunning && isRelayStopping && h == realayStopTime[0] && m == realayStopTime[1] && s > realayStopTime[2]) {
    return true;
  }
  return false;
}

void setTimeToRunRelay(int h, int m, int s) {
  realayRunTime[0] = h;
  realayRunTime[1] = m + pauseDuration;
  realayRunTime[2] = s;

  fixTimeOvercome(&realayRunTime[0], &realayRunTime[0], &realayRunTime[0]);
}

void setTimeToStopRelay(int h, int m, int s) {
  realayStopTime[0] = h;
  realayStopTime[1] = realayRunTime[1];
  realayStopTime[2] = s + 2;

  fixTimeOvercome(&realayStopTime[0], &realayStopTime[0], &realayStopTime[0]);
}

void fixTimeOvercome(int *hh, int *mm, int *ss) {
  if (*ss >= 60) {
    *mm = *mm + 1;
    *ss = *ss - 60;
  }
  if (*mm >= 60) {
    *hh = *hh + 1;
    *mm = *mm - 60;
  }
  if (*hh >= 24) {
    *hh = 0;
  }
}

void resetRelayFlags() {
  isRelayRunning = false;
  isRelayStopping = false;
}
