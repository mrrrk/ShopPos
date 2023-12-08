#include <Arduino.h>
#include "LedControl.h"

// pins (and any other constants)

const int buzzerPin = 5;

const int trigPin = 5;
const int echoPin = 6;

const int ledLoadCsPin = 10;
const int ledClkPin = 11;
const int ledDataInPin = 12;
const int ledNumDevices = 1;

// global variables

long durationMicroSecs;
long lastBeepAt;
float distanceCm;
float prevDistanceCm;
LedControl ledControl = LedControl(ledDataInPin, ledClkPin, ledLoadCsPin, ledNumDevices);

// fn declarations
void detectScan(long duration);
void showLedNumber(int number);

//
// -- run once at start
//

void setup() {
    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);
    Serial.begin(9600); // Starts the serial communication

    Serial.println("Starting!");
}

//
// -- loop forever...
//

void loop() {

    // send pulse to trigger then get time for return pulse
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    // timeout after 0.9 millsecs (about 150mm) - ignores far away and improves perf.
    // (returns zero if timed out)
    durationMicroSecs = pulseIn(echoPin, HIGH, 900);

    if(durationMicroSecs != 0) {
        detectScan(durationMicroSecs);
    }
}

//
// -- our own stuff...
//

void detectScan(long duration) {

    // calculate distance (Vs = 340 m/s, divide by 2 because time is for there *and* back)
    distanceCm = (float)durationMicroSecs * (340.0 / 2.0 / 10000.0);

    Serial.print("Duration: ");
    Serial.print(durationMicroSecs);
    Serial.print("ms | Distance: ");
    Serial.print(distanceCm);
    Serial.println("cm");

    long sinceLastBeep = lastBeepAt == 0 ? 9999999 : millis() - lastBeepAt;

    // only if close and moving towards sensor
    if(prevDistanceCm > distanceCm && distanceCm <= 3) {
        //Serial.print(sinceLastBeep);
        //Serial.println("  - maybe beep?");

        // don't beep if last beep was less than a second ago
        if(sinceLastBeep > 1000) {
            Serial.print(sinceLastBeep);
            Serial.println("  @@@ BEEP @@@");
            lastBeepAt = millis();
            tone(buzzerPin, 2000, 300); // ???
        }
    }

    // remember how close we were to see which direction were moving
    prevDistanceCm = distanceCm;
}


void showLedNumber(int number) {
    //void setDigit(int addr, int digit, byte value, boolean dp);
    ledControl.setDigit(0, 3, (byte)1, false);
    ledControl.setDigit(0, 2, (byte)2, true);
    ledControl.setDigit(0, 1, (byte)3, false);
    ledControl.setDigit(0, 0, (byte)4, false);
}