

#include <Adafruit_GFX.h>
#include <Adafruit_NeoPixel.h>
#include <VL53L1X.h>
#include <algorithm>
#define LED_PIN 6
#define NUM_LEDS 24
#define LEFTBACKTOF 3
#define LEFTFRONTTOF 2
#define RIGHTFRONTTOF 1
#define RIGHTBACKTOF 0
#define BNOPORT 0
Adafruit_NeoPixel ring(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);
VL53L1X leftback, lefttop, rightback, righttop;




#include <Wire.h>
#include <Adafruit_BNO055.h>

const byte trig = 17;
const byte echo = 16;


struct bnodata {
  int x = 0;
  int y = 0;
  int z = 0;
 };


Adafruit_BNO055 bno(55, 0x28);



int og = 0;
//bnodata data = {0,0,0};
int condition = 0;

int L_motor = 0;
int R_motor = 0;


char buffy[100];
  int ind = 0;
char buff[100];

void rightmuxselect(int port){
  if(port > 3){
    return;
  }
  Wire1.beginTransmission(0x70);
  Wire1.write(1<<port);
  int ret = Wire1.endTransmission();

  if(ret!=0){
    Serial.print("right Mux error: ");
    Serial.println(ret);
    while(1);
  }

}


void leftmuxselect(int port){
  if(port > 3){
    return;
  }
  Wire.beginTransmission(0x70);
  Wire.write(1<<port);
  int ret = Wire.endTransmission();

  if(ret!=0){
    Serial.print("left Mux error: ");
    Serial.println(ret);
    while(1);
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



class dists {
  public: 
    int right_front, right_back, left_front, left_back, ultrasonic;



    void update_dist(){
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





dists dist;




// DO NOT DELETE
enum bub {l1 = 50, l2 = 50, r1 = 50, r2 = 50};
// DO NOT DELETE



int cmap(int val) {
  int mapped = map(val, 0 , 100, 0, 255);
  return constrain(mapped, 0, 255);

}

struct motor {
  uint8_t fpin, rpin;
  void speed(int val);
};




void motor::speed(int val) {
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
motor motorFR{12, 13};
motor motorBR{9, 8};
motor motorFL{15, 14};
motor motorBL{11, 10};

void stop(int time)
{
  motorFR.speed(0);
  motorFL.speed(0);
  motorBR.speed(0);
  motorBL.speed(0);
  delay(time);

}

void move(int L,int R) {
  motorFL.speed(L);
  motorBL.speed(L);
  motorFR.speed(-R);
  motorBR.speed(-R);
}

void getYaw(int *data) {
  leftmuxselect(BNOPORT);
  int counter = 0;
  *data = 0;
  while (*data == 0 && counter++ < 100) {
    sensors_event_t n;
    bno.getEvent(&n);
    *data = n.orientation.x;
  }
}

void getPitch(int *data) {
  leftmuxselect(BNOPORT);
  int counter = 0;
  *data = 0;
  while (*data == 0 && counter++ < 100) {
    sensors_event_t n;
    bno.getEvent(&n);
    *data = n.orientation.y;
  }
}



void getRoll(int *data) {
  leftmuxselect(BNOPORT);
  int counter = 0;
  *data = 0;
  while (*data == 0 && counter++ < 100) {
    sensors_event_t n;
    bno.getEvent(&n);
    *data = n.orientation.z;
  }
}


void turnright(float degrees) {
  leftmuxselect(BNOPORT);
  stop(100);
  sensors_event_t sensorData;
  bno.getEvent(&sensorData);
  float startHeading = sensorData.orientation.x;
  float targetHeading = startHeading + degrees;
  if (targetHeading >= 360) targetHeading -= 360;
  Serial.print("Turning from ");
  Serial.print(startHeading);
  Serial.print(" to ");
  Serial.println(targetHeading);

  while (true) {
    bno.getEvent(&sensorData);
    double currentHeading = sensorData.orientation.x;

    float turn = targetHeading - currentHeading;

    if (turn < -180) turn += 360;
    if (turn > 180) turn -= 360;

    if (abs(turn) < 2) break;

    move(50, -50);
    delay(10);
  }
  stop(100);
}




void turnleft(float degrees) {
  leftmuxselect(BNOPORT);
  stop(100);
  sensors_event_t sensorData;
  bno.getEvent(&sensorData);
  float startHeading = sensorData.orientation.x;
  float targetHeading = startHeading - degrees;
  if (targetHeading >= 360) targetHeading -= 360;
  Serial.print("Turning from ");
  Serial.print(startHeading);
  Serial.print(" to ");
  Serial.println(targetHeading);

  while (true) {
    bno.getEvent(&sensorData);
    double currentHeading = sensorData.orientation.x;

    float turn = targetHeading - currentHeading;

    if (turn < -180) turn += 360;
    if (turn > 180) turn -= 360;

    if (abs(turn) < 2) break;

    move(-50, 50);
    delay(10);
  }
  stop(100);
}

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
    while(1);

  }

  pinMode(20, INPUT);
  pinMode(21, INPUT);




  
  Serial.println("hi");
  

 

  ring.begin();
    

  ring.setBrightness(5);

  for (int i = 0; i < 24; i++) {
    ring.setPixelColor(i, ring.Color(255, 255, 255));
    delay(100);
    ring.show();
  }




//left side tofs
leftmuxselect(LEFTBACKTOF);
  leftback.setTimeout(500);
  if(!leftback.init()){
    Serial.println("Left black tof failed");
    while(1);
  }



  leftback.setDistanceMode(VL53L1X::Long);
  leftback.setMeasurementTimingBudget(50000);
 leftback.startContinuous(50);



 leftmuxselect(LEFTFRONTTOF);
  lefttop.setTimeout(500);
  if(!lefttop.init()){
    Serial.println("Left front tof failed");
    while(1);
  }



  lefttop.setDistanceMode(VL53L1X::Long);
 lefttop.setMeasurementTimingBudget(50000);
  lefttop.startContinuous(50);


  //right side tofs
  rightmuxselect(RIGHTBACKTOF);
  
rightback.setBus(&Wire1);
  rightback.setTimeout(500);
  if(!rightback.init()){
    Serial.println("Right black tof failed");
    while(1);
  }



  rightback.setDistanceMode(VL53L1X::Long);
  rightback.setMeasurementTimingBudget(50000);
 rightback.startContinuous(50);

  Serial.printf("Right back: %d\n", rightback.read());

 rightmuxselect(RIGHTFRONTTOF);
 righttop.setBus(&Wire1);
  righttop.setTimeout(500);
  if(!righttop.init()){
    Serial.println("Right front tof failed");
    while(1);
  }



  righttop.setDistanceMode(VL53L1X::Long);
 righttop.setMeasurementTimingBudget(50000);
  righttop.startContinuous(50);

Serial.printf("Right front: %d\n", righttop.read());
  

  
  Serial.println("hi2");
  while (digitalRead(21));

  Serial1.write('z');
  getRoll(&og);



}

void loop() {

  dist.update_dist();
  
 
  Serial.printf("left back: %d left front %d right back %d right front %d\n", dist.left_back, dist.left_front, dist.right_back, dist.right_front);

  if(dist.ultrasonic<=40){

   int right_dist = (dist.right_front + dist.right_back)/2;
   int left_dist = (dist.left_front + dist.left_back) / 2;
   Serial.printf("right: %d   left: %d", right_dist, left_dist);
   if(left_dist>right_dist){
    turnleft(90);  
    Serial1.write('r');
    while(1){
      dist.update_dist();
      while(dist.right_front>=30){
        dist.update_dist();
        move(60,-60);
      }

      move(60,60);
      delay(100);  

    }
   }
   else{
    turnright(90);
    Serial1.write('l');
    while(1){
      dist.update_dist();
      while(dist.left_front>=30){
        dist.update_dist();
        move(-60,60);
      }


      move(60,60);
      delay(100);  

    }     

    
   }
    
    while(1);   
  }







    
  while(Serial1.available()){
    
 
    char c = Serial1.read();
    if(c=='\n'){
      buffy[ind] = '\0';
      ind = 0;

      sscanf(buffy,"[%d,%d,%d]",&condition,&L_motor,&R_motor);



      switch(condition){
        case 1:{
          //turndegrees
          if(L_motor>0){
            turnright(L_motor);
          }  
          else{
            turnleft(abs(L_motor));  
          }
          Serial1.write('z');
          break;
        } 
        case 2:{
          //right
          break;  
        } 
        case 3:{
          turnright(180);
          Serial1.write('z');
          break;
        }
        case 4:{
          //Serial.printf("RS: %d\nLS: %d\n",R_motor, L_motor); 
          
          move(L_motor,R_motor);  
        }
        
      }
      
    }
    else{
      if(ind<100-1){
        buffy[ind] = c;
        ind++;  
      }
      
      
      
    }
  }

  
}
