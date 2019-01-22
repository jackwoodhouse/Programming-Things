#include <Wire.h>
#include <ZumoShield.h>

#define MAX_DISTANCE 400 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.
#define rev_distance 100
#define TURN_DURATION 593
#define QTR_THRESHOLD  800 // microseconds
#define NUM_SENSORS 6


ZumoMotors motors;
ZumoReflectanceSensorArray sensors;
Pushbutton button(ZUMO_BUTTON);

boolean canMove =  true;
char incomingByte;
unsigned int sensor_values[6];


void setup()
{
  // initialize serial communication:
  Serial.begin(9600);
  // initialize the LED pin as an output:
  // Initialize the reflectance sensors module
  sensors.init();
}

void loop()
{

  movement();

  if (canMove == true) {

    borderDetect();

  }

}

void movement()
{

  if (Serial.available() > 0)  // see if there's incoming serial data:
  {

    //    incomingByte = Serial.readString(); // read the serial string data into the incoming bytes variable
    //    incomingByte.trim(); // trim the string in the incomingBytes variable to remove any whitespace at the end of the string

    // read the oldest byte in the serial buffer:
    incomingByte = Serial.read();

    if (incomingByte == 'w') {
      motors.setSpeeds(50, 50);
    }

    if (incomingByte == 's') {
      motors.setSpeeds(-50, -50);
    }

    if (incomingByte == 'a') {
      motors.setSpeeds(-170, 170);
      delay(TURN_DURATION);
      motors.setSpeeds(0, 0);
    }

    if (incomingByte == 'd') {
      motors.setSpeeds(170, -170);
      delay(TURN_DURATION);
      motors.setSpeeds(0, 0);
    }

    if (incomingByte == 'x') {
      motors.setSpeeds(0, 0);
    }

    if (incomingByte == 'c') {
      Serial.println("action complete!");
      Serial.println("resuming!");
      delay(250);
      canMove = true;
    }

    if (incomingByte == "ro r") {

      Serial.println ("entering a room on the right!");
      delay(250);
      motors.setSpeeds(170, -170);
      delay(TURN_DURATION);
      motors.setSpeeds(0, 0);

    }

    if (incomingByte == "ro l") {

      Serial.println ("entering a room on the left!");
      delay(250);
      motors.setSpeeds(-170, 170);
      delay(TURN_DURATION);
      motors.setSpeeds(0, 0);

      searchRoom();

    }

  }

}

void searchRoom()
{

  Serial.println("searching room");

  //add to a list

}

void borderDetect()
{

  sensors.read(sensor_values);
  
  if ((sensor_values[0] > QTR_THRESHOLD && sensor_values[5] > QTR_THRESHOLD) || (sensor_values[0] > QTR_THRESHOLD && sensor_values[1] > QTR_THRESHOLD) || (sensor_values[4] > QTR_THRESHOLD && sensor_values[5] > QTR_THRESHOLD))
  {
    canMove = false;
    motors.setSpeeds(0, 0);
    Serial.println("You hit a wall!");
    Serial.println("Please enter a command");
    motors.setSpeeds(-50, -50);
    delay(300);
    motors.setSpeeds(0, 0);
    movement();

  }
  else
  {
    delay(50);
    if (sensor_values[0] > QTR_THRESHOLD)
    {
      motors.setSpeeds(0, 0);
      delay(500);
      motors.setSpeeds(150, 0);
      delay(250);

    }
    else if (sensor_values[5] > QTR_THRESHOLD)
    {
      motors.setSpeeds(0, 0);
      delay(500);
      motors.setSpeeds(0, 150);
      delay(250);
    }
  }

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
