#include <NewPing.h> //Include for the ulrasonic sensor
#include <Wire.h> //Include for communication between zumo and ardunio
#include <ZumoShield.h> //Include for access to zumo functions such as motors and the reflectance array


#define TRIGGER_PIN  6  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN     2  // Arduino pin tied to echo pin on the ultrasonic sensor.
#define MAX_DISTANCE 30 // Maximum distance we want to ping for (in centimeters).

#define QTR_THRESHOLD 800 // microseconds
#define NUM_SENSORS 6 //Set number of reflectance sensors

ZumoMotors motors; //declaration of motors variable

ZumoReflectanceSensorArray sensors(QTR_NO_EMITTER_PIN); //variable for the reflectance sensor

Pushbutton button(ZUMO_BUTTON); //declaration variable for the start button on the zumo

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); //sensor setup with pins and max distance 

bool canMove = true; // variable that allows for borderdetect and movement to be activated and deactivated 

int endCount = 0; // count to tell the zumo that it has reached each end of the tjunction in my loop

int Input; // a variable to read incoming serial data into

unsigned int sensor_values[NUM_SENSORS]; //Array  that holds the sensor values readings from ZumoReflectanceSensorArray

int roomNumber = 0; // integer variable to store the room number

bool ignore = false; // bool to ignore commands 

bool room = false;  //lets me tell a function if a room as been found or not 

bool tJunction = false; //tells the zumo if it has passed the junction or not


String roomAndNumbers[10]; // an array to hold the rooms and locations
String roomsAndObjects[10]; // an array to hold the rooms with object


void setup() //list of functions that only need to be called once per run of the program
{
  // initialize serial communication:
  Serial.begin(9600);

  // Initialize the reflectance sensors module
  sensors.init(QTR_NO_EMITTER_PIN);
  // Calibrate Sensor
  sensors.calibrate(QTR_NO_EMITTER_PIN);

  //the zumo waits for button press before starting the program
  button.waitForButton();
}

void loop() // list of functions that need to be called numerous times during the program
{
  
  movement(); // calls the movement loop so the user can input commands 
  
  if (canMove == true) { // if the robot can move, automatically move forward and check the sensors to kepp inbounds 

    motors.setSpeeds(100, 100); // set both motors to move forward
    borderDetect(); //call the function that check the sensors to keep the robot inbounds or stop at a corner

  }
   
}

