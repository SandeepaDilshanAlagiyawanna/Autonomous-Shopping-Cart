#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <NewPing.h>
#include <SoftwareSerial.h>

SoftwareSerial nodeMcuSerial(3, 2);  // RX, TX pins connected to ESP32

Adafruit_PWMServoDriver pwm;

float camValue;

int item1 = 1;
int item2 = 2;
int item3 = 3;
int item4 = 4;

// Motor A connections
int in1 = 6;
int in2 = 7;
// Motor B connections
int in3 = 8;
int in4 = 9;

const int echoPin = 10;
const int trigPin = 11;

long duration;; 
float distance;

const float servoMin = 130;   
const float servoMax = 256;
const float maxRange = 18;  
const float upAngle = 7.0*10.0;
const float minusDistance = 8;
const float angleRange = 420.0/180.0; 
const float camAngle = 53.0/400.0;
float finalRotateAngle = 150.0;

float vehicleFinalDistance;
float item1Distance = 160;
float item2Distance = 120;
float item3Distance = 80;
float item4Distance = 40;
float timeForOneDistance = 1000.0/20.743;

float measureDistance(){
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);  
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);  
  duration = pulseIn(echoPin, HIGH);  
  distance = duration * 0.034 / 2;
  return distance;
}

void printDistance(){
  float distance = measureDistance();
  delay(1000);
  Serial.print("distance: ");
  Serial.print(distance);
  Serial.println(" cm");
}

void fixPosition(){
  pwm.setPWM(0, 0, 130);
  pwm.setPWM(1, 0, 130);
  pwm.setPWM(2, 0, 150);
  pwm.setPWM(3, 0, 180);
}

float rotateServoMovement(float rotateAngle){  
  float endAngle = 150.0 + angleRange*rotateAngle;
  for (float angle = finalRotateAngle; angle <= endAngle; angle++) {
    pwm.setPWM(2, 0, angle);     
    delay(25);
  }
  finalRotateAngle = endAngle; 
}

void backRotateAngle(){
  float endAngle = finalRotateAngle;
  for (float angle = endAngle; angle >= 150.0; angle--) {
    pwm.setPWM(2, 0, angle);     
    delay(25);
  }
}
void callToCamera(){
  Serial.println("CallCamera");
  nodeMcuSerial.println("CallCamera");  
  //delay(1000); 
  int x=0;
  while (x==0) {
    if(nodeMcuSerial.available()) {    
    String data = nodeMcuSerial.readStringUntil('\n');
    data.trim();     
    camValue = data.toFloat();
      if (data.length() > 0) {
        camValue = data.toFloat();
        Serial.println(camValue);
        x=1;      
      }    
    }    
  }   
}

void startMovement(){   
  delay(3000); 
  callToCamera();
  delay(1000);
  float itemCenter = camValue;
  float centerAngle = itemCenter * camAngle;
  float startAngle = finalRotateAngle;
  float plusEndAngle = startAngle - centerAngle;
  float minusEndAngle = startAngle - centerAngle;
        
  if (itemCenter == 0){
    Serial.println("Item is Center");
    finalRotateAngle = startAngle;   
  } else if (itemCenter < 0.0){    
      //delay(1000);
      for (float angle = startAngle; angle <= plusEndAngle; angle++) {
        pwm.setPWM(2, 0, angle);     
        delay(50);        
  }
    finalRotateAngle = plusEndAngle;
  } else if (400 >= itemCenter >0.0){
      //delay(1000);
      Serial.println("4"); 
      for (float angle = startAngle; angle >= minusEndAngle; angle--) {
        pwm.setPWM(2, 0, angle);     
        delay(50);
  }     
      finalRotateAngle = minusEndAngle; 
  } 
  delay(2000);
  float distance = measureDistance();
  float firstMoveDistance = distance - minusDistance;
  float servoRange = servoMax - servoMin;
  float oneDistanceServoRange = servoRange/maxRange;
  float fisrtMovementAngle = servoMin + firstMoveDistance*oneDistanceServoRange;
  
  printDistance();

  if(distance<=23){ 
    for (int angle = servoMin; angle <= fisrtMovementAngle; angle++) {
      pwm.setPWM(0, 0, angle);
      pwm.setPWM(1, 0, angle); 
      delay(100);
    }    
    delay(1000);

    printDistance();
   
    
    float secondDistance = measureDistance();
    float newSecondDistance = secondDistance -3.0;
    float secondMovementAngle = fisrtMovementAngle + newSecondDistance * oneDistanceServoRange;
    
    for (float angle = fisrtMovementAngle; angle <= secondMovementAngle; angle++) {
      pwm.setPWM(0, 0, angle);
      pwm.setPWM(1, 0, angle); 
      delay(100);
    }    
    delay(1000);    

    for (int angle = 150; angle <= 250; angle++) {
      pwm.setPWM(3, 0, angle);     
      delay(15);
    }
    delay(500);    
      
    float afterGripUpAngle = secondMovementAngle + upAngle;  
    for (float angle = secondMovementAngle; angle <= afterGripUpAngle; angle++) {    
      pwm.setPWM(1, 0, angle); 
      delay(50);
    } 
    delay(500);
      
    for (float angle = secondMovementAngle; angle >= 130; angle--) {
      pwm.setPWM(0, 0, angle);
      pwm.setPWM(1, 0, angle + upAngle); 
      delay(50);
    }   
    
    delay(500);
    
      for (int angle = finalRotateAngle; angle >= 150; angle--) {
      pwm.setPWM(2, 0, angle);     
      delay(50);
    } 
    delay(500);

    for (int angle = 350; angle >= 150; angle--) {
      pwm.setPWM(3, 0, angle);     
      delay(15);
    }
    delay(500);

      int startAngle = servoMin + upAngle;  
      for (int angle =  startAngle; angle >=servoMin; angle--) {
      pwm.setPWM(1, 0, angle);     
      delay(50);
    }
  }else {
    Serial.println("Distance is more high");
    for (float angle = finalRotateAngle; angle >= 150; angle--) {
      pwm.setPWM(2, 0, angle);     
      delay(50);
    }
  }
  
}

