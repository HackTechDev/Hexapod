/*
Hexapod

Modification : lesanglierdesardennes@gmail.com

Code :
 - https://github.com/Nekrofage/Hexapod

Liens :
 - http://letsmakerobots.com/node/32854

IDE Arduino 1.0.3 sous Debian Squeeze

Configuration IDE :
 - Type de carte : Arduino Mega (ATmega 1280)
 - Port Série : /dev/ttyUSB0
 - Vitesse Serial Monitor : 9600 Baud

Télécommande :
  - Smart SliderX S-0003

*/

// InfraRed libraries
#include <IRremote.h>
#include <IRremoteInt.h>

// set to 1 for IR test mode
#define IRtest 0

// Standard Arduino servo libarary
#include <Servo.h>

// servo center positions (typically 1500uS)
// D29 knee1, D46 Hip1, D47 knee2, D48 Hip2
// D49 knee3, D50 Hip3, D51 knee4, D24 Hip4
// D25 knee5, D26 Hip5, D27 knee6, D28 Hip6
int svc[12] = {
  1500,1550,1550,1450,
  1500,1400,1500,1550,
  1500,1500,1500,1400
};

// Servos can be an array
Servo sv[12];

// determines the direction/angle (0°-360°) that the robot will walk in
int angle;

// rotate mode: -1 = anticlockwise, +1 = clockwise, 0 = no rotation
int rotate;

// walking speed: -15 to +15
int Speed;

// size of step: exceeding 400 may cause the legs to hit each other
int Stride;

// define D10 as input for IR receiver
int RECV_PIN = 10;
IRrecv irrecv(RECV_PIN);
decode_results results;

void setup() {
	// use D11 as GND for IR receiver
  pinMode(11,OUTPUT);
  digitalWrite(11,0);

	// use D12 as Vcc for IR receiver
  pinMode(12,OUTPUT);
  digitalWrite(12,1);

  Serial.print("Setup");

  // knee 1
  sv[0].attach(29,800,2200);
  delay(40);

  // Hip  1
  sv[1].attach(46,800,2200);
  delay(40);

  // knee 2
  sv[2].attach(47,800,2200);
  delay(40);

  // Hip  2
  sv[3].attach(48,800,2200);
  delay(40);

  // knee 3
  sv[4].attach(49,800,2200);
  delay(40);

  // Hip  3
  sv[5].attach(50,800,2200);
  delay(40);

  // knee 4
  sv[6].attach(51,800,2200);
  delay(40);

  // Hip  4
  sv[7].attach(24,800,2200);
  delay(40);

  // knee 5
  sv[8].attach(25,800,2200);
  delay(40);

  // Hip  5
  sv[9].attach(26,800,2200);
  delay(40);

  // knee 6
  sv[10].attach(27,800,2200);
  delay(40);

  // Hip  6
  sv[11].attach(28,800,2200);
  delay(40);

  // initialize servos
  for(int i=0;i<12;i++) {
    sv[i].writeMicroseconds(svc[i]);
  }

  // optional - gives you time to put the robot down before it starts
  delay(3000);

  // IR test mode displays IR receiver values on serial monitor
  Serial.begin(9600);

  // Start the receiver
  irrecv.enableIRIn();
}

