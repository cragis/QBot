#include <EEPROM.h>
#include "qbot.h"

/*
   (servo index, pin to attach pwm)
   __________ __________ _________________
  |(3,9)_____)(1,8)      (0,2)(______(2,3)|
  |__|       |left FRONT right|        |__|
             |                |
             |                |
             |                |
   _________ |                | __________
  |(7,7)_____)(5,6)______(4,4)(______(6,5)|
  |__|                                 |__|

*/
//comment below manually setting trim in MiniKame() constructor
#define __LOAD_TRIM_FROM_EEPROM__

#define EEPROM_MAGIC  0xabcd
#define EEPROM_OFFSET 2   //eeprom starting offset to store trim[]


MiniKame::MiniKame(): reverse{0, 0, 0, 0, 0, 0, 0, 0}, trim{0, 0, 0, 0, 0, 0, 0, 0} {
  board_pins[FRONT_RIGHT_HIP] = 10; // front left inner
  board_pins[FRONT_LEFT_HIP] = 11; // front right inner
  board_pins[BACK_RIGHT_HIP] = 4; // back left inner
  board_pins[BACK_LEFT_HIP] = 5; // back right inner
  board_pins[FRONT_RIGHT_KNEE] = 6; // front left outer
  board_pins[FRONT_LEFT_KNEE] = 7; // front right outer
  board_pins[BACK_RIGHT_KNEE] = 8; // back left outer
  board_pins[BACK_LEFT_KNEE] = 9; // back right outer
}

void MiniKame::init() {
  /*
     trim[] for calibrating servo deviation,
     initial posture (home) should like below
     in symmetric
        \       /
         \_____/
         |     |
         |_____|
         /     \
        /       \
  */
  /*
    trim[FRONT_LEFT_HIP] = 0;
    trim[FRONT_RIGHT_HIP] = -8;
    trim[BACK_LEFT_HIP] = 8;
    trim[BACK_RIGHT_HIP] = 5;

    trim[FRONT_LEFT_LEG] = 2;
    trim[FRONT_RIGHT_LEG] = -6;
    trim[BACK_LEFT_LEG] = 6;
    trim[BACK_RIGHT_LEG] = 5;
  */
#ifdef __LOAD_TRIM_FROM_EEPROM__
  int val = EEPROMReadWord(0);
  if (val != EEPROM_MAGIC) {
    EEPROMWriteWord(0, EEPROM_MAGIC);
    storeTrim();
  }
#endif

  for (int i = 0; i < 8; i++) {
    oscillator[i].start();
    servo[i].attach(board_pins[i]);
#ifdef __LOAD_TRIM_FROM_EEPROM__
    int val = EEPROMReadWord(i * 2 + EEPROM_OFFSET);
    if (val >= -90 && val <= 90) {
      trim[i] = val;
    }
#endif
  }

  home();
}

void MiniKame::turnR(float steps, float T = 600) {

}

void MiniKame::turnL(float steps, float T = 600) {

}

void MiniKame::dance(float steps, float T = 600) {
  int x_amp = 20;
  int z_amp = 40;
  int ap = 30;
  int hi = 0;
  float period[] = {T, T, T, T, T, T, T, T};
  int amplitude[] = {x_amp, x_amp, z_amp, z_amp, x_amp, x_amp, z_amp, z_amp};
  int offset[] = {90 + ap, 90 - ap, 90 - hi, 90 + hi, 90 - ap, 90 + ap, 90 + hi, 90 - hi};
  int phase[] = {0, 0, 0, 270, 0, 0, 90, 180};

  execute(steps, period, amplitude, offset, phase);
}

void MiniKame::frontBack(float steps, float T = 600) {

}

void MiniKame::run(int dir, float steps, float T) {

}

void MiniKame::omniWalk(float steps, float T, bool side, float turn_factor) {
   
}

void MiniKame::moonwalkL(float steps, float T = 5000) {

}


