#include <Arduino.h>
#include "BluetoothSerial.h"

// Motor forward/backward PIN setup
const int EN12GPIO = 25;
const int IN1GPIO = 33;
const int IN2GPIO = 32;

// Motor left/right PIN setup
const int EN34GPIO = 26;
const int IN3GPIO = 27;
const int IN4GPIO = 14;

// incoming data
byte xyInput;
// FB data part
byte forthBack = 7;
// LR data part
byte leftRight = 7;

// FB PWM properties
const int freq1 = 30000;
const int pwmChannel1 = 1;
const int resolution1 = 3; // 3 bits == 8 values
int dutyCycle1 = 0;

// LR PWM
const int freq2 = 200;
const int pwmChannel2 = 2;
const int resolution2 = 3;
int dutyCycle2 = 0;

BluetoothSerial SerialBT;
String device_name = "BTCar_1";

void setup() {
  pinMode(IN1GPIO,OUTPUT);
  pinMode(IN2GPIO,OUTPUT);
  pinMode(EN12GPIO,OUTPUT);

  pinMode(IN3GPIO,OUTPUT);
  pinMode(IN4GPIO,OUTPUT);
  pinMode(EN34GPIO,OUTPUT);

  ledcSetup(pwmChannel1,freq1,resolution1);
  ledcAttachPin(EN12GPIO,pwmChannel1);

  ledcSetup(pwmChannel2,freq2,resolution2);
  ledcAttachPin(EN34GPIO,pwmChannel2);

  Serial.begin(115200);
  SerialBT.begin(device_name); //Bluetooth device name
  log_i("The device %s started, now you can pair it with bluetooth!",device_name);
}

void loop() {

  if (SerialBT.available()) {
    // read and denormalize incoming stream
    xyInput = SerialBT.read(); // one byte
    leftRight = xyInput >> 4; 
    forthBack = xyInput & byte(15); // filter the 4 LSB
    log_i("input commands : X = %d , Y = %d",forthBack,leftRight);

    // translate into direction and PWM signals
    if (forthBack == 7) { // idle
      digitalWrite(IN1GPIO,LOW);
      digitalWrite(IN2GPIO,LOW);
      ledcWrite(pwmChannel1,0);
    } else {
      dutyCycle1 = (abs(2*forthBack-15)-1)/2; // map [0-7] and [8-15] values in [0,7] range.
      if (forthBack < 7) {
        log_i("Go backward : %d speed",dutyCycle1);
        digitalWrite(IN1GPIO,HIGH);
        digitalWrite(IN2GPIO,LOW);
        ledcWrite(pwmChannel1,dutyCycle1+1);
        //analogWrite(EN12GPIO,map(dutyCycle,0,7,0,255)); // for arduinos...
      } else {
        log_i("Go forward : %d speed",dutyCycle1);
        digitalWrite(IN1GPIO,LOW);
        digitalWrite(IN2GPIO,HIGH);
        ledcWrite(pwmChannel1,dutyCycle1+1);
      }
    }
    if (leftRight == 7) {
      digitalWrite(IN3GPIO,LOW);
      digitalWrite(IN4GPIO,LOW);
      ledcWrite(pwmChannel2,0);
    } else {
      dutyCycle2 = (abs(2*leftRight-15)-1)/2;
      if (leftRight < 7) {
        log_i("Turn left : %d ",dutyCycle2);
        digitalWrite(IN3GPIO,HIGH);
        digitalWrite(IN4GPIO,LOW);
        ledcWrite(pwmChannel2,dutyCycle2+1);
        //analogWrite(EN34GPIO,map(dutyCycle2,0,7,0,255));
      } else {
        log_i("Turn right : %d ",dutyCycle2);
        digitalWrite(IN3GPIO,LOW);
        digitalWrite(IN4GPIO,HIGH);
        ledcWrite(pwmChannel2,dutyCycle2+1);
      }
    }
  }
}