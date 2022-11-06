#include <Wire.h>                                                        
String readString;                                                          
byte I2C_OnOff = 0;

void serial_flush(void);
void readstring();
String inputString = "";
bool stringComplete = false; 
                                                             
void setup()
{
  Wire.begin();                                                             
  Serial.begin(9600);
  Serial.println("Ready");   
}
void loop()
{
  
    delay(20);
    readstring();
    serial_flush();                                                              

  int intVal = inputString.toInt();
  byte bb;
  if (inputString != "")
  {
    bb = (byte)intVal;
      
    inputString = "";
  }
  //inputString = "0";
 
  //if(bb > 0 )
  if(true)
  {
      Wire.beginTransmission(3);                                                
      Wire.write(bb); 
                                                      
      Wire.endTransmission();
      //**********************
      Wire.beginTransmission(4);                                                
      Wire.write(bb); 
                                                      
      Wire.endTransmission(); 
      //********************
      Wire.beginTransmission(5);                                                
      Wire.write(bb); 
                                                      
      Wire.endTransmission(); 
      //*********************
      Wire.beginTransmission(6);                                                
      Wire.write(bb); 
                                                      
      Wire.endTransmission(); 
      // *************************
      Serial.println(bb);
      bb = 0;
  }
                                                   
}

void serial_flush(void) {
  while (Serial.available()) Serial.read();
}

void readstring()
{
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline, set a flag so the main loop can
    // do something about it:
    if (inChar == '\n') {
      stringComplete = true;
      serial_flush();
      return;
    }
  }

  
}
