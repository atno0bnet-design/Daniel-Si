#include "header.h"


void rightmuxselect(int port) {
  if (port > 3) {
    return;
  }
  Wire1.beginTransmission(0x70);
  Wire1.write(1 << port);
  int ret = Wire1.endTransmission();

  if (ret != 0) {
    Serial.print("right Mux error: ");
    Serial.println(ret);
    while (1);
  }

}


void leftmuxselect(int port) {
  if (port > 3) {
    return;
  }
  Wire.beginTransmission(0x70);
  Wire.write(1 << port);
  int ret = Wire.endTransmission();

  if (ret != 0) {
    Serial.print("left Mux error: ");
    Serial.println(ret);
    while (1);
  }

}



float getDist() {
  float dist = 0;
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);
  dist = pulseIn(echo, HIGH, 5830);
  if (dist == 0) {
    return 200.0;
  }
  else {
    return dist * (0.343) / 2.0;
  }
}



void tofinit() {
  //left side tofs
  leftmuxselect(LEFTBACKTOF);
  leftback.setTimeout(500);
  if (!leftback.init()) {
    Serial.println("Left black tof failed");
    while (1);
  }



  leftback.setDistanceMode(VL53L1X::Long);
  leftback.setMeasurementTimingBudget(50000);
  leftback.startContinuous(50);



  leftmuxselect(LEFTFRONTTOF);
  lefttop.setTimeout(500);
  if (!lefttop.init()) {
    Serial.println("Left front tof failed");
    while (1);
  }



  lefttop.setDistanceMode(VL53L1X::Long);
  lefttop.setMeasurementTimingBudget(50000);
  lefttop.startContinuous(50);


  //right side tofs
  rightmuxselect(RIGHTBACKTOF);

  rightback.setBus(&Wire1);
  rightback.setTimeout(500);
  if (!rightback.init()) {
    Serial.println("Right black tof failed");
    while (1);
  }



  rightback.setDistanceMode(VL53L1X::Long);
  rightback.setMeasurementTimingBudget(50000);
  rightback.startContinuous(50);

  Serial.printf("Right back: %d\n", rightback.read());

  rightmuxselect(RIGHTFRONTTOF);
  righttop.setBus(&Wire1);
  righttop.setTimeout(500);
  if (!righttop.init()) {
    Serial.println("Right front tof failed");
    while (1);
  }



  righttop.setDistanceMode(VL53L1X::Long);
  righttop.setMeasurementTimingBudget(50000);
  righttop.startContinuous(50);
}






bool obby() {
  float dist = getDist();
  if (dist < 20) {
    Serial.println(dist);
    tone(22, 800, 100);
    tone(22, 400, 100);
    return true;
  }
  else {
    return false;
  }

}



void colorinit() {
  leftmuxselect(LEFTCOLOR);
  if (leftcolor.begin() == 0) {
    Serial.println("LEFTTTTTTTTTT COLOR SENSOR FAILED");
    while (true);
  }
  rightmuxselect(RIGHTCOLOR);
  if (rightcolor.begin() == 0)
  {
    Serial.println("RIGHT COLOR SENSOR FAILED");
    while (true);
  }

  leftmuxselect(CUPCOLOR);
  if (cupcolor.begin() == 0 ) {
    Serial.println("CUP COLOR FAILED");
    while (1);
  }

  cupcolor.enableColor(true);
  leftcolor.enableColor(true);
  rightcolor.enableColor(true);
}
