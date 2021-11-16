#include <Arduino.h>
#include <Wire.h>
#include <Zumo32U4.h>
//Zumo32U4LCD lcd;
//Zumo32U4IMU imu;
//Zumo32U4ButtonA buttonA;
//Zumo32U4Encoders encoders;
//Zumo32U4Motors motors;


double xdist = 10;
double ydist = 10;
double count = 0;
bool XYdicration = true; //true for x og false for y
double EncoderCount = 0;



struct lokation {
  int xvalue;
  int yvalue;
};


/*void setup() {
  Serial.begin(9600);

}

void loop() {
  distCal(xdist);
  delay(1000);
  distCal(xdist);

}*/


void distCal(double dist) {
  resetEncoders();
  int f = 1;
  if (dist < 0) f = -1;
  if (XYdicration == true) {
    
    count = dist*78.5; //Don't ask, it just works
    lcd.gotoXY(0,0);
    lcd.print("Driving");
    lcd.gotoXY(0,1);
    lcd.print((String)dist + " cm");
    
    //Serial.println((String)count);
    //Serial.println("Encoder1 count = " + (String)EncoderCount);
    while(f*encoders.getCountsRight()< f*count) {   
      motors.setSpeeds(f*100,f*100);
      if(encoders.getCountsRight()*1.01 > encoders.getCountsLeft()){
        motors.setSpeeds(f*300,f*100);
      } else if (encoders.getCountsRight()*1.01 < encoders.getCountsLeft()){
        motors.setSpeeds(f*100,f*300);
      }
    } 
      motors.setSpeeds(0, 0);
      lcd.clear();
    }


}

void resetEncoders() {
  encoders.getCountsAndResetLeft();
  encoders.getCountsAndResetRight();
}

