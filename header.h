#pragma once

#include <Adafruit_GFX.h>
#include <Adafruit_NeoPixel.h>
#include <VL53L1X.h>
#include <algorithm>
#include <Wire.h>
#include <Adafruit_BNO055.h>
#include <Servo.h>

inline Servo leftServo;
inline Servo rightServo;



#define LED_PIN 6
#define NUM_LEDS 24
#define LEFTBACKTOF 3
#define LEFTFRONTTOF 2
#define RIGHTFRONTTOF 1
#define RIGHTBACKTOF 0
#define BNOPORT 0

inline VL53L1X leftback, lefttop, rightback, righttop;



inline const byte trig = 17;
inline const byte echo = 16;


void rightmuxselect(int port);

void leftmuxselect(int port);

float getDist();

class dists {
  public:
    int right_front, right_back, left_front, left_back, ultrasonic;



    void update_dist() {
      leftmuxselect(LEFTBACKTOF);
      left_back = leftback.read();
      leftmuxselect(LEFTFRONTTOF);
      left_front = lefttop.read();
      rightmuxselect(RIGHTBACKTOF);
      right_back = rightback.read();
      rightmuxselect(RIGHTFRONTTOF);
      right_front = righttop.read();
      ultrasonic = getDist();
    }



};


inline dists dist;
int cmap(int val);

class motor {
  public:
    uint8_t fpin, rpin;
    void speed(int val) {
      int map_speed = cmap(abs(val));
      if (val > 0) {
        analogWrite(fpin, map_speed);
        analogWrite(rpin, 0);
      }
      else {
        analogWrite(fpin, 0);
        analogWrite(rpin, map_speed);
      }
    }
};



inline motor motorFR{12, 13};
inline motor motorBR{9, 8};
inline motor motorFL{15, 14};
inline motor motorBL{11, 10};

void stop(int time);


void move(int L, int R);

void tofinit();
inline Adafruit_BNO055 bno(55, 0x28);


struct bnodata {
  int x;
  int y;
  int z;
  void updateOrientation() {
    leftmuxselect(BNOPORT);
    int counter = 0;
    x = 0;
    while (x == 0 && counter++ < 100) {
      sensors_event_t n;
      bno.getEvent(&n);
      x = n.orientation.x;
    }
    counter = 0;
    y = 0;
    while (y == 0 && counter++ < 100) {
      sensors_event_t n;
      bno.getEvent(&n);
      y = n.orientation.y;
    }


    counter = 0;
    z = 0;
    while (z == 0 && counter++ < 100) {
      sensors_event_t n;
      bno.getEvent(&n);
      z = n.orientation.z;
    }
  }

};

inline bnodata data;




void turnright(float degrees);
void turnleft(float degrees);





void servoSetup();

void cupUp();
void cupDown();
void cupMid();