void movement() // this functions takes the commands from the GUI and moves the robot in given direction
{

  if (Serial.available() > 0)
  {
    // read the oldest byte in the serial buffer:
    Input = Serial.read();
    
    if (Input == 'w') //if the users types w or presses the forward button move the zumo
    {
      motors.setSpeeds(150, 150); // sets both motors to 150 to move forward 
    }

    if (Input == 's') //if the users types s or presses the forward button move the zumo
    {

      if (!ignore) // this checks if the robot is ignoring commands or not
      {
        motors.setSpeeds(-150, -150); // if the robot is not ignoring commands then reverse the robot 
      }

    }

    if (Input == 'a') //if the users types a or presses the forward button move the zumo
    {

      if (!ignore) // this checks if the robot is ignoring commands or not
      {
        motors.setSpeeds(-150, 150); // if the robot is not ignoring commands then turn the robot left 
      }
      else
      {
        Serial.println("ignoring commands!"); // if the robot is ignoring commands print this line to the GUI
        tJunction = true; //tell the zumo that it has passed the tjunction 
        ignore = false; // stop ignoring this command
        canMove = true; //allow it to move forward and keep checking the borderdectect
      }

    }

    if (Input == 'd') //if the users types d or presses the forward button move the zumo 
    {
      if (!ignore) // this checks if the robot is ignoring commands or not
      {
        motors.setSpeeds(150, -150); // if the robot is not ignoring commands then turn the robot right 
      }
      else
      {
        Serial.println("ignoring commands!"); // if the robot is ignoring commands print this line to the GUI
        tJunction = true; //tell the zumo that it has passed the tjunction 
        ignore = false; // stop ignoring this command
        canMove = true; //allow it to move forward and keep checking the borderdectect
      }

    }

    if (Input == 'x') //if the users types x or presses the forward button stop the zumo
    {
      motors.setSpeeds(0,0); //stops the robot if commands is entered from the gui 
    }

    if (Input == 'c') // if the user types c it tells the zumo that it can carry on with auto movement
    {
      Serial.println("Action complete!"); // Prints this to the GUI for the user
      Serial.println("Resuming!");  // Prints this to the GUI for the user
      delay(250); // A small wait before continuing 
      canMove = true; // allows the robot to move forward and restart border checking
    }

    if (Input == 'r') { // turns the robot right and start checking a room

      if (!ignore) // this checks if the robot is ignoring commands or not
      {
        Serial.println ("Entering a room on the right!");  // Prints this to the GUI for the user
        delay(250); //A small wait before continuing
        motors.setSpeeds(150, -150); // turns the zumo right into the room
        delay(250); //A small wait before continuing
        motors.setSpeeds(0, 0); // stops the zumo from moving 
        searchRoom("right"); //sends a string to the function to be used later
      }
      else {
        ignore = false; //set the bool to false so that it will accept the next r command
        Serial.println("ignoring Commands!"); // Prints this to the GUI for the user
        room = true; //sets room bool to true so it knows that it shouldnt enter but should store the command
      }
    }

    if (Input == 'l') { // turns the robot left and start checking a room

      if (!ignore) // this checks if the robot is ignoring commands or not
      {
        Serial.println ("Entering a room on the left!"); // turns the robot right and start checking a room
        delay(250); //A small wait before continuing
        motors.setSpeeds(-150, 150);  // turns the zumo left into the room
        delay(250); //A small wait before continuing
        motors.setSpeeds(0, 0);  // stops the zumo from moving 
        searchRoom("left"); //sends a string to the function to be used later
      }
      else {
        ignore = false; //set the bool to false so that it will accept the next l command
        Serial.println("ignoring Commands!"); // Prints this to the GUI for the user
        room = true; //sets room bool to true so it knows that it shouldnt enter but should store the command
      }

    }

    if (Input == 'e') // if the user enters e then it tells the zumo it has reached an end of the tjunction 
    {
      endCount++; //increments the count so that it knows if it is at the first or second end of the tjunction
      Serial.println("You hit an end!"); // Prints this to the GUI for the user

      while (!canMove && (endCount != 2)) // if the robot can move and is not at the second end of the tjunction keep moving
      {
        movement(); //allows the user to enter commands to search the rooms or stop
      }

      if (!room && !tJunction) // check if the robot has not passed a room and tjunction 
      {
        ignore = true; // if it hasent yet passed both a room or junction keep ignoring commands 
      }
      else
      {
        ignore = false; // if the room and junction has been passed allow for commands to be accepted 
      }

      if (endCount == 2) // if the zumo has reached both ends of the junction 
      {

        for (String roomAndNumber : roomAndNumbers) // for each room and its number in the array
        {
          if (roomAndNumber.length() > 0)
          {
            Serial.println(roomAndNumber); //print out the elemtents to the GUI 
          }
        }

        for (String roomWithObject : roomsAndObjects) //for each room with a object in the room 
        {
          if (roomWithObject.length() > 0)
          {
            Serial.println(roomWithObject); //print out the elements to GUI
          }
        }
      }
    }
  }
}

