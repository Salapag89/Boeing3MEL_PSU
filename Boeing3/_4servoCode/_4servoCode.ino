#include <Wire.h>
#include <Servo.h>

#define UP 100
#define STOP 90
#define DOWN 87
#define MAXTIME 3000
#define BAUD 9600
#define DEFX 500
#define DEFY 190
#define DEFZ 15700

// Address for the GYRO, uncomment the one you want, comment the one you don't
//byte GYRO = B110100;    //PSU
//byte GYRO = B110100;    //MEL


enum status {
  landing,
  landed,
  flying
  };

status mission = flying;  

Servo servo[4];
byte switchState;
unsigned long Time, runTime;
unsigned long servoTime[4];

void setup() {

	// Init i2c
	//Wire.begin();
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
				servoTime[0] = 4*(millis() - Time);
    
		}
    
		// Check servo[1] switch
		if( !( switchState >> 1 & B1 ) ){
			servo[1].write(STOP);

                        Serial.println("Stop 1");
			if( !servoTime[1] )
				servoTime[1] = 4*(millis() - Time);
    
		}

		// Check servo[2] switch
		if( !( switchState >> 2 & B1 ) ){
			servo[2].write(STOP);

                        Serial.println("Stop 2");
			if( !servoTime[2] )
				servoTime[2] = 4*(millis() - Time);
    
		}

		// Check servo[3] switch
		if( !(switchState >> 3 & B1) ){
			servo[3].write(STOP);

                        Serial.println("Stop 3");
			if( !servoTime[3] )
				servoTime[3] = 4*(millis() - Time);
    
		} 
	}

}