void MiniKame::walk(int dir, float steps, float T) {
 
}

void MiniKame::upDown(float steps, float T = 5000) {

}


void MiniKame::pushUp(float steps, float T = 600) {
  int z_amp = 40;
  int x_amp = 65;
  int hi = 0;
  float period[] = {T, T, T, T, T, T, T, T};
  int amplitude[] = {0, 0, z_amp, z_amp, 0, 0, 0, 0};
  int offset[] = {90, 90, 90 - hi, 90 + hi, 90 - x_amp, 90 + x_amp, 90 + hi, 90 - hi};
  int phase[] = {0, 0, 0, 180, 0, 0, 0, 180};

  execute(steps, period, amplitude, offset, phase);
}

void MiniKame::hello() {

}



void MiniKame::jump() {

}

void MiniKame::home() {

}



void MiniKame::reverseServo(int id) {
  if (reverse[id])
    reverse[id] = 0;
  else
    reverse[id] = 1;
}


void MiniKame::setServo(int id, float target) {
  if (!reverse[id])
    servo[id].writeMicroseconds(angToUsec(target + trim[id]));
  else
    servo[id].writeMicroseconds(angToUsec(180 - (target + trim[id])));
  _servo_position[id] = target + trim[id];
}

float MiniKame::getServo(int id) {
  return _servo_position[id];
}


void MiniKame::moveServos(int time, float target[8]) {
  if (time > 10) {
    for (int i = 0; i < 8; i++)	_increment[i] = (target[i] - _servo_position[i]) / (time / 10.0);
    _final_time =  millis() + time;

    while (millis() < _final_time) {
      _partial_time = millis() + 10;
      for (int i = 0; i < 8; i++) setServo(i, _servo_position[i] + _increment[i]);
      while (millis() < _partial_time); //pause
    }
  }
  else {
    for (int i = 0; i < 8; i++) setServo(i, target[i]);
  }
  for (int i = 0; i < 8; i++) _servo_position[i] = target[i];
}

void MiniKame::execute(float steps, float period[8], int amplitude[8], int offset[8], int phase[8]) {

  for (int i = 0; i < 8; i++) {
    oscillator[i].setPeriod(period[i]);
    oscillator[i].setAmplitude(amplitude[i]);
    oscillator[i].setPhase(phase[i]);
    oscillator[i].setOffset(offset[i]);
  }

  unsigned long global_time = millis();

  for (int i = 0; i < 8; i++) oscillator[i].setTime(global_time);

  _final_time = millis() + period[0] * steps;
  while (millis() < _final_time) {
    for (int i = 0; i < 8; i++) {
      setServo(i, oscillator[i].refresh());
    }
    yield();
  }
}
void MiniKame::storeTrim() {
  for (int i = 0; i < 8; i++) {
    EEPROMWriteWord(i * 2 + EEPROM_OFFSET, trim[i]);
    delay(100);
  }

}

// load/send only trim of hip servo
void MiniKame::loadTrim() {
  //FRONT_LEFT/RIGHT_HIP
  for (int i = 0; i < 4; i++) {
    Serial.write(EEPROM.read(i + EEPROM_OFFSET));
  }
  
  //BACK_LEFT/RIGHT_HIP
  for (int i = 8; i < 12; i++) {
    Serial.write(EEPROM.read(i + EEPROM_OFFSET));
  }
}

int MiniKame::EEPROMReadWord(int p_address)
{
  byte lowByte = EEPROM.read(p_address);
  byte highByte = EEPROM.read(p_address + 1);

  return ((lowByte << 0) & 0xFF) + ((highByte << 8) & 0xFF00);
}

void MiniKame::EEPROMWriteWord(int p_address, int p_value)
{
  byte lowByte = ((p_value >> 0) & 0xFF);
  byte highByte = ((p_value >> 8) & 0xFF);

  EEPROM.write(p_address, lowByte);
  EEPROM.write(p_address + 1, highByte);
}
