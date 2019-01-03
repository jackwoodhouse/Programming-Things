/*
  Physical Pixel

  An example of using the Arduino board to receive data from the computer. In
  this case, the Arduino boards turns on an LED when it receives the character
  'H', and turns off the LED when it receives the character 'L'.

  The data can be sent from the Arduino Serial Monitor, or another program like
  Processing (see code below), Flash (via a serial-net proxy), PD, or Max/MSP.

  The circuit:
  - LED connected from digital pin 13 to ground

  created 2006
  by David A. Mellis
  modified 30 Aug 2011
  by Tom Igoe and Scott Fitzgerald

  This example code is in the public domain.

  http://www.arduino.cc/en/Tutorial/PhysicalPixel
*/
#include <NewPing.h>
#include <Wire.h>
#include <ZumoShield.h>

#define TRIGGER_PIN  6  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN     2  // Arduino pin tied to echo pin on the ultrasonic sensor.
#define MAX_DISTANCE 400 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.
#define rev_distance 100


// this might need to be tuned for different lighting conditions, surfaces, etc.
#define QTR_THRESHOLD  1000 // microseconds
#define NUM_SENSORS 6

ZumoBuzzer buzzer;
ZumoReflectanceSensorArray reflectanceSensors;
ZumoMotors motors;
Pushbutton button(ZUMO_BUTTON);

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); // NewPing setup of pins and maximum distance.

const int ledPin = 13; // the pin that the LED is attached to
int incomingByte;      // a variable to read incoming serial data into

boolean canMove;

//SENSORS

unsigned int sensor_values[NUM_SENSORS];
ZumoReflectanceSensorArray sensors(QTR_NO_EMITTER_PIN);


void setup() {
  // initialize serial communication:
  Serial.begin(9600);
  canMove = true;
  reflectanceSensors.init();

}

void loop() {


  while(canMove = true){  
    
  borderDetect();
  
  }

  delay(250);
  
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
      motors.setSpeeds(150, -150);
    }
    if (incomingByte == 'd') {
      motors.setSpeeds(-150, 150);
    }
    //STOP ZUMO
    if (incomingByte == 'x') {
      motors.setSpeeds(0, 0);
    }

  }

}


void borderDetect()
{

  sensors.read(sensor_values);

  if (sensor_values[2] > QTR_THRESHOLD)
  {
    motors.setSpeeds(0, 0);
    Serial.println("hit a wall!");
    canMove = false;
  }
  else if (sensor_values[3] > QTR_THRESHOLD)
  {
    motors.setSpeeds(0, 0);
    Serial.println("hit a wall!");
    canMove = false;
  }
  else {
    //nothing yet
  }


  if (sensor_values[0] > QTR_THRESHOLD)
  {

    motors.setSpeeds(100, 0);
    delay(250);
    motors.setSpeeds(50, 50);

  }
  else if (sensor_values[5] > QTR_THRESHOLD)
  {

    motors.setSpeeds(0, 100);
    delay(250);
    motors.setSpeeds(50, 50);

  }
  else {
    // nothing yet
  }

}
