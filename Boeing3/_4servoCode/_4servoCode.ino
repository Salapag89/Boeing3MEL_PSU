#include <Wire.h>
#include <Servo.h>

// Address for the GYRO, uncomment the one you want, comment the one you don't
byte GYRO = B110100;    //PSU
//byte GYRO = B110100;    //MEL

int BAUD = 9600;   //Bitrate for gyro, need to check the datasheet to find the correct rate

Servo servo1;
Servo servo2;
Servo servo3;
Servo servo4;
int switchState1 = 1;
int switchState2 = 1;
int switchState3 = 1;
int switchState4 = 1;
unsigned long currentTIME = 0;
unsigned long previousTIME = 0;
long ELAPSED = 3000;
int LANDED = 1;
int TAKEN_OFF = 0;
int EXTEND = 0;
int STOP = 90;
int RETRACT = 100;
int RETRACT_TIME = 6000;

// 0 rotates CW at full speed
// 180 rotates CCW at full speed
// 90 means no rotation 

void setup() {

 // Init i2c
 Wire.begin();
 Serial.begin(BAUD);

 // Init the servos
 servo1.attach(8);
 servo2.attach(9);
 servo3.attach(10);
 servo4.attach(11);
 // Init the switches
 pinMode(3,INPUT);
 pinMode(4,INPUT);
 pinMode(5,INPUT);
 pinMode(6,INPUT);
 pinMode(13,OUTPUT); // LED
 Serial.print("Quadcopter is awaiting take-off\n");
}

