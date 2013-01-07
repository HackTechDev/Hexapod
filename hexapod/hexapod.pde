#include <IRremote.h>
#include <IRremoteInt.h> 

#define IRtest 0                                           // set to 1 for IR test mode

#include <Servo.h>                                         // Standard Arduino servo libarary

int svc[12] = {                                              // servo center positions (typically 1500uS)
  1500,1550,1550,1450,                                     // D29 knee1, D46 Hip1, D47 knee2, D48 Hip2
  1500,1400,1500,1550,                                     // D49 knee3, D50 Hip3, D51 knee4, D24 Hip4
  1500,1500,1500,1400                                      // D25 knee5, D26 Hip5, D27 knee6, D28 Hip6
};

Servo sv[12];                                              // Yes! servos can be an array

int angle;                                                 // determines the direction/angle (0°-360°) that the robot will walk in 
int rotate;                                                // rotate mode: -1 = anticlockwise, +1 = clockwise, 0 = no rotation
int Speed;                                                 // walking speed: -15 to +15 
int Stride;                                                // size of step: exceeding 400 may cause the legs to hit each other

int RECV_PIN = 10;                                         // define D10 as input for IR receiver
IRrecv irrecv(RECV_PIN);
decode_results results;

void setup() {
  pinMode(11,OUTPUT);                                      // use D11 as GND for IR receiver
  digitalWrite(11,0);
  pinMode(12,OUTPUT);                                      // use D12 as Vcc for IR receiver
  digitalWrite(12,1);
   Serial.print("Setup");
  sv[0].attach(29,800,2200);                               // knee 1 
  delay(40);
  sv[1].attach(46,800,2200);                               // Hip  1
  delay(40);
  sv[2].attach(47,800,2200);                               // knee 2
  delay(40);
  sv[3].attach(48,800,2200);                               // Hip  2
  delay(40);
  sv[4].attach(49,800,2200);                               // knee 3
  delay(40);
  sv[5].attach(50,800,2200);                               // Hip  3
  delay(40);
  sv[6].attach(51,800,2200);                               // knee 4
  delay(40);
  sv[7].attach(24,800,2200);                               // Hip  4
  delay(40);
  sv[8].attach(25,800,2200);                               // knee 5
  delay(40);
  sv[9].attach(26,800,2200);                               // Hip  5
  delay(40);
  sv[10].attach(27,800,2200);                              // knee 6
  delay(40);
  sv[11].attach(28,800,2200);                              // Hip  6
  delay(40);
  
  for(int i=0;i<12;i++) {
    sv[i].writeMicroseconds(svc[i]);                       // initialize servos
  }
  delay(3000);                                             // optional - gives you time to put the robot down before it starts
  
  if(IRtest==1) Serial.begin(9600);                        // IR test mode displays IR receiver values on serial monitor
  irrecv.enableIRIn();                                     // Start the receiver
}

void loop() { 
  if (irrecv.decode(&results))                             // check for IR command
  {                                                        // change IRC comparison values to suit your TV, DVD, Stereo remote
    int IRC=results.value;
    

      Serial.print("Commande IR : ");
      Serial.println(IRC);                     // display value from IR receiver on serial monitor in test mode
    
    if(IRC==2640 || IRC==528 || IRC==255)                              // STOP
    {
      Serial.print("Stop\n");      
      Speed=0;
      angle=0;
      rotate=0;
    }
    
    if(IRC==144 || IRC==2064 || IRC==-32641)                              // FORWARD
    {
      Serial.print("Forward\n");
      Speed=10;
      rotate=0;
      angle=0;
    }
    
    if(IRC==2192 || IRC==3600 || IRC==-28561)                             // REVERSE    
    {
      Serial.print("Reverse\n"); 
      Speed=-10;
      rotate=0;
      angle=0;
    }
    
    if(IRC==1168 || IRC==24735)                                          // ROTATE CLOCKWISE  
    {
      Serial.print("Rotate Clockwise\n"); 
      Speed=10;
      rotate=1;
      angle=0;
    }
    
    if(IRC==3216 || IRC==8415)                                          // ROTATE COUNTER CLOCKWISE  
    {
      Serial.print("Rotate Counter Clockwise\n"); 
      Speed=10;
      rotate=-1;
      angle=0;
    }
    
    // Touche 1
    if(IRC==16 || IRC==2295)                                            // 45 DEGREES    
    {
      Serial.print("45 Degrees\n"); 
      Speed=10;
      rotate=0;
      angle=45;
    }
    
    // Touche 2
    if(IRC==3088 || IRC==-30601)                                          // 90 DEGREES    
    {
      Serial.print("90 Degrees\n"); 
      Speed=10;
      rotate=0;
      angle=90;
    }
    
    // Touche 3
    if(IRC==1552 || IRC==18615)                                          // 135 DEGREES    
    {
      Serial.print("135 Degrees\n"); 
      Speed=10;
      rotate=0;
      angle=135;
    }
    
    // Touche 4
    if(IRC==272 || IRC==10455)                                           // 225 DEGREES    
    {
      Serial.print("225 Degrees\n"); 
      Speed=10;
      rotate=0;
      angle=225;
    }
    
    // Touche 5
    if(IRC==2576 || IRC==-22441)                                          // 270 DEGREES    
    {
      Serial.print("270 Degrees\n"); 
      Speed=10;
      rotate=0;
      angle=270;
    }
    
    // Touche 6
    if(IRC==1040 || IRC==26775)                                          // 315 DEGREES    
    {
      Serial.print("315 Degrees\n"); 
      Speed=10;
      rotate=0;
      angle=315;
    }
    irrecv.resume();                                       // receive the next value
  }
  
  
  if(IRtest == 1) {
  	return;                                    // robot does not walk in IRtest mode
  }
  
  if (angle < 0) {
  	angle += 360;                                 // keep travel angle within 0°-360°
	}
	
  if (angle > 359) {
  	angle -= 360;                               // keep travel angle within 0°-360°
	}
	
  Walk();                                                  // move legs to generate walking gait
  delay(15);
}