void borderDetect() // the main function that keeps the zumo in the maze and stops at an end e.g. a corner or a junction 
{

  sensors.read(sensor_values); // read the sensor values to be used to either move or stop the zumo 

  //if the outer sensors, left sensors or right sensors decect a light reading over the threshold stop the zumo
  if ((sensor_values[0] > QTR_THRESHOLD && sensor_values[5] > QTR_THRESHOLD) || (sensor_values[0] > QTR_THRESHOLD && sensor_values[1] > QTR_THRESHOLD) || (sensor_values[4] > QTR_THRESHOLD && sensor_values[5] > QTR_THRESHOLD))
  {

    canMove = false; // stop executing the code to move the zumo and stop dececting the border 
    motors.setSpeeds(0, 0); // stop the zumo
    Serial.println("You hit a wall!"); // Prints this to the GUI for the user
    Serial.println("Please enter a command"); // Prints this to the GUI for the user
    motors.setSpeeds(-150, -150); // slightly reverse the zumo away from the wall 
    delay(200); //A small wait before continuing
    motors.setSpeeds(0, 0); // stop the zumo 
    movement(); // call the movement function that allows the user to turn the robot and then continue the maze 

  }
  else
  {
    delay(50); //A small wait before continuing
    
    if (sensor_values[0] > QTR_THRESHOLD) // if the outer left sensor decects a line then slightly adjust the zumo inwards 
    {
      motors.setSpeeds(0, 0); // stop the zumo continuing 
      delay(500); //A small wait before continuing
      motors.setSpeeds(150, -50); //turn the robot right towards the middle of the corridor 
      delay(250); //A small wait before continuing
    }
    else if (sensor_values[5] > QTR_THRESHOLD) // if the outer right sensor decects a line then slightly adjust the zumo inwards 
    {
      motors.setSpeeds(0, 0); // stop the zumo continuing 
      delay(500); //A small wait before continuing
      motors.setSpeeds(-50, 150); //turn the robot left  towards the middle of the corridor 
      delay(250); //A small wait before continuing
    }
  }
}


void searchRoom(String location) // a function that searches the room for an object and takes the left or right string and stores it in location 
{

  canMove = false; // stops the robot moving and dectecting the borders 
  bool found = false; // create a set a bool to tell the robot if it has found an object in the room or not 

  roomNumber++; // increment the room number each time the function is called 

  String RoomNumber = String(roomNumber); // convert the room number to a string

  Serial.println("Room:  " + RoomNumber + "\n located on the: " + location + "\n"); // print the room number and what side of the corridor it is on 

  storeRoomLocations(roomNumber, location); //Zumo retains the room numbers and locations

  Serial.println("Searching room"); // Prints this to the GUI for the user


  for (int i = 0; i < 80; i++) // for 80 repetitions of the for loop move the zumo left and right
  {
    if ((i > 10 && i <= 30) || (i > 50 && i <= 70)) // if i is greater than 40 and i is less than or equal to 60 or i is greater than 80 or less than or equal to 100
    {
      motors.setSpeeds(-150, 150); // move the zumo left
    }
    else
    {
      motors.setSpeeds(150, -150); // move the zumo right
    }
    
    if (sonar.ping_cm() > 0) // if the ultrasonic sensor has a reading in centimetres greater than 0
    {
      found = true; // set found to true so that it can store the object location 
      motors.setSpeeds(0, 0); // stop moving the zumo
    }
    delay(50);
  }
  
  motors.setSpeeds(0, 0); // stop the zumo moving 

  if (found) // if the zumo has dectected an object in the room it just searched 
  {
    Serial.println("Object detected in room: " + RoomNumber + "\n"); // Prints this to the GUI for the user  
    storeObject(roomNumber); //store that the zumo has found an object and where 
  }
  else {
    Serial.println("No object found in this room!"); // if the zumo doesnt find an object after the rotation has finished then tell the user 
  }
  
}

void storeObject(int roomNumber) // take the room number from the roomSearch function 
{
  String strRoomNumber = String(roomNumber); //convert the room number to a string 
  String text = "Room: " + strRoomNumber + " Object Detected"; // Prints this to the GUI for the user 
  roomsAndObjects[roomNumber - 1] = text; // add the object and its location to an array
}

void storeRoomLocations(int roomNumber, String location) // take the room number and its location on the map from the roomSearch function
{
  String strRoomNumber = String(roomNumber); //convert the room number to a string 
  String text = "Room: " + strRoomNumber + " Location: " + location + ""; // Prints this to the GUI for the user 
  roomAndNumbers[roomNumber - 1] = text;  // add the room and its location to the array
}
