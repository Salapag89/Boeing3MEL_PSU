 #include <Wire.h>
#include <Servo.h>

#define UP 100
#define STOP 90
#define DOWN 87
#define MAXTIME 3000
#define BAUD 9600
#define DEFX -800
#define DEFY -80
#define DEFZ 15700
#define TOL 800
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
unsigned long Time, runTime;
unsigned long servoTime[4];

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

	// Init mission as flying
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

	delay(100);

	switchState = 0;
	switchState |= digitalRead(3);	//servo[0]
	switchState |= digitalRead(4) << 1;   //servo[1]
	switchState |= digitalRead(5) << 2;	//servo[2]
	switchState |= digitalRead(6) << 3;	//servo[3]
	Serial.println(switchState, BIN);
        Serial.println(switchState & B1, BIN);

	if( mission == flying){
		if( switchState != B1111 ){
			mission = landing;
			Serial.println("Landing");

			Time = millis();
			runTime = millis();

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

			while(1){
				runTime = millis() - Time;
        
				if( servoTime[0] <= runTime ){
					servo[0].write(STOP);
					switchState&= B1110;
				}
				
				if( servoTime[1] <= runTime ){
					servo[1].write(STOP);
					switchState&= B1101;
				}

				if( servoTime[2] <= runTime ){
					servo[2].write(STOP);
					switchState&= B1011;
				}
				
				if( servoTime[3] <= runTime ){
					servo[3].write(STOP);
					switchState&= B0111;
				}

				if( ~switchState )
					break;
			}
		}
	}  else if (mission == landing){
		runTime = millis();

		if( switchState == B0000 ||  (runTime - Time) >= MAXTIME ){
			mission = landed;
			Serial.println( "Landed" );
		}

		// Check servo[0] switch
		if( !( switchState & B1 ) ){
			servo[0].write(STOP);
                        
                        Serial.println("Stop 0");
			if( !servoTime[0] )
				servoTime[0] = (millis() - Time);
    
		}
    
		// Check servo[1] switch
		if( !( switchState >> 1 & B1 ) ){
			servo[1].write(STOP);

                        Serial.println("Stop 1");
			if( !servoTime[1] )
				servoTime[1] = (millis() - Time);
    
		}

		// Check servo[2] switch
		if( !( switchState >> 2 & B1 ) ){
			servo[2].write(STOP);

                        Serial.println("Stop 2");
			if( !servoTime[2] )
				servoTime[2] = (millis() - Time);
    
		}

		// Check servo[3] switch
		if( !(switchState >> 3 & B1) ){
			servo[3].write(STOP);

                        Serial.println("Stop 3");
			if( !servoTime[3] )
				servoTime[3] = (millis() - Time);
    
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

  while(abs(abs(xVal)-abs(DEFX)) > TOL || abs(abs(yVal)-abs(DEFY)) > TOL) { 
    if(abs(abs(xVal)-abs(DEFX)) > TOL) { 
      if(xVal < DEFX) { 
        servo[0].write(UP);
        servo[2].write(DOWN);
      } else if (xVal > DEFX) {
        servo[0].write(DOWN);
        servo[2].write(UP);
      }
    } else if (abs(abs(yVal)-abs(DEFY)) > TOL) {
      if(yVal < DEFY) { 
        servo[1].write(UP);
        servo[3].write(DOWN);
      } else if (yVal > DEFY) {
        servo[1].write(DOWN);
        servo[3].write(UP);
      }
    }

    xVal; // Get the gyro X values again
    yVal; // Get the gyro Y values again
  }
}

