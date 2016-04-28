 #include <Wire.h>
#include <Servo.h>

#define UP 100
// Stop - no force on legs
#define STOP 93
// Hold - downward force on legs
#define HOLD 90
#define DOWN 0
#define DOWNA 80
#define MAXTIME 300000
#define BAUD 9600
#define DEFX 0
#define DEFY 0
#define TOL 1000
#define X_H 0x3B
#define X_L 0x3C
#define Y_H 0x3D
#define Y_L 0x3E


enum status {
  landing,
  landed,
  flying
  };

status mission = flying;  

// MAKE SURE THAT:
//  -servo[0] and servo[2] are opposites on the X-axis
//    -servo[0] should be positive, servo[2] should be negative
//  -servo[1] and servo[3] are opposites on the Y-axis
//    -servo[1] should be positive, servo[3] should be negative
Servo servo[4];
byte switchState;
signed long servoTime[4];
signed long Time, runTime;

byte read(int reg)
{
    Wire.beginTransmission(0x68); // starting the transmission to sensor address 0x68
    Wire.write(reg);
    Wire.endTransmission(false);
    Wire.requestFrom(0x68, 1, false); // requestone byte of data
    byte val = Wire.read();
    Wire.endTransmission(true);
    return val;
}

void write(int reg, int data)
{
    Wire.beginTransmission(0x68); // starting the transmission to sensor address 0x68
    Wire.write(reg);
    Wire.write(data);
    Wire.endTransmission(true);
}

void setup() {

	// Init i2c
	Wire.begin();
	write(0x6B, 0);

	// Init serial to write to terminal
	Serial.begin(BAUD);

	// Init mission as landed
	mission = landed;

	// Init the servos
	servo[0].attach(8);
	servo[1].attach(9);
	servo[2].attach(10);
	servo[3].attach(11);
 
	servoTime[0] = 0;
	servoTime[1] = 0;
	servoTime[2] = 0;
	servoTime[3] = 0;

	// Init the switches
	pinMode(3,INPUT);
	pinMode(4,INPUT);
	pinMode(5,INPUT);
	pinMode(6,INPUT);
	
	digitalWrite(3, HIGH);
	digitalWrite(4, HIGH);
	digitalWrite(5, HIGH);
	digitalWrite(6, HIGH);

  	int xVal; // Read the gyro value for the X value here
  	int yVal; // Read the gyro value for the Y value here
  
  	int temp1, temp2;
  	temp1 = read(X_H);
  	temp2 = read(X_L);
  	
	temp1 *= -1;
	temp2 *= -1;

  	write(0x13,temp1);
  	write(0x14,temp2);

  	temp1 = read(Y_H);
  	temp2 = read(Y_L);
  	
	temp1 *= -1;
	temp2 *= -1;

  	write(0x15,temp1);
  	write(0x16,temp2);
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
      
        //delay(1000);
        

	
  // Pin 3 == servo[0]
  // Pin 4 == servo[1]
  // Pin 5 == servo[2]
  // Pin 6 == servo[3]
  GetSwitchState();
  
	Serial.println(switchState, BIN);

	if( mission == flying){
		if( switchState != B1111 ){
			mission = landing;
			Serial.println("Landing");

			Time = millis();
                        Serial.print("Time :");
                        Serial.println(Time,DEC);

			servoTime[0] = 0;
			servoTime[1] = 0;
			servoTime[2] = 0;
			servoTime[3] = 0;

			servo[0].write(DOWN);
			servo[1].write(DOWN);
			servo[2].write(DOWN);
			servo[3].write(DOWN);
		}
	} else if (mission == landed){
		if( switchState == B1111 ){
			mission = flying;
			Serial.println("Flying");

			switchState = B11110000;
			Time = millis();
			servo[0].write(UP);
			servo[1].write(UP);
			servo[2].write(UP);
			servo[3].write(UP);

			
		}else{
		   adjust();
                   //delay(5000);
                }
	}  else if (mission == landing){
		runTime = millis() - Time;

                //delay(1000);

		if( switchState == B0000 ||  runTime >= MAXTIME ){
			mission = landed;
			Serial.println( "Landed" );
		}

		// Check servo[0] switch
		if( !( switchState & B1 ) ){
			servo[0].write(HOLD);
                        
                        Serial.println("Stop 0");
			if( !servoTime[0] )
				servoTime[0] = 4*(millis() - Time);
    
		}
    
		// Check servo[1] switch
		if( !( switchState >> 1 & B1 ) ){
			servo[1].write(HOLD);

                        Serial.println("Stop 1");
			if( !servoTime[1] )
				servoTime[1] = 4*(millis() - Time);
    
		}

		// Check servo[2] switch
		if( !( switchState >> 2 & B1 ) ){
			servo[2].write(HOLD);

                        Serial.println("Stop 2");
			if( !servoTime[2] )
				servoTime[2] = 4*(millis() - Time);
    
		}

		// Check servo[3] switch
		if( !(switchState >> 3 & B1) ){
			servo[3].write(HOLD);

                        Serial.println("Stop 3");
			if( !servoTime[3] )
				servoTime[3] = 4*(millis() - Time);
    
		} 
	}
}

