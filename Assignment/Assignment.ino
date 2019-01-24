#include <NewPing.h>
#include <Wire.h>
#include <ZumoShield.h>

#define TRIGGER_PIN  2  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN     6  // Arduino pin tied to echo pin on the ultrasonic sensor.
#define MAX_DISTANCE 50 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.

#define FORWARD_SPEED 150
#define REVERSE_SPEED -150
#define STOP 0

#define QTR_THRESHOLD 800 // microseconds
#define NUM_SENSORS 6


ZumoMotors motors;

ZumoReflectanceSensorArray sensors(QTR_NO_EMITTER_PIN);

Pushbutton button(ZUMO_BUTTON);

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);

bool anEnd = false;
bool canMove = true;

int Input;      // a variable to read incoming serial data into

unsigned int sensor_values[NUM_SENSORS];

int roomNumber = 0; // integer variable to store the room number

String roomsAndLocations[5]; // an array to hold the rooms and locations
String roomsWithObjects[5]; // an array to hold the rooms with object


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

  if (canMove == true) {

    motors.setSpeeds(FORWARD_SPEED, FORWARD_SPEED);
    borderDetect();

  }

  if (anEnd == false ) {
    movement();
  }

}

void movement()
{
 
  int endCount = 0;

  if (Serial.available() > 0) 
  {
    // read the oldest byte in the serial buffer:
    Input = Serial.read();
    //input.trim(); // trim the string in the incomingBytes variable to remove any whitespace at the end of the string


    if (Input == 'w') {
      motors.setSpeeds(FORWARD_SPEED, FORWARD_SPEED);
    }

    if (Input == 's') {
      motors.setSpeeds(REVERSE_SPEED, REVERSE_SPEED);
    }

    if (Input == 'a') {
      motors.setSpeeds(REVERSE_SPEED, FORWARD_SPEED);

    }

    if (Input == 'd') {
      motors.setSpeeds(FORWARD_SPEED, REVERSE_SPEED);
    }

    if (Input == 'x') {

      motors.setSpeeds(STOP, STOP);
    }

    if (Input == 'c') {
      Serial.println("action complete!");
      Serial.println("resuming!");
      delay(250);
      canMove = true;
    }

    if (Input == 'e') {
      ++endCount;
      motors.setSpeeds(REVERSE_SPEED, FORWARD_SPEED);
      delay(350);
      anEnd = true;
      delay(250);
      motors.setSpeeds(FORWARD_SPEED, FORWARD_SPEED);
      delay(1000);
      anEnd = false;

      if (endCount == 2) {
        Serial.println ("thats the end of the maze!");
        canMove = false;
        motors.setSpeeds(FORWARD_SPEED, FORWARD_SPEED);
        delay(250);
        motors.setSpeeds(STOP, STOP);
        //maybe play a tune to signal end of the maze.
      }

    }

    if (Input == 'r') {

      Serial.println ("entering a room on the right!");
      delay(250);
      motors.setSpeeds(FORWARD_SPEED, REVERSE_SPEED);
      delay(350);
      motors.setSpeeds(STOP, STOP);
      searchRoom("right");
    }

    if (Input == 'l') {
      Serial.println ("entering a room on the left!");
      delay(250);
      motors.setSpeeds(REVERSE_SPEED, FORWARD_SPEED);
      delay(350);
      motors.setSpeeds(STOP, STOP);
      searchRoom("left");
    }

  }
}

void borderDetect()
{

  sensors.read(sensor_values);

  if ((sensor_values[0] > QTR_THRESHOLD && sensor_values[5] > QTR_THRESHOLD) || (sensor_values[0] > QTR_THRESHOLD && sensor_values[1] > QTR_THRESHOLD) || (sensor_values[4] > QTR_THRESHOLD && sensor_values[5] > QTR_THRESHOLD))
  {
    canMove = false;
    motors.setSpeeds(STOP, STOP);

    Serial.println("You hit a wall!");
    Serial.println("Please enter a command");
    motors.setSpeeds(-REVERSE_SPEED, -REVERSE_SPEED);
    delay(200);
    motors.setSpeeds(STOP, STOP);
    movement();

  }
  else
  {
    delay(50);
    if (sensor_values[0] > QTR_THRESHOLD)
    {
      motors.setSpeeds(STOP, STOP);
      delay(250);
      motors.setSpeeds(FORWARD_SPEED, -50);
      delay(250);
    }
    else if (sensor_values[5] > QTR_THRESHOLD)
    {
      motors.setSpeeds(STOP, STOP);
      delay(250);
      motors.setSpeeds(-50, FORWARD_SPEED);
      delay(250);
    }
  }
}


void searchRoom(String location)
{

  bool found = false;
  
  roomNumber++; // increment the room number
  
  String strRoomNumber = String(roomNumber); // convert the room number to a string
  
  Serial.println("Here is room number " + strRoomNumber + " and is located on the " + location + "\r\n"); //number the room and state whether on the left or right of the room in the GUI

  storeRoomLocations(roomNumber, location); //Zumo retains the room numbers and locations

  Serial.println("searching room");

  delay(250);

  motors.setSpeeds(FORWARD_SPEED, FORWARD_SPEED);

  delay(250);

  motors.setSpeeds(REVERSE_SPEED, FORWARD_SPEED);

  delay(500);
  
  motors.setSpeeds(STOP, STOP);

  for (int i = 0; i < 100; i++)
  {
    
    if ((i > 40 && i <= 60) || (i > 80 && i <= 100)) // if i is greater than 40 and i is less than or equal to 60 or i is greater than 80 or less than or equal to 100 
      motors.setSpeeds(REVERSE_SPEED, FORWARD_SPEED); 
    else
      motors.setSpeeds(FORWARD_SPEED, REVERSE_SPEED); // set the left motor to a speed of 150 forwards and the right to 150 in reverse to turn the Zumo right
      
    if (sonar.ping_cm() > 0) // if the ultrasonic sensor has a reading in centimetres greater than 0
    {
      found = true;
      motors.setSpeeds(STOP, STOP);
    }
    delay(50);
  }
  motors.setSpeeds(STOP, STOP);
  
  if (found)
  {
    Serial.println("Object detected in room " + strRoomNumber + "\r\n");
    storeObjectDetected(roomNumber); //store room number and object
  }
  else {
    Serial.println("no object found in this room!");
  }
}

void storeObjectDetected(int roomNumber)
{
  String strRoomNumber = String(roomNumber); // convert the room number to a string
  String text = "Room: " + strRoomNumber + " Object Detected"; // create a string with the room number and an object has been detected
  roomsWithObjects[roomNumber - 1] = text; // add the string of room and object detected to the array of rooms with objects detected
}

void storeRoomLocations(int roomNumber, String location)
{
  String strRoomNumber = String(roomNumber); // convert the room number to a string
  String text = "Room: " + strRoomNumber + " Location: " + location + ""; // create a string with the room number and location
  roomsAndLocations[roomNumber - 1] = text; // add the string of room and location to the array of rooms and locations
}
