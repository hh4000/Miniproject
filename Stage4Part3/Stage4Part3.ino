#include <Arduino.h>
#include <Wire.h>
#include <Zumo32U4.h>
Zumo32U4LCD lcd;
Zumo32U4IMU imu;
Zumo32U4ButtonA buttonA;
Zumo32U4Encoders encoders;
Zumo32U4Motors motors;
Zumo32U4Buzzer buzzer;


void setup() {
  Serial.println(9600);
  turnSensorSetup();
  delay(500);
  turnSensorReset();
  lcd.clear();
  lcd.print("Press A");
  buttonA.waitForPress();
  lcd.clear();
  buzzer.playNote(NOTE_A(4),200,15);
  delay(500);
}

void loop() {
  checkturn(-180);
  distCal(45);
  checkturn(-90);
  distCal(20);
  checkturn(-90);
  lcd.clear();
  lcd.print("Done");
  delay(30000);

}
