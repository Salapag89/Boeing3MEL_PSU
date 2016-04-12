#include <Write.h>
#include <Servo.h>

// Address for the GYRO, uncomment the one you want, comment the one you don't
byte GYRO = B110100;    //PSU
//byte GYRO = B110100;    //MEL

int BAUD = 9600;   //Bitrate for gyro, need to check the datasheet to find the correct rate

enum mission {
  landing,
  landed,
  flying
  }

Servo servo[4];
byte switchState = B0000;
unsigned long TIME = 0;
long ELAPSED = 3000;

void setup() {

 // Init i2c
 Wire.begin();
 Serial.begin(BAUD);

 // Init mission as flying
 mission = flying;

 // Init the servos
 servo[0].attach(8);
 servo[1].attach(9);
 servo[2].attach(10);
 servo[3].attach(11);
 
 // Init the switches
 pinMode(2,INPUT);
 pinMode(3,INPUT);
 pinMode(4,INPUT);
 pinMode(5,INPUT);
}

void loop() {
  /* switchState is now hotcoded
  *  4 bit value, 1 bit for each switch
  *  Ex: 0000 = None Pressed
  *      0010 = One Pressed
  *      1001 = Two Pressed
  *      1110 = Three Pressed
  *      1111 = Four Pressed
  *  The actual value doesn't matter, just the total number of ones
  */
  switchState = B0000;
  switchState |= digitalRead(2);
  switchState |= digitalRead(3) << 1;
  switchState |= digitalRead(4) << 2;
  switchState |= digitalRead(5) << 3;
  
  if( mission == flying){
    if( ~(switchState || B0000) )
      mission = landing;
  } else if (mission == landed){
    if( switchState && B1111 ){
      mission = flying;

      // Add code to retract legs
     
     }
  }  else if (mission == landing){
    if( switchState && B1111 )
      mission = landed;

    if( switchState && B0001 ){

  }

}
