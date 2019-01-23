#include <NewPing.h>
#include <Wire.h>
#include <ZumoShield.h>


#define TRIGGER_PIN  2  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN     6  // Arduino pin tied to echo pin on the ultrasonic sensor.
#define MAX_DISTANCE 25 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.

#define QTR_THRESHOLD 800 // microseconds
#define NUM_SENSORS 6


ZumoMotors motors;

ZumoReflectanceSensorArray sensors(QTR_NO_EMITTER_PIN);

Pushbutton button(ZUMO_BUTTON);

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);

int scan = sonar.ping_cm();

boolean found = false;

String Input;


boolean canMove = true;
unsigned int sensor_values[NUM_SENSORS];


void setup()
{
  // initialize serial communication:
  Serial.begin(9600);

  // Initialize the reflectance sensors module
  sensors.init();

  //sensors.calibrate();

  button.waitForButton();
}

void loop()
{

  movement();

  if (canMove == true) {

    motors.setSpeeds(100, 100);
    borderDetect();

  }

}

void movement()
{

  if (Serial.available() > 0)  // see if there's incoming serial data:
  {

    Input = Serial.readString(); // read the serial string data into the incoming bytes variable
    Input.trim(); // trim the string in the incomingBytes variable to remove any whitespace at the end of the string


    if (Input == "w") {
      motors.setSpeeds(100, 100);
    }

    if (Input == "s") {
      motors.setSpeeds(-100, -100);
    }

    if (Input == "a") {
      motors.setSpeeds(-100, 100);

    }

    if (Input == "d") {
      motors.setSpeeds(100, -100);
    }

    if (Input == "x") {

      motors.setSpeeds(0, 0);
    }

    if (Input == "c") {
      Serial.println("action complete!");
      Serial.println("resuming!");
      delay(250);
      canMove = true;
    }

    if (Input == "ro r") {

      Serial.println ("entering a room on the right!");
      delay(250);
      motors.setSpeeds(150, -100);
      delay(250);
      motors.setSpeeds(0, 0);

      searchRoom();

    }

    if (Input == "ro l") {

      Serial.println ("entering a room on the left!");
      delay(250);
      motors.setSpeeds(-100, 150);
      delay(250);
      motors.setSpeeds(0, 0);

      searchRoom();

    }

  }
}


void searchRoom()
{

  Serial.println("searching room");

  delay(250);

  motors.setSpeeds(100, 100);

  delay(250);

  motors.setSpeeds(0, 0);

  motors.setSpeeds(-100, 150);

  delay(250);
  motors.setSpeeds(0, 0);

  for (int i = 0; i < 80; i++)
  {
    if ((i > 10 && i <= 30) || (i > 50 && i <= 70))
      motors.setSpeeds(-200, 200);
    else
      motors.setSpeeds(200, -200);
    if (sonar.ping_cm() > 0)
    {
      found = true;
    }
    motors.setSpeeds(0,0);
  }

   movement();
   
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
      motors.setSpeeds(-100, -100);
      delay(200);
      motors.setSpeeds(0, 0);
      movement();

    }
    else
    {

      delay(50);

      if (sensor_values[0] > QTR_THRESHOLD)
      {
        motors.setSpeeds(0, 0);
        delay(250);
        motors.setSpeeds(150, 0);
        delay(250);
      }
      else if (sensor_values[5] > QTR_THRESHOLD)
      {
        motors.setSpeeds(0, 0);
        delay(250);
        motors.setSpeeds(0, 150);
        delay(250);
      }
    }

  }