void Walk()                                                // all legs move in a circular motion
{

  if(Speed==0)                                             // return all legs to default position when stopped
  {
    Stride-=25;                                            // as Stride aproaches 0, all servos return to center position
    if(Stride<0) Stride=0;                                 // do not allow negative values, this would reverse movements
  }
  else                                                     // this only affects the robot if it was stopped
  {
    Stride+=25;                                            // slowly increase Stride value so that servos start up smoothly
    if(Stride>450) Stride=450;                             // maximum value reached, prevents legs from colliding.
  }
  
  float A;                                                 // temporary value for angle calculations
  double Xa,Knee,Hip;                                      // results of trigometric functions
  static int Step;                                         // position of legs in circular motion from 0° to 360°                               
  
  for(int i=0;i<6;i+=2)                                    // calculate positions for odd numbered legs 1,3,5
  {
    A=float(60*i+angle);                                   // angle of leg on the body + angle of travel
    if(A>359) A-=360;                                      // keep value within 0°-360°
   
    A=A*PI/180;                                            // convert degrees to radians for SIN function
    
    Xa=Stride*rotate;                                      // Xa value for rotation
    if(rotate==0)                                          // hip movement affected by walking angle
    {
      Xa=sin(A)*-Stride;                                   // Xa hip position multiplier for walking at an angle
    }
        
    A=float(Step);                                         // angle of leg
    A=A*PI/180;                                            // convert degrees to radians for SIN function
    Knee=sin(A)*Stride;
    Hip=cos(A)*Xa;
    
    sv[i*2].writeMicroseconds(svc[i*2]+int(Knee));         // update knee  servos 1,3,5
    sv[i*2+1].writeMicroseconds(svc[i*2+1]+int(Hip));      // update hip servos 1,3,5
  }
  
  for(int i=1;i<6;i+=2)                                    // calculate positions for even numbered legs 2,4,6
  {
    A=float(60*i+angle);                                   // angle of leg on the body + angle of travel
    if(A>359) A-=360;                                      // keep value within 0°-360°
   
    A=A*PI/180;                                            // convert degrees to radians for SIN function
    Xa=Stride*rotate;                                      // Xa value for rotation
    if(rotate==0)                                          // hip movement affected by walking angle
    {
      Xa=sin(A)*-Stride;                                   // Xa hip position multiplier for walking at an angle
    }
        
    A=float(Step+180);                                     // angle of leg
    if(A>359) A-=360;                                      // keep value within 0°-360°
    A=A*PI/180;                                            // convert degrees to radians for SIN function
    Knee=sin(A)*Stride;
    Hip=cos(A)*Xa;
    
    sv[i*2].writeMicroseconds(svc[i*2]+int(Knee));         // update knee  servos 2,4,6
    sv[i*2+1].writeMicroseconds(svc[i*2+1]+int(Hip));      // update hip servos 2,4,6
  }
  
  Step+=Speed;                                             // cycle through circular motion of gait
  if (Step>359) Step-=360;                                 // keep value within 0°-360°
  if (Step<0) Step+=360;                                   // keep value within 0°-360°
}
