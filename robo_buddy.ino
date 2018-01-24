
/**
 * robo_buddy.ino
 *
 * Program for my cheap robot. He looks around for slim profiled objects (like a pencil)
 * and slams into them comically. He's not interested in wide-profiled object (like flower
 * pots).
 *
 * @author (Khalil Stemmler)
 */

#include "Ultrasonic.h"
#include <Wire.h>
#include <Servo.h>

Ultrasonic ultrasonic(trig, echo);

/*
 * Constants
 */

const int AIN1 = 12;          // Motor Direction Pin - AIN1
const int AIN2 = 13;          // Motor Direction Pin - AIN2
const int BIN1 = 2;           // Motor Direction Pin - BIN1
const int BIN2 = 3;           // Motor Direction Pin - BIN2

const int PWM_A = 5;          // Control Pins - Output - Pulse Width Modulation A
const int PWM_B = 6;          // Control Pins - Output - Pulse Width Modulation B

const int servoPWM = 11;      // Servo (Scanner) - Pulse Width Modulation Input

const int trig = 7;           // Rangefinder - Input - Trig
const int echo = 8;           // Rangefinder - Input - Echo

const String BACKWARD = "BACKWARD";
const String FORWARD = "FORWARD";

const int DISTANCE_THRESH = 60;

/*
 * Global Variables
 */

int currentWallRange;         
int currentSpeed = 30;  
bool isCentered = false;          

Servo myservo;                  // create servo object to control a servo

/**
 * Initializes the pins on the board.
 * @return {void}
 */

void setup() {
  Wire.begin();
  Serial.begin(9600);

  // Set PWM output 
  pinMode(PWM_A, OUTPUT);
  pinMode(PWM_B, OUTPUT);

  // Set motor output pins
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT); 

  // Attach server on PIN 11 to the server object
  myservo.attach(servoPWM); 

  // Initialize backwards motion
  setDirection(FORWARD);
}

/**
 * Lifecycle of the program. 
 * @return {void}
 */

void loop() {

  // ======================================================================== //
  // =========================== MAIN LAYER ================================= //
  // ======================================================================== //


  centerServo();
  readRangeFinder();

  /*
  * First, we are just going to move the robot forward for a while until
  * it sees something that is within 10 cm.
  */

  if (currentWallRange > 10) {
    moveForward();
  }

  else {

    /*
    * Now, at this point we must be looking at an object in front of us.
    * It can be either a wall or a slim profiled object; so, we need to
    * examine it to determine what to do next.
    */

    stopMoving();

    myservo.write(40);
    delay(1000);
    int leftRange = readRangeFinder();
    delay(1000);

    myservo.write(70);
    delay(1000);
    int middleRange = readRangeFinder();
    delay(1000);

    myservo.write(100);
    delay(1000);
    int rightRange = readRangeFinder();
    delay(1000);

    centerServo();

    /*
    * If the range between the left/right and middle vary by some threshold
    * then we know we are looking at a slim profile object.
    */

    if (((leftRange - middleRange) > DISTANCE_THRESH) || ((rightRange - middleRange ) > DISTANCE_THRESH)) {

      /*
      * There appears to be a slim profiled object in front of us.
      * Let's ram it.
      */

      ramObject();

      /*
      * After ramming the object. Reverse.
      */

      stopMoving();
      setDirection(BACKWARD);

      for(int h = 0; h < 30; h++) {
        moveForward();
        delay(30);
      }

      /*
      * Rotate to another direction to look for other stuff.
      */

      setDirection(FORWARD);

      for(int k = 0; k < 90; k++) {
        moveRight();
        delay(30);
      }
    }
  }
}

void ramObject () {
  setDirection(BACKWARD);

  for(int i = 0; i < 30; i++) {
    moveForward();
    delay(30);
  }

  stopMoving();
  setDirection(FORWARD);  // set to move forward
  delay(1000);

  currentSpeed = 60;

  isCentered = false;
  centerServo();

  for(int j = 0; j < 30; j++) {
    moveForward();
    delay(30);
    if(readRangeFinder() <= 3) {
      return; 
    }
  }

  currentSpeed = 30;
}

int readRangeFinder() {
  currentWallRange = ultrasonic.Ranging(CM);
  return currentWallRange;
}

/* 
* updateMotorcurrentSpeeds
* 
* Change the currentSpeeds of the motors. This is how we achieve movement/turning
* through the robot. By writing analog writes on Pulse Width Modulation pins,
* we can set the currentSpeed of the motor anywhere from 0 -> 255.
*/

void updateMotorcurrentSpeeds (int leftcurrentSpeed, int rightcurrentSpeed) {
  analogWrite(PWM_A, leftcurrentSpeed);
  analogWrite(PWM_B, rightcurrentSpeed);
}

void moveForward() {
  updateMotorcurrentSpeeds(currentSpeed, currentSpeed);
}

void moveRight() {
  updateMotorcurrentSpeeds(currentSpeed, 0);
}

void moveLeft() {
  updateMotorcurrentSpeeds(0, currentSpeed);
}

void stopMoving() {
  updateMotorcurrentSpeeds(0,0);
}

void setDirection(String direction) {
  if (direction == BACKWARD) {
    digitalWrite(AIN1, HIGH);
    digitalWrite(AIN2, LOW);
    digitalWrite(BIN1, LOW);
    digitalWrite(BIN2, HIGH);
  } else {
    digitalWrite(AIN1, LOW);
    digitalWrite(AIN2, HIGH);
    digitalWrite(BIN1, HIGH);
    digitalWrite(BIN2, LOW);
  }
}

void centerServo() {
  if (!isCentered) {
    myservo.write(0);
    delay(2000);

    myservo.write(70);
    delay(2000);

    centered = true;
  }
}