void loop() {
  // check for IR command
  if (irrecv.decode(&results)) {
  // change IRC comparison values to suit your TV, DVD, Stereo remote
    int IRC=results.value;

			// display value from IR receiver on serial monitor
      Serial.print("Commande IR : ");
      Serial.println(IRC);

    // Touche : OK
    // Arret
    if(IRC == 5100) {
      Serial.print("OK : Arret\n");
      Speed=0;
      angle=0;
      rotate=0;
    }

    // Touche : Haut
    // Avancer
    if(IRC == -27796) {
      Serial.print("Haut : Avancer\n");
      Speed=10;
      rotate=0;
      angle=0;
    }

    // Touche : Bas
    // Reculer
    if(IRC == 21420) {
      Serial.print("Bas : Reculer\n");
      Speed=-10;
      rotate=0;
      angle=0;
    }

    // Touche : Droite
    // Rotation horaire
    if(IRC == 13260) {
      Serial.print("Droite : Rotation horaire\n");
      Speed=10;
      rotate=1;
      angle=0;
    }

    // Touche : Gauche
    // Rotation anti-horaire
    if(IRC == -11476) {
      Serial.print("Gauche : Rotation anti-horaire\n");
      Speed=10;
      rotate=-1;
      angle=0;
    }

    // Rotation en degres

    // Touche 1
    // Rotation : 45
    if(IRC == 3570) {
      Serial.print("1 : 45\n");
      Speed=10;
      rotate=0;
      angle=45;
    }

    // Touche 2
    // Rotation : 90
    if(IRC == 28050) {
      Serial.print("2 : 90\n");
      Speed=10;
      rotate=0;
      angle=90;
    }

    // Touche 3
    // Rotation : 135
    if(IRC == 9690) {
      Serial.print("3 : 135\n");
      Speed=10;
      rotate=0;
      angle=135;
    }

    // Touche 4
    // Rotation : 225
    if(IRC == -29326) {
      Serial.print("4 : 225\n");
      Speed=10;
      rotate=0;
      angle=225;
    }

    // Touche 5
    // Rotation : 270
    if(IRC == -13006) {
      Serial.print("5 : 270\n");
      Speed=10;
      rotate=0;
      angle=270;
    }

    // Touche 6
    // Rotation : 315
    if(IRC == -25246) {
      Serial.print("6 : 315\n");
      Speed=10;
      rotate=0;
      angle=315;
    }

    // Touche 7
    if(IRC == 11730) {
      Serial.print("7 : Rien\n");

    }

    // Touche 8
    if(IRC == 5610) {
      Serial.print("8 : Rien\n");

    }

    // Touche 9
    if(IRC == 30090) {
      Serial.print("9 : Rien\n");

    }

    // Touche 0
    if(IRC == -2806) {
      Serial.print("0 : Rien\n");
    }

     // receive the next value
    irrecv.resume();
  }

  // robot does not walk in IRtest mode
  if(IRtest ==  1) {
  	return;
  }

  // keep travel angle within 0°-360°
  if (angle < 0) {
  	angle += 360;
	}

	// keep travel angle within 0°-360°
  if (angle > 359) {
  	angle -= 360;
	}

	// move legs to generate walking gait
  Walk();

  delay(15);
}


void Walk()
{

	// all legs move in a circular motion

	// return all legs to default position when stopped
  if(Speed == 0) {
  	// as Stride aproaches 0, all servos return to center position
    Stride-=25;

    // do not allow negative values, this would reverse movements
    if(Stride<0)
    	Stride=0;
  } else {
  	// this only affects the robot if it was stopped
  	// slowly increase Stride value so that servos start up smoothly
    Stride+=25;

     // maximum value reached, prevents legs from colliding.
    if(Stride>450) {
	    Stride=450;
    }
  }

  // temporary value for angle calculations
  float A;

  // results of trigometric functions
  double Xa,Knee,Hip;

  // position of legs in circular motion from 0° to 360°
  static int Step;

  // calculate positions for odd numbered legs 1,3,5
  for(int i=0;i<6;i+=2) {
    // angle of leg on the body + angle of travel
    A=float(60*i+angle);

    // keep value within 0°-360°
    if(A>359) {
    	A-=360;
    }

   	// convert degrees to radians for SIN function
    A=A*PI/180;

    // Xa value for rotation
    Xa=Stride*rotate;

    // hip movement affected by walking angle
    if(rotate == 0) {
    	// Xa hip position multiplier for walking at an angle
      Xa=sin(A)*-Stride;
    }

    // angle of leg
    A=float(Step);

    // convert degrees to radians for SIN function
    A=A*PI/180;
    Knee=sin(A)*Stride;
    Hip=cos(A)*Xa;

    // update knee  servos 1,3,5
    sv[i*2].writeMicroseconds(svc[i*2]+int(Knee));

    // update hip servos 1,3,5
    sv[i*2+1].writeMicroseconds(svc[i*2+1]+int(Hip));
  }

  // calculate positions for even numbered legs 2,4,6
  for(int i=1;i<6;i+=2) {
  	// angle of leg on the body + angle of travel
    A=float(60*i+angle);

    // keep value within 0°-360°
    if(A>359) {
    	A-=360;
    }

   	// convert degrees to radians for SIN function
    A=A*PI/180;

    // Xa value for rotation
    Xa=Stride*rotate;

    // hip movement affected by walking angle
    if(rotate == 0) {
    	// Xa hip position multiplier for walking at an angle
      Xa=sin(A)*-Stride;
    }

    // angle of leg
    A=float(Step+180);

    // keep value within 0°-360°
    if(A>359) {
    	A-=360;
    }

    // convert degrees to radians for SIN function
    A=A*PI/180;
    Knee=sin(A)*Stride;
    Hip=cos(A)*Xa;

    // update knee  servos 2,4,6
    sv[i*2].writeMicroseconds(svc[i*2]+int(Knee));

    // update hip servos 2,4,6
    sv[i*2+1].writeMicroseconds(svc[i*2+1]+int(Hip));
  }

  // cycle through circular motion of gait
  Step+=Speed;

  // keep value within 0°-360°
  if (Step>359) {
  	Step-=360;
  }
   // keep value within 0°-360°
  if (Step<0){
  	Step+=360;
  }
}
