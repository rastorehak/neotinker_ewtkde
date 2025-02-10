/*

MIT License

Copyright (c) 2025 Rastislav Rehak

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/

/* This software uses library FastLED https://github.com/FastLED/FastLED */
#include <FastLED.h>

#define NUM_STRIPS 6
#define NUM_LEDS_PER_STRIP 3

#define TYPE_SWITCH 0
#define TYPE_RAMP 1

#include <Servo.h>

CRGB leds[NUM_STRIPS][NUM_LEDS_PER_STRIP];

int buttons[NUM_STRIPS] = { A5, A2, A4, A3, A0, A1 };
int servos[NUM_STRIPS] = { 12, 10, 8, 6, 4, 2 };
 
boolean oldValues[NUM_STRIPS] = { HIGH, HIGH, HIGH, HIGH, HIGH, HIGH };

byte types[NUM_STRIPS] = { TYPE_SWITCH, TYPE_SWITCH, TYPE_SWITCH, TYPE_SWITCH, TYPE_RAMP, TYPE_RAMP };

long redTimer;

Servo sa[NUM_STRIPS];
long timers[NUM_STRIPS];

boolean phase = false;

void setup() {
  Serial.begin(115200);

  delay(100);
  FastLED.addLeds<NEOPIXEL, 13>(leds[0], NUM_LEDS_PER_STRIP);
  FastLED.addLeds<NEOPIXEL, 11>(leds[1], NUM_LEDS_PER_STRIP);
  FastLED.addLeds<NEOPIXEL, 9>(leds[2], NUM_LEDS_PER_STRIP);
  FastLED.addLeds<NEOPIXEL, 7>(leds[3], NUM_LEDS_PER_STRIP);
  FastLED.addLeds<NEOPIXEL, 5>(leds[4], NUM_LEDS_PER_STRIP);
  FastLED.addLeds<NEOPIXEL, 3>(leds[5], NUM_LEDS_PER_STRIP);

  for (int i = 0; i < NUM_STRIPS; i++) {
    pinMode(buttons[i], INPUT_PULLUP);
    pinMode(servos[i], OUTPUT);
    timers[i] = 0;
  }
  redTimer = millis();
}


void loop() {
  boolean change = readButtons();
  
  boolean newPhase = false;
  if (millis() - redTimer < 500) {    
      newPhase = true;    
  }
  if (millis() - redTimer > 1000) {
    redTimer = millis();
  }

   if( phase!=newPhase){
    change = true;
    phase=newPhase;
   }

  long thrhld = millis() - 1500;
  for (int i = 0; i < NUM_STRIPS; i++) {
    if (timers[i] > 0 && timers[i] < thrhld) {
      sa[i].detach();
      timers[i] = 0;
    }
  }
  if (change) {
    for (int i = 0; i < NUM_STRIPS; i++) {
      if (types[i] == TYPE_SWITCH) {
        if (oldValues[i]) {
          leds[i][0] = CRGB::Gray;
          leds[i][1] = CRGB::Black;
        } else {
          leds[i][0] = CRGB::Black;
          leds[i][1] = CRGB::Gray;
        }
        leds[i][2] = CRGB::Black;
      } else {
        if (oldValues[i]) {
          leds[i][0] = CRGB::Gray;
          leds[i][1] = CRGB::Black;
          leds[i][2] = CRGB::Black;
        } else {
          leds[i][0] = CRGB::Black;
          leds[i][1] = phase ? CRGB::Red : CRGB::Black;
          leds[i][2] = phase ? CRGB::Black : CRGB::Red;
        }
      }
    }

    FastLED.show();
  }

  delay(50);
}




boolean readButtons() {
  boolean changed = false;
  for (int i = 0; i < NUM_STRIPS; i++) {
    boolean v = digitalRead(buttons[i]);
    if (oldValues[i] != v) {
      if (oldValues[i] == HIGH) {
        buttonDown(i);
      } else {
        buttonUp(i);
      }
      changed = true;
    }
    oldValues[i] = v;
  }
  return changed;
}




void buttonDown(int i) {

  sa[i].attach(servos[i]);
  if (types[i] == TYPE_SWITCH) {
    sa[i].write(0);
  } else {
    sa[i].write(135);
  }
  timers[i] = millis();

  Serial.print("D ");
  Serial.println(i);
}

void buttonUp(int i) {
  sa[i].attach(servos[i]);
  if (types[i] == TYPE_SWITCH) {
    sa[i].write(180);
  } else {
    sa[i].write(45);
  }



  timers[i] = millis();
  Serial.print("U ");
  Serial.println(i);
}