// MAKE SURE THAT:
//  -servo[0] and servo[2] are opposites on the X-axis
//    -servo[0] should be positive, servo[2] should be negative
//  -servo[1] and servo[3] are opposites on the Y-axis
//    -servo[1] should be positive, servo[3] should be negative
void adjust() {
  int xVal; // Read the gyro value for the X value here
  int yVal; // Read the gyro value for the Y value here
  
  int temp1, temp2;
  temp1 = read(X_H);
  temp2 = read(X_L);
  xVal = (temp1 << 8) | (temp2 & 0xff);
  
  temp1 = read(Y_H);
  temp2 = read(Y_L);
  yVal = (temp1 << 8) | (temp2 & 0xff);

  signed long servoStartTime[4] = {0,0,0,0};
  bool servoUpDown[4];

  GetSwitchState();

  // I think this should continue until switchState == B0000 (i.e. none of the switches are pressed) and then as soon as it exits, 
  // all of the legs should retract completely, as it should be flying at that point. Call loop(). Sound right?
  while(switchState != B1111) {
   
        Serial.print("Servo[0] time: ");
        Serial.println(servoTime[0]);
        Serial.print("Servo[1] time: ");
        Serial.println(servoTime[1]);
        Serial.print("Servo[2] time: ");
        Serial.println(servoTime[2]);
        Serial.print("Servo[3] time: ");
        Serial.println(servoTime[3]);
    //delay(3000);
        
    if(abs(abs(xVal)-abs(DEFX)) > TOL) { 
      if(xVal < DEFX) { 
        if( servoStartTime[0] )
          UpdateServoTime(servoUpDown[0],0, servoStartTime[0]);
        
        if( servoTime[0] > 0 ) {
          Serial.println("Servo[0] going up");
          servo[0].write(UP);
          servoStartTime[0] = millis();
          servoUpDown[0] = 1;
        } else{
          Serial.println("Servo[0] stopping.");
          servo[0].write(HOLD);
	  servoStartTime[0] = 0;
	}
        
        if( servoStartTime[2] )
          UpdateServoTime(servoUpDown[2],2, servoStartTime[2]);
        
        if( servoTime[2] < 2*MAXTIME){
          Serial.println("Servo[2] going down");
          servo[2].write(DOWNA);
          servoStartTime[2] = millis();
          servoUpDown[2] = 0;
        } else{
          Serial.println("Servo[2] stopping.");
          servo[2].write(HOLD);
	  servoStartTime[2] = 0;
	}
      } else if (xVal > DEFX) {
        
        if( servoStartTime[0] )
          UpdateServoTime(servoUpDown[0],0, servoStartTime[0]);
        
        if(servoTime[0] < 2*MAXTIME) {
          Serial.println("Servo[0] going down");
          servo[0].write(DOWNA);
          servoStartTime[0] = millis();
          servoUpDown[0] = 0;
        } else {
          Serial.println("Servo[0] stopping.");
          servo[0].write(HOLD);
          servoStartTime[0] = 0;
	}

        if( servoStartTime[2] )
          UpdateServoTime(servoUpDown[2],2, servoStartTime[2]);
        
        if(servoTime[2] > 0){
          Serial.println("Servo[2] going up");
          servo[2].write(UP);
          servoStartTime[2] = millis();
          servoUpDown[2] = 1;
        } else {
          Serial.println("Servo[2] stopping.");
          servo[2].write(HOLD);
          servoStartTime[2] = 0;
	}
      }
    } else {
      Serial.println("Servo[0] and servo[2] stopping");
      servo[0].write(HOLD);
      servo[2].write(HOLD);
      if( servoStartTime[0] )
        UpdateServoTime(servoUpDown[0], 0, servoStartTime[0]);
      if( servoStartTime[2] )
        UpdateServoTime(servoUpDown[2], 2, servoStartTime[2]);
      servoStartTime[0] = 0;
      servoStartTime[2] = 0;
    }
    
    if (abs(abs(yVal)-abs(DEFY)) > TOL) {
      if(yVal < DEFY) { 
        
        if( servoStartTime[1] )
          UpdateServoTime(servoUpDown[1],1, servoStartTime[1]);
        
        if( servoTime[1] > 0) {
          Serial.println("Servo[1] going up");
          servo[1].write(UP);
          servoStartTime[1] = millis();
          servoUpDown[1] = 1;
        } else {
          Serial.println("Servo[1] stopping.");
          servo[1].write(HOLD);
	  servoStartTime[1] = 0;
	}

        if( servoStartTime[3] )
          UpdateServoTime(servoUpDown[3],3, servoStartTime[3]);
        
        if(servoTime[3] < 2*MAXTIME){
          Serial.println("Servo[3] going down");
          servo[3].write(DOWNA);
          servoStartTime[3] = millis();
          servoUpDown[3] = 0;
        } else {
          Serial.println("Servo[3] stopping.");
	  servo[3].write(HOLD);
          servoStartTime[3] = 0;
        }
      } else if (yVal > DEFY) {
        if( servoStartTime[1] )
          UpdateServoTime(servoUpDown[1], 1, servoStartTime[1]);
        if(servoTime[1] > 2*MAXTIME) {
          Serial.println("Servo[1] going down");
          servo[1].write(DOWNA);
          servoStartTime[1] = millis();
          servoUpDown[1] = 0;
        } else {
          Serial.println("Servo[1] stopping.");
	  servo[1].write(HOLD);
          servoStartTime[1] = 0;
        }
        if( servoStartTime[3] )
          UpdateServoTime(servoUpDown[3], 3, servoStartTime[3]);
        if(servoTime[3] > 0){
          Serial.println("Servo[3] going up");
          servo[3].write(UP);
          servoStartTime[3] = millis();
          servoUpDown[3] = 1;
        } else {
          Serial.println("Servo[3] stopping.");
	  servo[3].write(HOLD);
          servoStartTime[3] = 0;
        }
      }
    } else {
      Serial.println("Servo[1] and servo[3] stopping");
      servo[1].write(HOLD);
      servo[3].write(HOLD);
      if( servoStartTime[1] )
        UpdateServoTime(servoUpDown[1], 1, servoStartTime[1]);
      if( servoStartTime[3] )
        UpdateServoTime(servoUpDown[3], 3, servoStartTime[3]); 
      servoStartTime[1] = 0;
      servoStartTime[3] = 0;
    }
    
    temp1 = read(X_H);
    temp2 = read(X_L);
    xVal = (temp1 << 8) | (temp2 & 0xff);
  
    temp1 = read(Y_H);
    temp2 = read(Y_L);
    yVal = (temp1 << 8) | (temp2 & 0xff);
    GetSwitchState();
        
    if( switchState & B0 ){
      while( switchState & B0 ){
        servo[0].write(DOWN);
      }
      servo[0].write(HOLD);
    }
    
    if( switchState >> 1 & B0 ){
      while( switchState >> 1 & B0 ){
        servo[1].write(DOWN);
      }
      servo[1].write(HOLD);
    }
    
    if( switchState >> 2 & B0 ){
      while( switchState >> 2 & B0 ){
        servo[2].write(DOWN);
      }
      servo[2].write(HOLD);
    } 
    
    if( switchState >> 3 & B0 ){
      while( switchState >> 3 & B0 ){
        servo[3].write(DOWN);
      }
      servo[3].write(HOLD);
    }
  }
  
  servo[0].write(STOP);
  servo[1].write(STOP);
  servo[2].write(STOP);
  servo[3].write(STOP);
  //delay(5000);
}

void StopServo(int servoNum, signed long servoStartTime, bool servoUpDown){
  servo[servoNum].write(STOP);
  UpdateServoTime(servoUpDown,servoNum, servoStartTime);
}

void GetSwitchState(){
  switchState = 0;
  switchState |= digitalRead(3);  //servo[0]
  switchState |= digitalRead(4) << 1; //servo[1]
  switchState |= digitalRead(5) << 2; //servo[2]
  switchState |= digitalRead(6) << 3; //servo[3]
}

void UpdateServoTime(bool servoUpDown, int servoNum, signed long servoStartTime){
  if(servoUpDown == 1){
    servoTime[servoNum]-=millis()-servoStartTime;
  } else {
    servoTime[servoNum]+=millis()-servoStartTime;
  }
}

