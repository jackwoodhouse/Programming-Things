import g4p_controls.*;

import processing.serial.*;

Serial myPort;  // Create object from Serial class
final char LF = 10;
int val;        // Data received from the serial port

Serial serialPort;
String Input;

int bgcol = 15;

public void setup() {
size(500, 360);
createGUI();

String portName = Serial.list()[1];
myPort = new Serial(this, portName, 9600);
}

public void draw() {
  background(200, 200, 200);
  if (myPort.available() > 0) 
  { 
    Input = myPort.readStringUntil(LF);
    println(Input);
    
    if (Input != null)
    {
      String text = textarea1.getText();
      if (text == "Messages from Zumo")
      {
        textarea1.setText("");
      }
      textarea1.appendText(Input);
    }
  }

}