void findItem(){
  rotateServoMovement(60.0);
  delay(1000);
  callToCamera();
  if(camValue != 1000){
    startMovement();
  } else {
    rotateServoMovement(80.0);
    delay(1000);
    callToCamera();
    if(camValue != 1000){
      startMovement();
    }else{
      rotateServoMovement(100.0);
      delay(1000);
      callToCamera();
      if(camValue != 1000){
        startMovement();
      }else{
        rotateServoMovement(120.0);
        delay(1000);
        callToCamera();
        if(camValue != 1000){
          startMovement();
        }else{
          Serial.println("No any item in the rack"); 
          backRotateAngle();
        }
      }
    }
  }
}

int getOrder(){
  while(true){
    if (nodeMcuSerial.available()) {
    String receivedData = nodeMcuSerial.readStringUntil('\n');
    receivedData.trim();  // Remove leading/trailing whitespaces
    int receivedValue = receivedData.toInt();  // Convert the received string to an integer
    Serial.print("Received value: ");
    Serial.println(receivedValue);    
  }
  }
}  

void stopMotor(){
  digitalWrite(in1, LOW);
	digitalWrite(in2, LOW);
	digitalWrite(in3, LOW);
	digitalWrite(in4, LOW);
}

void moveVehicle(float item1Distance){  
  vehicleFinalDistance = measureDistance();
  float moveDistance = vehicleFinalDistance - item1Distance;
  float moveTime = moveDistance *timeForOneDistance;
  Serial.println(vehicleFinalDistance);
  Serial.println(moveTime); 
  Serial.println("strat move vehical");  
  Serial.println(moveDistance);
  digitalWrite(in1, LOW);
	digitalWrite(in2, HIGH);
	digitalWrite(in3, LOW);
	digitalWrite(in4, HIGH);
  Serial.println("befor delay move vehical");
  delay(3000);
  Serial.println("after delay move vehical");
  stopMotor();
Serial.println("after stop move vehical");
  }

void setup() {
  Serial.begin(9600);
  nodeMcuSerial.begin(9600);   // Initialize the software serial connection
  pinMode(trigPin, OUTPUT);  
  pinMode(echoPin, INPUT); 
  pwm.begin();
  pwm.setPWMFreq(50);

  // Set all the motor control pins to outputs	
	pinMode(in1, OUTPUT);
	pinMode(in2, OUTPUT);
	pinMode(in3, OUTPUT);
	pinMode(in4, OUTPUT);
	Serial.println("1");

  findItem();
  //rotateServoMovement();
     
  // startMovement(); 
	//stopMotor();

  //getOrder();  

  // if (item1 == 1){
  // //moveVehicle(item1Distance);   
  // fixPosition(); 
  // ; 
  // } 
  //else if (item2 == 2) {   
  // fixPosition();  
  // rotateServoFirstMovement();     
  // startMovement(); 
  // } else if (item3 == 3) {   
  // fixPosition();  
  // rotateServoFirstMovement();     
  // startMovement(); 
  // } else if (item4 == 4) {   
  // fixPosition();  
  // rotateServoFirstMovement();     
  // startMovement(); 
  // } 
}

void loop() {
  
}


