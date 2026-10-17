#include "header.h"


Adafruit_NeoPixel ring(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);



int og = 0;

int condition = 0;

int L_motor = 0;
int R_motor = 0;

char buffy[100];
int ind = 0;
char buff[100];



void setup() {

  Serial.begin(115200);
  Serial.println("Hello");
  Serial1.setRX(1);
  Serial1.setTX(0);
  Serial1.begin(115200);
  delay(2000);
  Wire.setSCL(29);
  Wire.setSDA(28);
  Wire.begin();


  Wire1.setSCL(27);
  Wire1.setSDA(26);
  Wire1.begin();



  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);
  leftmuxselect(BNOPORT);
  if (bno.begin(OPERATION_MODE_IMUPLUS) == 0) {
    Serial.println("Bno failed");
    while (1);
  }

  pinMode(20, INPUT);
  pinMode(21, INPUT);

  ring.begin();
  ring.setBrightness(5);

  for (int i = 0; i < 24; i++) {
    ring.setPixelColor(i, ring.Color(255, 255, 255));
    delay(10);
    ring.show();
  }

  colorinit();


  tofinit();
  //servoSetup();
  while (1) {
    data.updateOrientation();
    Serial.printf("x: %d y:%d z: %d\n", data.x, data.y, data.z);
    //Serial.println(getDist());
    if (!digitalRead(21)) {
      cupUp();
      break;
    }
    if (!digitalRead(20)) {
      cupDown();
      break;
    }

  }
  delay(500);

  Serial1.write('z');




}

void loop() {
  data.updateOrientation();
    Serial.printf("x: %d y:%d z: %d\n", data.x, data.y, data.z);
  ramp_condition = getRamp();
  switch(ramp_condition){
    case up:{
      Serial1.print('u');
      break;
    }
    case right:{
      Serial1.print('q');
      break; 
    }  
    case left:{
      Serial.println(";llleft");
      Serial1.print('w');  
      break;
    }
    case down:{
      Serial1.write('d');
      break;  
    }
  }
  
  if (checkSilver() == true) {
    Serial1.write('e');
    stop(1);
    while (1);
  }
  if (obby() == true) {
    Serial1.write('o');
    move(-45, -45);
    delay(100);
    stop(100);

    while (Serial1.available()) {
      Serial1.read();
    }

    dist.update_dist();
    delay(500);
    int leftdist = (dist.left_front + dist.left_back) / 2;
    int rightdist = (dist.right_front + dist.right_back) / 2;
    Serial.printf("Left dist: %d    Right dist: %d\n", leftdist, rightdist);
    if (leftdist > rightdist) {


      Serial1.write('l');
      Serial.println("Left side");
      turnleft(90);
      leftObstacle();

    }
    else {
      Serial1.write('r');
      Serial.println("Right side");
      turnright(90);
      rightObstacle();

    }
  }






  if (Serial1.available()) {
    while (Serial1.available() && Serial1.peek() != '[') {
      Serial1.read();
    }


    if (Serial1.available() && Serial1.peek() == '[') {
      Serial1.read();
      condition = Serial1.parseInt();
      L_motor = Serial1.parseInt();
      R_motor = Serial1.parseInt();
      Serial1.read();
      Serial.printf("Condition: %d    L: %d    R: %d\n", condition, L_motor, R_motor);
      switch (condition) {
        case 1: {
            stop(100);
            //turndegrees
            Serial.println(abs(L_motor));
            if (R_motor == 1) {
              Serial.println("RIghtturn");
              turnright(abs(L_motor));
            }
            else {
              Serial.println("leftturn");
              turnleft(abs(L_motor));
            }
            Serial.println("Done turning");
            stop(100);
            move(45, 45);
            delay(400);


            stop(5);
            Serial1.write('z');
            break;

          }
        case 2: {
            //right
            if(L_motor == 2){
              move(45,45);
              delay(600);
              turnleft(80);
                move(45,45);
                delay(800);
            }
            if(L_motor == 1){
              Serial.println("right green on ramp");
              move(45,45);
              delay(100);
              turnright(95);
                move(-45,-45);
                delay(200);
            }
            Serial1.write('z');
            break;
          }
        case 3: {
            turnright(180);
            Serial1.write('z');
            break;
          }
        case 4: {
           
            Serial.printf("RS: %d\nLS: %d\n", R_motor, L_motor);
          

                move(L_motor, R_motor);

                  

            break;
          }
        case 5: {
            if(L_motor == 2){
              
              move(45,45);
              delay(100);
              turnleft(95);
                move(-45,-45);
                delay(200);
            }
            if(L_motor == 1){
              Serial.println("right green on ramp");
              move(45,45);
              delay(700);
              turnright(70);
                move(45,45);
                delay(800);
            }
            
            break;
          }
          case 6:{
            if(L_motor == 1){
              Serial.println("right green on down ramp");
              move(-45,-45);
              delay(400);
              turnright(80);
                move(45,45);
                delay(100);
            }
          }
        default: {
            tone(22, 100, 220);
            break;
          }
      }

    }
  }



}
