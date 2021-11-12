#include <Zumo32U4.h>
#include <Wire.h>

Zumo32U4LCD lcd;
Zumo32U4LineSensors lineSensors;
Zumo32U4ProximitySensors proxSensors;

int cans = 0;

//Here you can set the strength on the 6 diffrent pulses the IR sensor sends out
uint16_t levels[] = { 1, 1, 3, 3, 5, 5 };

void setup() {
  Serial.begin(9600);

  // Define lineSensors and proxSensor
  // Set lineSensors 5 (1, 2, 3, 4, 5)
  // Set proxSensor 1 (front)
  lineSensors.initFiveSensors();
  proxSensors.initFrontSensor();

  //This funktion update the strength of the different pulses on the IR sensor. The values is found in the array "levels"
  proxSensors.setBrightnessLevels(levels, sizeof(levels) / 2);

}

void loop() {

  //This should run in a loop when the zumo is sitting on top of the IR sensor that controlles the belt
  //Should have potential to be overridden
  sortCans();

}

void proxSensor() {

  //This turns on the proximitysensor and sends out 6 pulses on the LEDs, that travels different lengths
  //It then gives a value from 1 to 6, depending on how many pulses that have reflektet back
  //This also turns off linesensors so the belt stops
  //Remember to turn on linesensors with "lineSensors.emittersOn()", if linesensors are needed again
  proxSensors.read();

/*
  //Just visual

  //  Serial.println(proxSensors.countsFrontWithLeftLeds());
  //  Serial.println(proxSensors.countsFrontWithRightLeds());
  //  lcd.setCursor(0, 0);
  //  lcd.print("L");
  //  lcd.setCursor(7, 0);
  //  lcd.print("R");
  //
  //  lcd.setCursor(0, 1);
  //  lcd.print(proxSensors.countsFrontWithLeftLeds());
  //  lcd.setCursor(7, 1);
  //  lcd.print(proxSensors.countsFrontWithRightLeds());
*/

  //This determance if there is a can in front of the zumo and if there is, then the size of it
  //It uses the data from the IR sensor, to determan how close the can is
  if (proxSensors.countsFrontWithLeftLeds() and proxSensors.countsFrontWithRightLeds() > 2 < 5 ) {
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

      //Potentialy have it count the number of large cans

      //Just visual
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Size");
      lcd.setCursor(0, 1);
      lcd.print("Large");

      delay(2000);

      break;

    case 2:
      //If this code uses linesensors, then run "lineSensors.emittersOn()"
      //Don't turn linesensors on before the zumo has left the sensor that controlles the belt

      //Potentialy have it count the number of small cans

      //Just visual
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Size");
      lcd.setCursor(0, 1);
      lcd.print("Small");

      delay(2000);

      break;

    default:

      //Just visual
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Waiting");
      lcd.setCursor(0, 1);
      lcd.print("For Cans");

      //This turns the linesensors back on, so the belt runs again
      lineSensors.emittersOn();

      //This delay set how long the linesensors is on for, between tjeks on the proxsimitysensor.
      //It controlles how long the belt should run for
      delay(1000);

      break;

  }

}
