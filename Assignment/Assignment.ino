#include <NewPing.h>
#include <Wire.h>
#include <ZumoShield.h>


#define TRIGGER_PIN  6  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN     2  // Arduino pin tied to echo pin on the ultrasonic sensor.
#define MAX_DISTANCE 30 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.

#define QTR_THRESHOLD 800 // microseconds
#define NUM_SENSORS 6

ZumoMotors motors;

ZumoReflectanceSensorArray sensors(QTR_NO_EMITTER_PIN);

Pushbutton button(ZUMO_BUTTON);

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);

bool canMove = true;

int endCount = 0;

int Input; // a variable to read incoming serial data into

unsigned int sensor_values[NUM_SENSORS];

int roomNumber = 0; // integer variable to store the room number

bool ignore = false;

bool room = false;

bool tJunction = false;


String roomsAndLocations[10]; // an array to hold the rooms and locations
String roomsWithObjects[10]; // an array to hold the rooms with object


void setup()
{
  // initialize serial communication:
  Serial.begin(9600);

  // Initialize the reflectance sensors module
  sensors.init(QTR_NO_EMITTER_PIN);

  sensors.calibrate(QTR_NO_EMITTER_PIN);

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

  if (Serial.available() > 0)
  {
    // read the oldest byte in the serial buffer:
    Input = Serial.read();

    if (Input == 'w')
    {
      motors.setSpeeds(150, 150);
    }

    if (Input == 's')
    {

      if (!ignore)
      {
        motors.setSpeeds(-150, -150);
      }

    }

    if (Input == 'a')
    {

      if (!ignore)
      {
        motors.setSpeeds(-150, 150);
      }
      else
      {
        Serial.println("ignoring commands!");
        tJunction = true;
        ignore = false;
        canMove = true;
      }

    }

    if (Input == 'd')
    {
      if (!ignore)
      {
        motors.setSpeeds(150, -150);
      }
      else
      {
        Serial.println("ignoring commands!");
        tJunction = true;
        ignore = false;
        canMove = true;
      }

    }

    if (Input == 'x')
    {
      motors.setSpeeds(0,0);
    }

    if (Input == 'c')
    {
      Serial.println("Action complete!");
      Serial.println("Resuming!");
      delay(250);
      canMove = true;
    }

    if (Input == 'r') {

      if (!ignore)
      {
        Serial.println ("Entering a room on the right!");
        delay(250);
        motors.setSpeeds(150, -150);
        delay(250);
        motors.setSpeeds(0, 0);
        searchRoom("right");
      }
      else {
        ignore = false;
        Serial.println("ignoring Commands!");
        room = true;

      }
    }

    if (Input == 'l') {

      if (!ignore)
      {
        Serial.println ("Entering a room on the left!");
        delay(250);
        motors.setSpeeds(-150, 150);
        delay(250);
        motors.setSpeeds(0, 0);
        searchRoom("left");
      }
      else {
        
        ignore = false;
        Serial.println("ignoring Commands!");
        room = true;
      }

    }

    if (Input == 'e')
    {
      endCount++;
      Serial.println("You hit an end!");

      while (!canMove && (endCount != 2))
      {
        movement();
      }

      if (!room && !tJunction)
      {
        ignore = true;
      }
      else
      {
        ignore = false;
      }

      if (endCount == 2)
      {

        for (String roomAndLocation : roomsAndLocations)
        {
          if (roomAndLocation.length() > 0)
          {
            Serial.println(roomAndLocation);
          }
        }

        for (String roomWithObject : roomsWithObjects)
        {
          if (roomWithObject.length() > 0)
          {
            Serial.println(roomWithObject);
          }
        }
      }
    }
  }
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
    motors.setSpeeds(-150, -150);
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
      delay(500);
      motors.setSpeeds(150, -50);
      delay(250);
    }
    else if (sensor_values[5] > QTR_THRESHOLD)
    {
      motors.setSpeeds(0, 0);
      delay(500);
      motors.setSpeeds(-50, 150);
      delay(250);
    }
  }
}


void searchRoom(String location)
{

  canMove = false;
  bool found = false;

  roomNumber++; // increment the room number

  String strRoomNumber = String(roomNumber); // convert the room number to a string

  Serial.println("Room:  " + strRoomNumber + "\n located on the: " + location + "\n"); //number the room and state whether on the left or right of the room in the GUI

  storeRoomLocations(roomNumber, location); //Zumo retains the room numbers and locations

  Serial.println("Searching room");


  for (int i = 0; i < 80; i++)
  {
    if ((i > 10 && i <= 30) || (i > 50 && i <= 70)) // if i is greater than 40 and i is less than or equal to 60 or i is greater than 80 or less than or equal to 100
    {
      motors.setSpeeds(-150, 150);
    }
    else
    {
      motors.setSpeeds(150, -150); // set the left motor to a speed of 150 forwards and the right to 150 in reverse to turn the Zumo right
    }
    
    if (sonar.ping_cm() > 0) // if the ultrasonic sensor has a reading in centimetres greater than 0
    {
      found = true;
      motors.setSpeeds(0, 0);
    }
    delay(50);
  }
  
  motors.setSpeeds(0, 0);

  if (found)
  {
    Serial.println("Object detected in room: " + strRoomNumber + "\n");
    storeObjectDetected(roomNumber); //store room number and object
  }
  else {
    Serial.println("No object found in this room!");
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