void loop() {
 if (LANDED == 1 && switchState1 == LOW && switchState2 == LOW && switchState3 == LOW && switchState4 == LOW) {
  LANDED = 0;
  TAKEN_OFF = 1; 
  Serial.print("Quadopter has taken off\n");
  Serial.print("Quadcopter has not yet landed\n");
}
if (LANDED == 0) {
  digitalWrite(13,LOW);
  switchState1 = digitalRead(3);
  switchState2 = digitalRead(4);
  switchState3 = digitalRead(5);
  switchState4 = digitalRead(6);
}
if (LANDED == 1) {
  digitalWrite(13,HIGH);
  switchState1 = digitalRead(3);
  switchState2 = digitalRead(4);
  switchState3 = digitalRead(5);
  switchState4 = digitalRead(6);
}
if (LANDED == 1 && switchState1 == HIGH && switchState2 == HIGH && switchState3 == HIGH && switchState4 == HIGH) {
  servo1.write(STOP);
  servo2.write(STOP);
  servo3.write(STOP);
  servo4.write(STOP);
} else if (LANDED == 1 && TAKEN_OFF == 1 && switchState1 == LOW && switchState2 == LOW && switchState3 == LOW && switchState4 == LOW) {
  Serial.print("Quadcopter legs are retracting\n");
  servo1.write(RETRACT);
  servo2.write(RETRACT);
  servo3.write(RETRACT);
  servo4.write(RETRACT);
  delay(RETRACT_TIME);
  LANDED = 0;
  TAKEN_OFF = 0;
  Serial.print("Quadcopter is waiting for the next land\n");
}

while(LANDED == 0 && switchState1 == LOW && switchState2 == LOW && switchState3 == LOW && switchState4 == LOW){
  switchState1 = digitalRead(3);
  switchState2 = digitalRead(4);
  switchState3 = digitalRead(5);
  switchState4 = digitalRead(6);
  servo1.write(STOP);
  servo2.write(STOP);
  servo3.write(STOP);
  servo4.write(STOP);
}
//all switches pressed
if (LANDED == 0 && switchState1 == HIGH && switchState2 == HIGH && switchState3 == HIGH && switchState4 == HIGH){
  servo1.write(STOP);
  servo2.write(STOP);
  servo3.write(STOP);
  servo4.write(STOP); //one switch pressed from here
  currentTIME = millis();
} if (currentTIME - previousTIME > ELAPSED){
  LANDED = 1;
  TAKEN_OFF = 1;
  previousTIME = currentTIME;
  Serial.print("Quadcopter has landed\n");
} else if(LANDED == 0 && switchState1 == HIGH && switchState2 == LOW && switchState3 == LOW && switchState4 == LOW){
  servo1.write(STOP);
  servo2.write(EXTEND);
  servo3.write(EXTEND);
  servo4.write(EXTEND);
} else if(LANDED == 0 && switchState1 == LOW && switchState2 == HIGH && switchState3 == LOW && switchState4 == LOW){
  servo1.write(EXTEND);
  servo2.write(STOP);
  servo3.write(EXTEND);
  servo4.write(EXTEND); 
} else if(LANDED == 0 && switchState1 == LOW && switchState2 == LOW && switchState3 == HIGH && switchState4 == LOW){
  servo1.write(EXTEND);
  servo2.write(EXTEND);
  servo3.write(STOP);
  servo4.write(EXTEND);
} else if(LANDED == 0 && switchState1 == LOW && switchState2 == LOW && switchState3 == LOW && switchState4 == HIGH){
  servo1.write(EXTEND);
  servo2.write(EXTEND);
  servo3.write(EXTEND);
  servo4.write(STOP); //two switches are pressed from here
} else if(LANDED == 0 && switchState1 == LOW && switchState2 == LOW && switchState3 == HIGH && switchState4 == HIGH){
  servo1.write(EXTEND);
  servo2.write(EXTEND);
  servo3.write(STOP);
  servo4.write(STOP);
} else if(LANDED == 0 && switchState1 == LOW && switchState2 == HIGH && switchState3 == LOW && switchState4 == HIGH){
  servo1.write(0);
  servo2.write(90);
  servo3.write(0);
  servo4.write(90);
} else if(LANDED == 0 && switchState1 == LOW && switchState2 == HIGH && switchState3 == HIGH && switchState4 == LOW){
  servo1.write(EXTEND);
  servo2.write(STOP);
  servo3.write(STOP);
  servo4.write(EXTEND);
} else if(LANDED == 0 && switchState1 == HIGH && switchState2 == LOW && switchState3 == LOW && switchState4 == HIGH){
  servo1.write(STOP);
  servo2.write(EXTEND);
  servo3.write(EXTEND);
  servo4.write(STOP);
} else if(LANDED == 0 && switchState1 == HIGH && switchState2 == LOW && switchState3 == HIGH && switchState4 == LOW){
  servo1.write(STOP);
  servo2.write(EXTEND);
  servo3.write(STOP);
  servo4.write(EXTEND);
} else if(LANDED == 0 && switchState1 == HIGH && switchState2 == HIGH && switchState3 == LOW && switchState4 == LOW){
  servo1.write(90);
  servo2.write(90);
  servo3.write(0);
  servo4.write(0);//3swtiches pressed
} else if(LANDED == 0 && switchState1 == LOW && switchState2 == HIGH && switchState3 == HIGH && switchState4 == HIGH){
  servo1.write(EXTEND);
  servo2.write(STOP);
  servo3.write(STOP);
  servo4.write(STOP);
} else if(LANDED == 0 && switchState1 == HIGH && switchState2 == LOW && switchState3 == HIGH && switchState4 == HIGH){
  servo1.write(STOP);
  servo2.write(EXTEND);
  servo3.write(STOP);
  servo4.write(STOP);
} else if(LANDED == 0 && switchState1 == HIGH && switchState2 == HIGH && switchState3 == LOW && switchState4 == HIGH){
  servo1.write(STOP);
  servo2.write(STOP);
  servo3.write(EXTEND);
  servo4.write(STOP);
} else if(LANDED == 0 && switchState1 == HIGH && switchState2 == HIGH && switchState3 == HIGH && switchState4 == LOW){
  servo1.write(STOP);
  servo2.write(STOP);
  servo3.write(STOP);
  servo4.write(EXTEND);
}
}
