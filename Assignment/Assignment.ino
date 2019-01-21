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
#define TURN_DURATION 600 // ms

// this might need to be tuned for different lighting conditions, surfaces, etc.
#define QTR_THRESHOLD1  1600 // microseconds
#define QTR_THRESHOLD2  1000 // microseconds


#define NUM_SENSORS 6


//ZumoReflectanceSensorArray reflectanceSensors;
ZumoMotors motors;
String incomingBytes;

int roomNumber = 0; // integer variable to store the room number

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); // NewPing setup of pins and maximum distance.

const int ledPin = 13; // the pin that the LED is attached to
int incomingByte;      // a variable to read incoming serial data into

boolean canMove = true;

//SENSORS

unsigned int sensor_values[NUM_SENSORS];
ZumoReflectanceSensorArray sensors(QTR_NO_EMITTER_PIN);


void setup() {
  // initialize serial communication:
  Serial.begin(9600);
  sensors.init();

}

void loop() {


  if (canMove == true) {

    borderDetect();

<<<<<<< HEAD
=======
  }

  movement();

  //sensors.read(sensor_values);

>>>>>>> 0fa6ba15f40357ea0e12041df7f02a404ccd5af7
}

void movement() {

  motors.setSpeeds(50, 50);

  if (Serial.available() > 0)
  {

    //    incomingBytes = Serial.readString(); // read the serial string data into the incoming bytes variable
    //    incomingBytes.trim(); // trim the string in the incomingBytes variable to remove any whitespace at the end of the string

    // read the oldest byte in the serial buffer:

    incomingByte = Serial.read();

    if (incomingByte == 'w') { //IF DOESNT WORK CHANGE TO DOUBLE QUOTE
      motors.setSpeeds(50, 50);
    }

    else if (incomingByte == 's') {
      motors.setSpeeds(-150, -150);
    }
    else if (incomingByte == 'a') {
      motors.setSpeeds(-150, 150);
      delay(TURN_DURATION);
      motors.setSpeeds(0, 0);
    }
    else if (incomingByte == 'd') {
      motors.setSpeeds(150, -150);
      delay(TURN_DURATION);
      motors.setSpeeds(0, 0);
    }
    else if (incomingByte == 'x') {
      motors.setSpeeds(0, 0);
    }

    else if (incomingBytes == "Ro R") // else if the incomingBytes string to set to 'Ro R'
    {
      motors.setSpeeds(-150, 150); //turn right into the room
      roomNumber = roomNumber++; // increment the room number
      String strRoomNumber = String(roomNumber); // convert
      Serial.println("Here is room number" + strRoomNumber + "and is located on the right"); //number the room and state whether on the left or right of the room in the GUI
      delay(50);
      //storeRoomLocations(roomNumber, "right");
      //Zumo to retain this information
      motors.setSpeeds(150, 150);
      delay(50);
      motors.setSpeeds(0, 0);
      delay(150);
      motors.setSpeeds(150, -150);

      int pingcm = sonar.ping_cm();
      if (pingcm > 0)       //see if it detects anything
      {
        Serial.println("Object detected in room " + strRoomNumber);
        //storeObjectDetected(roomNumber);
      }
      else      //if not detected go further into the room
      {

        motors.setSpeeds(50, 50);
        delay(60);
        motors.setSpeeds(0, 0);
        if (pingcm > 0)
        {
          Serial.println("Object detected in room " + strRoomNumber);
          //storeObjectDetected(roomNumber);
        }
        else
        {
          Serial.println("Nothing detected");
        }
      }
      //Zumo to stop and wait for manual control
    }
    //
    //     else if (incomingBytes == "Ro L") // else if the incomingBytes string to set to 'Ro R'
    //    {
    //      motors.setSpeeds(150, -150); //turn left into the room
    //      roomNumber = roomNumber++; // increment the room number
    //      String strRoomNumber = String(roomNumber); // convert
    //      Serial.println("Here is room number" + strRoomNumber + "and is located on the left"); //number the room and state whether on the left or right of the room in the GUI
    //      delay(50);
    //
    //      storeRoomLocations(roomNumber, "left");
    //      //Zumo to retain this information
    //
    //      motors.setSpeeds(150,150);
    //
    //      int pingcm = sonar.ping_cm();
    //      if (pingcm > 0)       //see if it detects anything
    //      {
    //        Serial.println("Object detected in room " + strRoomNumber);
    //        storeObjectDetected(roomNumber);
    //      }
    //      else      //if not detected go further into the room
    //      {
    //
    //        motors.setSpeeds(150, -150);
    //        delay(20);
    //        motors.setSpeeds(0, 0);
    //        if (pingcm > 0)
    //        {
    //          Serial.println("Object detected in room " + strRoomNumber);
    //          storeObjectDetected(roomNumber);
    //        }
    //        else
    //        {
    //          Serial.println("Nothing detected");
    //        }
    //      }
    //      //Zumo to stop and wait for manual control
    //    }

  }

}

void borderDetect()
{
  sensors.read(sensor_values);
<<<<<<< HEAD
  
  if (sensor_values[2] > QTR_THRESHOLD || sensor_values[3] > QTR_THRESHOLD) /// TRY 2 DIFFERNT QTR VALUES
=======
  Serial.println(sensor_values[0]);
  Serial.println(sensor_values[2]);
  Serial.println(sensor_values[3]);
  Serial.println(sensor_values[5]);
  
  if (sensor_values[2] > QTR_THRESHOLD1 || sensor_values[3] > QTR_THRESHOLD1) /// TRY 2 DIFFERNT QTR VALUES
>>>>>>> 0fa6ba15f40357ea0e12041df7f02a404ccd5af7
  {

    motors.setSpeeds(0, 0);
    Serial.println("hit a wall!");
    Serial.println("please enter a command");
    canMove = false;
  }
  
  if (sensor_values[0] > QTR_THRESHOLD2)
  {
    motors.setSpeeds(200, 0);
    delay(250);
    motors.setSpeeds(50, 50);
    delay(250);
    loop();

  }
  
  if (sensor_values[5] > QTR_THRESHOLD2)
  {
    motors.setSpeeds(0, 200);
    delay(250);
    motors.setSpeeds(50, 50);
    delay(250);
    loop();
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


// see if there's incoming serial data:




//if (sensor_values[2] > QTR_THRESHOLD1 || sensor_values[3] > QTR_THRESHOLD1) /// TRY 2 DIFFERNT QTR VALUES
//{
//
//  motors.setSpeeds(0, 0);
//  Serial.println("hit a wall!");
//  Serial.println("please enter a command");
//  canMove = false;
//}
//else
//{
//  if (sensor_values[0] > QTR_THRESHOLD2)
//  {
//    motors.setSpeeds(200, 0);
//    delay(250);
//    motors.setSpeeds(50, 50);
//    delay(250);
//    loop();
//
//  }
//  else if (sensor_values[5] > QTR_THRESHOLD2)
//  {
//    motors.setSpeeds(0, 200);
//    delay(250);
//    motors.setSpeeds(50, 50);
//    delay(250);
//    loop();
//  }
