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
int switchState1 = 0;
int switchState2 = 0;
int switchState3 = 0;
int switchState4 = 0;
int OFF = 0;
unsigned long TIME = 0;
long ELAPSED = 3000;

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
 pinMode(2,INPUT);
 pinMode(3,INPUT);
 pinMode(4,INPUT);
 pinMode(5,INPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  if(OFF == 0){
    switchState1 = digitalRead(2);
    switchState2 = digitalRead(3);
    switchState3 = digitalRead(4);
    switchState4 = digitalRead(5);
  } else if(OFF == 1){
    switchState1 = 1;
    switchState2 = 1;
    switchState3 = 1;
    switchState4 = 1;
  }

  while(switchState1 == LOW && switchState2 == LOW && switchState3 == LOW && switchState4 == LOW){
    switchState1 = digitalRead(2);
    switchState2 = digitalRead(3);
    switchState3 = digitalRead(4);
    switchState4 = digitalRead(5);
    servo1.write(90);
    servo2.write(90);
    servo3.write(90);
    servo4.write(90);
  }

  //all switches pressed
  if (switchState1 == HIGH && switchState2 == HIGH && switchState3 == HIGH && switchState4 == HIGH){
    servo1.write(90);
    servo2.write(90);
    servo3.write(90);
    servo4.write(87); //one switch pressed from here
    TIME = millis();
  }if (TIME > ELAPSED){
    OFF = 1;
  } else if(switchState1 == HIGH && switchState2 == LOW && switchState3 == LOW && switchState4 == LOW){
    servo1.write(90);
    servo2.write(0);
    servo3.write(0);
    servo4.write(0);
  } else if(switchState1 == LOW && switchState2 == HIGH && switchState3 == LOW && switchState4 == LOW){
    servo1.write(0);
    servo2.write(90);
    servo3.write(0);
    servo4.write(0); 
  } else if(switchState1 == LOW && switchState2 == LOW && switchState3 == HIGH && switchState4 == LOW){
    servo1.write(0);
    servo2.write(0);
    servo3.write(90);
    servo4.write(0);
  } else if(switchState1 == LOW && switchState2 == LOW && switchState3 == LOW && switchState4 == HIGH){
    servo1.write(0);
    servo2.write(0);
    servo3.write(0);
    servo4.write(90); 
  //two switches are pressed from here
  } else if(switchState1 == LOW && switchState2 == LOW && switchState3 == HIGH && switchState4 == HIGH){
    servo1.write(0);
    servo2.write(0);
    servo3.write(90);
    servo4.write(90);
  } else if(switchState1 == LOW && switchState2 == HIGH && switchState3 == LOW && switchState4 == HIGH){
    servo1.write(0);
    servo2.write(90);
    servo3.write(0);
    servo4.write(90);
  } else if(switchState1 == LOW && switchState2 == HIGH && switchState3 == HIGH && switchState4 == LOW){
    servo1.write(0);
    servo2.write(90);
    servo3.write(90);
    servo4.write(0);
  } else if(switchState1 == HIGH && switchState2 == LOW && switchState3 == LOW && switchState4 == HIGH){
    servo1.write(90);
    servo2.write(0);
    servo3.write(0);
    servo4.write(90);
  } else if(switchState1 == HIGH && switchState2 == LOW && switchState3 == HIGH && switchState4 == LOW){
    servo1.write(90);
    servo2.write(0);
    servo3.write(90);
    servo4.write(0);
  } else if(switchState1 == HIGH && switchState2 == HIGH && switchState3 == LOW && switchState4 == LOW){
    servo1.write(90);
    servo2.write(90);
    servo3.write(0);
    servo4.write(0);//3swtiches pressed
  } else if(switchState1 == LOW && switchState2 == HIGH && switchState3 == HIGH && switchState4 == HIGH){
    servo1.write(0);
    servo2.write(90);
    servo3.write(90);
    servo4.write(90);
  } else if(switchState1 == HIGH && switchState2 == LOW && switchState3 == HIGH && switchState4 == HIGH){
    servo1.write(90);
    servo2.write(0);
    servo3.write(90);
    servo4.write(90);
  } else if(switchState1 == HIGH && switchState2 == HIGH && switchState3 == LOW && switchState4 == HIGH){
    servo1.write(90);
    servo2.write(90);
    servo3.write(0);
    servo4.write(90);
  } else if(switchState1 == HIGH && switchState2 == HIGH && switchState3 == HIGH && switchState4 == LOW){
    servo1.write(90);
    servo2.write(90);
    servo3.write(90);
    servo4.write(0);
  }
}
