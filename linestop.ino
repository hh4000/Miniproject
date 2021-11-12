#include <Wire.h>​
#include <Zumo32U4.h>
Zumo32U4LineSensors lineSensors;
Zumo32U4ButtonA buttonA;
Zumo32U4Motors motors;

#define NUM_SENSORS 3
uint16_t lineSensorValues[NUM_SENSORS];
bool useEmitters = true;
bool completed= false;
struct LineSensorsWhite{ // True if White, False if Black 
    bool L;
    bool C;
    bool R;
};

int threshold[NUM_SENSORS] = {400,400,400}; // White threshold, white return values lower than this 
LineSensorsWhite sensorsState = {0,0,0}; // populate the struct with false values ( 0 ), like sensorsState = {false,false,false,false,false};​

void setup(){
    lineSensors.initFiveSensors();
    Serial.begin(9600);
    delay(1000);
}

void loop() {
  motors.setSpeeds(105,100);
  delay(500);
  while(completed==false){
  readSensors(sensorsState);
  checkWhiteForward();
  Serial.println(completed);
  delay(50);
  }
  motors.setSpeeds(-105,-100);
  delay(500);
  while(completed==true){
  readSensors(sensorsState);
  checkWhiteBack();
  Serial.println(completed);
  delay(50);
  }
}


void readSensors(LineSensorsWhite &state){
  // Next line reads the sensor values and store them in the array lineSensorValues , aparameter passed by reference
  lineSensors.read(lineSensorValues, useEmitters ? QTR_EMITTERS_ON : QTR_EMITTERS_OFF);
  // In the following lines use the values of the sensors to update the struct 
  sensorsState = {false,false,false};
  if ( lineSensorValues[0] < threshold[0])
    sensorsState.L = true;
  if ( lineSensorValues[1] < threshold[1])
    sensorsState.C = true;
  if ( lineSensorValues[2] < threshold[2])
    sensorsState.R = true;
}




void checkWhiteBack(){  
  if(sensorsState.L==false && sensorsState.C==false && sensorsState.R==false){
    motors.setSpeeds(-105,-100);
  }
  else{
    motors.setSpeeds(0,0);
    completed=false;
  }
  }

void checkWhiteForward(){

  if(sensorsState.L==false && sensorsState.C==false && sensorsState.R==false){
    motors.setSpeeds(105,100);
  }
  else{
    motors.setSpeeds(0,0);
    completed=true;
  }
  }
