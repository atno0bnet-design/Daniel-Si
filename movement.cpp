#include "header.h"


int cmap(int val) {
  int mapped = map(val, 0 , 100, 0, 255);
  return constrain(mapped, 0, 255);

}


void stop(int time)
{
  motorFR.speed(0);
  motorFL.speed(0);
  motorBR.speed(0);
  motorBL.speed(0);
  delay(time);

}

void move(int L, int R) {
  motorFL.speed(L);
  motorBL.speed(L);
  motorFR.speed(-R);
  motorBR.speed(-R);
}



void turnright(float degrees) {

  stop(100);
  data.updateOrientation();
  float startHeading = data.x;
  float targetHeading = startHeading + degrees;
  if (targetHeading >= 360) targetHeading -= 360;
  Serial.print("Turning from ");
  Serial.print(startHeading);
  Serial.print(" to ");
  Serial.println(targetHeading);

  while (true) {
    data.updateOrientation();
    double currentHeading = data.x;

    float turn = targetHeading - currentHeading;

    if (turn < -180) turn += 360;
    if (turn > 180) turn -= 360;

    if (abs(turn) < 4) break;

    move(30, -30);
  }
  stop(100);
}



void turnleft(float degrees) {

  stop(100);
  data.updateOrientation();

  float startHeading = data.x;
  float targetHeading = startHeading - degrees;
  if (targetHeading >= 360) targetHeading -= 360;
  Serial.print("Turning from ");
  Serial.print(startHeading);
  Serial.print(" to ");
  Serial.println(targetHeading);

  while (true) {
    data.updateOrientation();
    double currentHeading = data.x;

    float turn = targetHeading - currentHeading;

    if (turn < -180) turn += 360;
    if (turn > 180) turn -= 360;

    if (abs(turn) < 4) break;

    move(-30, 30);
  }
  stop(100);
}




void servoSetup() {
  cupDown();
  leftServo.attach(5, 400, 2400);
  rightServo.attach(4, 400, 2400);
  cupDown();
}



void cupUp() {
  leftServo.write(75);
  rightServo.write(95);
  delay(100);
}

void cupDown() {
  leftServo.write(175);
  rightServo.write(5);
  delay(100);
}

void cupMid() {

}




void rightObstacle() {

  while (true) {
    if(Serial1.available()){
      stop(100);
      while(1);  
    }
    dist.update_dist();
    int d = dist.left_front;
    Serial.printf("dist:%d\n", d);
    while (d > 15) {
      dist.update_dist();
      d = dist.left_front;
      Serial.printf("dist:%d\n", d);
      move(-45, 45);
    }
    stop(5);
    move(50, 50);
    delay(100);
    stop(5);
  }



}


void leftObstacle() {
  while (true) {
    if(Serial1.available()){
      stop(100);
      while(1);  
    }
    dist.update_dist();
    int d = dist.right_front;
    Serial.printf("dist:%d\n", d);
    while (d > 15) {
      dist.update_dist();
      d = dist.right_front;
      Serial.printf("dist:%d\n", d);
      move(45, -45);
    }
    stop(5);
    move(50, 50);
    delay(100);
    stop(5);
  }

}
