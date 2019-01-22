#include <Wire.h>
#include <ZumoShield.h>

#define QTR_THRESHOLD 1000 // microseconds
#define NUM_SENSORS 6


ZumoMotors motors;
ZumoReflectanceSensorArray sensors(QTR_NO_EMITTER_PIN);
Pushbutton button(ZUMO_BUTTON);

boolean canMove =  true;
String Input;
unsigned int sensor_values[NUM_SENSORS];


void setup()
{
  // initialize serial communication:
  Serial.begin(9600);

  // Initialize the reflectance sensors module
  sensors.init();
  sensors.calibrate();

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
      motors.setSpeeds(100, -50);
      motors.setSpeeds(0, 0);

    }

    if (Input == "ro l") {

      Serial.println ("entering a room on the left!");
      delay(250);
      motors.setSpeeds(-50, 100);
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
