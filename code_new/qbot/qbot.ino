#include <Arduino.h>
#include "qbot.h"

#define CAL_TRIGGER_PIN 12
#define LED_PIN A5
#define TIME_INTERVAL 5000

#define FORWARD 'f'
#define LEFT 'l'
#define STAND 's'
#define RIGHT 'r'
#define BACKWARD 'b'
#define GO 'g'
#define RIGHT_FRONT 'c'
#define RIGHT_BACK 'e'
#define LEFT_FRONT 'd'
#define LEFT_BACK 'h'

MiniKame robot;

volatile boolean auto_mode = true;
bool state = true;
char cmd = STAND;
boolean random_walk = true;

void setup() {
  Serial.begin(115200);


  randomSeed(analogRead(A7));

  //
  robot.init();
  delay(2000);

  //begin: triggering delay for servo calibrating
  pinMode(CAL_TRIGGER_PIN, OUTPUT);
  digitalWrite(CAL_TRIGGER_PIN, 0);
  pinMode(CAL_TRIGGER_PIN, INPUT);
  while (digitalRead(CAL_TRIGGER_PIN)) {
    analogWrite(LED_PIN, 128 * state); // on calibrating indication LED
    delay(1000);
    state = !state;
  }
  analogWrite(LED_PIN, 0); // off calibarting indication LED
  //end:
}

void loop() {
  gaits2(0);
}

void gaits2(int pattern) {
  char movements[] = {FORWARD, LEFT, GO};
  char movements2[] = {FORWARD, LEFT, FORWARD,  RIGHT};
  static unsigned long cur_time = 0, prev_time = 0;
  static char cmd = FORWARD, prev_cmd = -1;
  static int c = 0;

  cur_time = millis();
  if (cur_time - prev_time >= TIME_INTERVAL) {
    prev_time = cur_time;
    do {
      switch (pattern) {
        case 0: c = (int)random(0, sizeof(movements));
          cmd = movements[c];
          break;
        case 1:  c = c % sizeof(movements2);
          cmd = movements2[c++];
          break;
        default:
          pattern = 0;
      }
    } while (cmd == prev_cmd);
    prev_cmd = cmd; //Serial.println(cmd);
  }

  switch (cmd) {
    case GO:
      robot.pushUp(2, 5000);
      break;
    case FORWARD:
      robot.pushUp(2, 5000);
      break;
    case LEFT:
      robot.dance(1, 550);
      break;
  }
}
