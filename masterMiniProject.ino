#include <Wire.h>​
#include <Zumo32U4.h>
Zumo32U4LCD lcd;
Zumo32U4LineSensors lineSensors;
Zumo32U4ProximitySensors proxSensors;
Zumo32U4ButtonA buttonA;
Zumo32U4Motors motors;
Zumo32U4Encoders encoders;
Zumo32U4IMU imu;

#define NUM_SENSORS 3
uint16_t lineSensorValues[NUM_SENSORS];
bool useEmitters = true;
bool completed = false;
struct LineSensorsWhite { // True if White, False if Black
  bool L;
  bool C;
  bool R;
};

int cans = 0;
double count = 0;
int largeCan = 0;
int smallCan = 0;

//Here you can set the strength on the 6 diffrent pulses the IR sensor sends out
uint16_t levels[] = { 1, 1, 3, 3, 5, 5 };

int threshold[NUM_SENSORS] = {400, 400, 400}; // White threshold, white return values lower than this
LineSensorsWhite sensorsState = {0, 0, 0}; // populate the struct with false values ( 0 ), like sensorsState = {false,false,false,false,false};​

uint32_t turnAngle = 0;
int turnAngle2 = turnAngle;
// turnRate is the current angular rate of the gyro, in units of
// 0.07 degrees per second.
int16_t turnRate;

// This is the average reading obtained from the gyro's Z axis
// during calibration.
int16_t gyroOffset;

// This variable helps us keep track of how much time has passed
// between readings of the gyro.
uint16_t gyroLastUpdate = 0;

//The amount of degrees the robot has turned
int turnAngleDegrees;

//These are the booleans values that the dependencies in the program relies on
bool xdriven        = false;
bool ydriven        = false;
bool turnedminus90  = false;
bool turned90       = false;

double EncoderCount = 0;
void setup() {
  Serial.begin(9600);
  lineSensors.initFiveSensors();
  proxSensors.initFrontSensor();
  //This funktion update the strength of the different pulses on the IR sensor. The values is found in the array "levels"
  proxSensors.setBrightnessLevels(levels, sizeof(levels) / 2);
  turnSensorSetup();
  delay(500);
  turnSensorReset();

  //set up the lcd
  countCans();

}

void loop() {

//Kører frem til linjen op stopper
//driveToLine();

//Christoffer kode indsættes for at blive ført til sensor

sortCans();
}


void readSensors(LineSensorsWhite &state) {
  // Next line reads the sensor values and store them in the array lineSensorValues , aparameter passed by reference
  lineSensors.read(lineSensorValues, useEmitters ? QTR_EMITTERS_ON : QTR_EMITTERS_OFF);
  // In the following lines use the values of the sensors to update the struct
  sensorsState = {false, false, false};
  if ( lineSensorValues[0] < threshold[0])
    sensorsState.L = true;
  if ( lineSensorValues[1] < threshold[1])
    sensorsState.C = true;
  if ( lineSensorValues[2] < threshold[2])
    sensorsState.R = true;
}

void depositLargeCan() {
  motors.setSpeeds(105, 100);
  delay(500);
  while (completed == false) {
    readSensors(sensorsState);
    checkWhiteForward();
  }
  motors.setSpeeds(-105, -100);
  delay(500);
  while (completed == true) {
    readSensors(sensorsState);
    checkWhiteBack();
  }
}

void driveToLine() {
  motors.setSpeeds(105, 100);
  delay(500);
  while (completed == false) {
    readSensors(sensorsState);
    checkWhiteForward();
    completed = false;
  }
}

void checkWhiteBack() {
  if (sensorsState.L == false && sensorsState.C == false && sensorsState.R == false) {
    motors.setSpeeds(-105, -100);
  }
  else {
    motors.setSpeeds(0, 0);
    completed = false;
  }
}

void checkWhiteForward() {

  if (sensorsState.L == false && sensorsState.C == false && sensorsState.R == false) {
    motors.setSpeeds(105, 100);
  }
  else {
    motors.setSpeeds(0, 0);
    completed = true;
  }
}
void proxSensor() {

  //This turns on the proximitysensor and sends out 6 pulses on the LEDs, that travels different lengths
  //It then gives a value from 1 to 6, depending on how many pulses that have reflektet back
  //This also turns off linesensors so the belt stops
  //Remember to turn on linesensors with "lineSensors.emittersOn()", if linesensors are needed again
  proxSensors.read();


  //This determance if there is a can in front of the zumo and if there is, then the size of it
  //It uses the data from the IR sensor, to determan how close the can is
  if (proxSensors.countsFrontWithLeftLeds() and proxSensors.countsFrontWithRightLeds() > 2
      and proxSensors.countsFrontWithLeftLeds() and proxSensors.countsFrontWithRightLeds() <= 4 ) {
    cans = 2;

  }
  else if (proxSensors.countsFrontWithLeftLeds() and proxSensors.countsFrontWithRightLeds() >= 5 ) {
    cans = 1;

  }
  else {
    cans = 0;

  }

}

