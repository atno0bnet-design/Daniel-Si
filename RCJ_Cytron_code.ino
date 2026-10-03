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


  tofinit();
  servoSetup();
  while(digitalRead(21));
  delay(500);
  cupDown();
  Serial1.write('z');




}

void loop() {
  data.updateOrientation();

  //Serial.printf("BNO data x:%d y:%d z:%d\n", data.x, data.y, data.z);



  dist.update_dist();


  //Serial.printf("left back: %d left front %d right back %d right front %d\n", dist.left_back, dist.left_front, dist.right_back, dist.right_front);
  
  

  



  while (Serial1.available()) {


    char c = Serial1.read();
    if (c == '\n') {
      buffy[ind] = '\0';
      ind = 0;

      sscanf(buffy, "[%d,%d,%d]", &condition, &L_motor, &R_motor);

    Serial.printf("%d %d %d\n",condition,L_motor,R_motor);

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
            while (1);
            Serial1.write('z');
            break;
          }
        case 2: {
            //right
            break;
          }
        case 3: {
            turnright(180);
            Serial1.write('z');
            break;
          }
        case 4: {
            //Serial.printf("RS: %d\nLS: %d\n",R_motor, L_motor);

            move(L_motor, R_motor);
          }

      }

    }
    else {
      if (ind < 100 - 1) {
        buffy[ind] = c;
        ind++;
      }



    }
  }


}
