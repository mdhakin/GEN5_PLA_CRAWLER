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
  Serial.println("Type On to turn on the LED and Off to shut it down!");   
}
void loop()
{
  //while (Serial.available())
  //{
    delay(20);
    readstring();
    serial_flush();                                                              
    //char c = Serial.read();                                                 
    //readString += c;                                                        
  //}
  if ((inputString[0] == 'o' || inputString[0] == 'O'))
  {
    I2C_OnOff = 1;
  }
  else if ((inputString[0] == 'f' || inputString[0] == 'F'))
  {
    I2C_OnOff = 0;
  }
  if (inputString.length() > 0)                                            
  {
    Serial.println(inputString.length());                                            
    inputString = "";                                                        
  }
  Wire.beginTransmission(1);                                                
  Wire.write(I2C_OnOff);                                                    
  Wire.endTransmission();                                                   
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
