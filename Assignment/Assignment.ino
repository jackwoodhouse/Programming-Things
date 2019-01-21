
#include <NewPing.h>
#include <Wire.h>
#include <ZumoShield.h>


#define TRIGGER_PIN  6  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN     2  // Arduino pin tied to echo pin on the ultrasonic sensor.
#define MAX_DISTANCE 400 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.
#define rev_distance 100
#define TURN_DURATION 593

// this might need to be tuned for different lighting conditions, surfaces, etc.
#define QTR_THRESHOLD  1000 // microseconds
#define NUM_SENSORS 6

ZumoBuzzer buzzer;
ZumoReflectanceSensorArray reflectanceSensors;
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); // NewPing setup of pins and
ZumoMotors motors;
const int ledPin = 13; // the pin that the LED is attached to
int incomingByte;      // a variable to read incoming serial data into

boolean canMove =  true;

//SENSORS

unsigned int sensor_values[NUM_SENSORS];
ZumoReflectanceSensorArray sensors(QTR_NO_EMITTER_PIN);


void setup() {
  // initialize serial communication:
  Serial.begin(9600);
  // initialize the LED pin as an output:
  pinMode(ledPin, OUTPUT);
  reflectanceSensors.init();

}

void loop() {

  movement();

  if (canMove == true) {

    borderDetect();

  }

}


void movement() {

  // see if there's incoming serial data:

  if (Serial.available() > 0)
  {
    // read the oldest byte in the serial buffer:
    incomingByte = Serial.read();

    if (incomingByte == 'w') {
      motors.setSpeeds(50, 50);
    }

    if (incomingByte == 's') {
      motors.setSpeeds(-150, -150);
    }
    if (incomingByte == 'a') {
      motors.setSpeeds(-150, 150);
      delay(TURN_DURATION);
      motors.setSpeeds(0, 0);
      canMove = true;
    }

    if (incomingByte == 'd') {
      motors.setSpeeds(170, -170);
      delay(TURN_DURATION);
      motors.setSpeeds(0, 0);
      canMove = true;
    }
    //STOP ZUMO

    if (incomingByte == 'x') {
      motors.setSpeeds(0, 0);
    }

    if (incomingByte == 'c') {
      canMove = true;
    }

  }

}

void borderDetect()
{
  sensors.read(sensor_values);

  if (sensor_values[2] > QTR_THRESHOLD && sensor_values[3] > QTR_THRESHOLD)
  {
    
    canMove = false;
    motors.setSpeeds(0, 0);
    Serial.println("You hit a wall!");
    Serial.println("Please enter a command");
    motors.setSpeeds(-50,-50);
    delay(250);
    motors.setSpeeds(0,0);
    movement();
    
  }

  
//  else
//  {
//    if (sensor_values[0] > QTR_THRESHOLD)
//    {
//      motors.setSpeeds(150, 0);
//      delay(250);
//      motors.setSpeeds(50, 50);
//      delay(250);
//    }
//    else if (sensor_values[5] > QTR_THRESHOLD)
//    {
//      motors.setSpeeds(0, 150);
//      delay(250);
//      motors.setSpeeds(50, 50);
//      delay(250);
//    }
//  }

}


//delay(250);                     // Wait 50ms between pings (about 20 pings/sec). 29ms should be the shortest delay between pings.
//  Serial.print("Ping: ");
//  Serial.print(sonar.ping_cm()); // Send ping, get distance in cm and print result (0 = outside set distance range)
//  Serial.println("cm");

//  if(sonar.ping() < MAX_DISTANCE){
//    Serial.print("CLOSE");
//    motors.setSpeeds(-100, -100);
//  }
//  else {
//    motors.setSpeeds(100,100);
//  }

//  Serial.print("Ping: ");
//  Serial.print(sonar.ping_cm()); // Send ping, get distance in cm and print result (0 = outside set distance range)
//  Serial.println("cm");

//  if(sonar.ping() < MAX_DISTANCE){
//    Serial.print("CLOSE");
//    motors.setSpeeds(-100, -100);
//  }
//  else {
//    motors.setSpeeds(100,100);
//  }