void sortCans() {

  //Determance if there is a can
  //Determance the size of the can
  proxSensor();

  //This decides which funktion to run depending on the can size
  //If there are no cans on the belt, it just loops
  switch (cans) {
    case 1:
      //If this code uses linesensors, then run "lineSensors.emittersOn()"
      //Don't turn linesensors on before the zumo has left the sensor that controlles the belt

      //add one count to large can and updates the lcd screen
      largeCan = largeCan + 1;
      countCans();
      depositLargeCan();


      delay(2000);

      break;

    case 2:
      //If this code uses linesensors, then run "lineSensors.emittersOn()"
      //Don't turn linesensors on before the zumo has left the sensor that controlles the belt

      //add one count to small can and updates the lcd screen
      imustart();
      stage4DriveToCan();
      driveToLine();
      returnAfterDeposit();
      smallCan = smallCan + 1;
      countCans();
      

      delay(2000);

      break;

    default:


      //This turns the linesensors back on, so the belt runs again
      lineSensors.emittersOn();

      //This delay set how long the linesensors is on for, between tjeks on the proxsimitysensor.
      //It controlles how long the belt should run for
      delay(1000);

      break;

  }

}

void countCans() {

  //counts the large cans
  lcd.setCursor(0, 0);
  lcd.print("Large:");
  lcd.setCursor(6, 0);
  lcd.clear();
  lcd.print(largeCan);

  //counts the small cans
  lcd.setCursor(0, 1);
  lcd.print("Small:");
  lcd.setCursor(6, 1);
  lcd.clear();
  lcd.print(smallCan);

}
void turnTo(int angle)
{
  bool done = false;
  int f = 1;
  if(angle<0) f = -1; 
  while(done==false)
  {
    if(turnAngleDegrees >= angle && angle + 1>= turnAngleDegrees)
    {
      motors.setSpeeds(0, 0);
      done=true;
      imustart();
    }
    else
    {
      motors.setSpeeds(f*-100, f*100);
      imustart();
    }
  }
  turnSensorReset();
  turnAngleDegrees = 0;
}




///This is important for the turn sensor and the lcd screen.
void imustart()
{
  turnSensorUpdate();
  turnAngleDegrees = ((((int32_t)turnAngle >> 16) * 360) >> 16);
  lcd.gotoXY(0, 0);
  lcd.print((((int32_t)turnAngle >> 16) * 360) >> 16);
  lcd.print(F("   "));
}





//Ik rør ved noget herefter, dette er alt sammen Carlos kode der bliver brugt til gyro
/* This should be called in setup() to enable and calibrate the
  gyro.  It uses the LCD, yellow LED, and button A.  While the LCD
  is displaying "Gyro cal", you should be careful to hold the robot
  still.
  The digital zero-rate level of the gyro can be as high as
  25 degrees per second, and this calibration helps us correct for
  that. */

void turnSensorSetup()
{
  Wire.begin();
  imu.init();
  imu.enableDefault();
  imu.configureForTurnSensing();

  lcd.clear();
  lcd.print(F("Gyro cal"));

  // Turn on the yellow LED in case the LCD is not available.
  ledYellow(1);

  // Delay to give the user time to remove their finger.
  delay(500);

  // Calibrate the gyro.
  int32_t total = 0;
  for (uint16_t i = 0; i < 1024; i++)
  {
    // Wait for new data to be available, then read it.
    while (!imu.gyroDataReady()) {}
    imu.readGyro();

    // Add the Z axis reading to the total.
    total += imu.g.z;
  }
  ledYellow(0);
  gyroOffset = total / 1024;

  // Display the angle (in degrees from -180 to 180) until the
  // user presses A.
  lcd.clear();



}


// This should be called to set the starting point for measuring
// a turn.  After calling this, turnAngle will be 0.
void turnSensorReset()
{
  gyroLastUpdate = micros();
  turnAngle = 0;
}

// Read the gyro and update the angle.  This should be called as
// frequently as possible while using the gyro to do turns.
void turnSensorUpdate()
{
  // Read the measurements from the gyro.
  imu.readGyro();
  turnRate = imu.g.z - gyroOffset;

  // Figure out how much time has passed since the last update (dxt)
  uint16_t m = micros();
  uint16_t dt = m - gyroLastUpdate;
  gyroLastUpdate = m;

  // Multiply dt by turnRate in order to get an estimation of how
  // much the robot has turned since the last update.
  // (angular change = angular velocity * time)
  int32_t d = (int32_t)turnRate * dt;

  // The units of d are gyro digits times microseconds.  We need
  // to convert those to the units of turnAngle, where 2^29 units
  // represents 45 degrees.  The conversion from gyro digits to
  // degrees per second (dps) is determined by the sensitivity of
  // the gyro: 0.07 degrees per second per digit.
  //
  // (0.07 dps/digit) * (1/1000000 s/us) * (2^29/45 unit/degree)
  // = 14680064/17578125 unit/(digit*us)
  turnAngle += (int64_t)d * 14680064 / 17578125;
}

void distDrive(double dist) {
  resetEncoders();
  int f = 1;
  if (dist < 0) f = -1;
    
    count = dist*78.5; //Don't ask, it just works
    lcd.gotoXY(0,0);
    lcd.print("Driving");
    lcd.gotoXY(0,1);
    lcd.print((String)dist + " cm");

    
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

void resetEncoders() {
  encoders.getCountsAndResetLeft();
  encoders.getCountsAndResetRight();
}
void stage4DriveToCan() {
  turnTo(-90); //Drejer 90 grader til højre
  distDrive(25); //køre 25 cm
  turnTo(90);//Drejer 90 grader til venstre
  distDrive(18);// køre 18 cm
  turnTo(90); // Drejer 90 grader til venstre 
  //Stop(); //stopper i 5 s
}

void Stop(){
  motors.setSpeeds(0, 0);
  delay(5000);
}

void returnAfterDeposit(){
  turnTo(-180);
  distDrive(45);
  turnTo(-90);
  distDrive(20);
  turnTo(-90);
  lcd.clear();
  lcd.print("Done");
  delay(30000);
}
