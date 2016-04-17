#include <Wire.h>
#include <Servo.h>

#define UP 100
#define STOP 90
#define DOWN 0
#define MAXTIME 3000
#define BAUD 9600

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
	wait(100);
	switchState = 0;
	switchState |= digitalRead(3);	//servo[0]
	switchState |= digitalRead(4) << 1;   //servo[1]
	switchState |= digitalRead(5) << 2;	//servo[2]
	switchState |= digitalRead(6) << 3;	//servo[3]
	Serial.println(switchState, BIN);

	if( mission == flying){
		if( switchState ){
			mission = landing;

			Time = millis();
			servoTime[0] = 0;
			servoTime[1] = 0;
			servoTime[2] = 0;
			servoTime[3] = 0;
		}
	} else if (mission == landed){
		if( !switchState ){
			mission = flying;

			switchState = B00001111;
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

		if( switchState == 15 ||  (runTime - Time) >= MAXTIME ){
			mission = landed;
			Serial.println( switchState, DEC );
		}

		// Check servo[0] switch
		if( switchState & B0001 ){
			servo[0].write(STOP);

			if( !servoTime[0] )
				servoTime[0] = 4*(millis() - Time);
    
		} else
			servo[0].write(DOWN);
    
		// Check servo[1] switch
		if( switchState & B0010 ){
			servo[1].write(STOP);

			if( !servoTime[1] )
				servoTime[1] = 4*(millis() - Time);
    
		} else
			servo[1].write(DOWN);

		// Check servo[2] switch
		if( switchState & B0100 ){
			servo[2].write(STOP);

			if( !servoTime[2] )
				servoTime[2] = 4*(millis() - Time);
    
		} else
			servo[2].write(DOWN);

		// Check servo[3] switch
		if( switchState & B1000 ){
			servo[3].write(STOP);

			if( !servoTime[3] )
				servoTime[3] = 4*(millis() - Time);
    
		} else
		servo[3].write(DOWN);
	}

}
