#include <Wire.h>                                                        
String readString;                                                          
byte I2C_OnOff = 0;

void serial_flush(void);
void readstring();
String inputString = "";
bool stringComplete = false; 
int echo = true;
int speedVal = 0;   
int cnt = 0;                                                         
void setup()
{
  Wire.begin();                                                             
  Serial.begin(250000);
  Serial.println("online");   
  serial_flush();
  serial_flush();
}
void loop()
{
  int addr = 1;
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
    if(inputString.substring(0,2) == "av")
    {
      String speed1 = inputString.substring(2,5);
      speedVal = speed1.toInt();
      //Serial.println("Changing speed for track a to " + speed1);
    }else if (inputString.substring(0,2) == "zo")
    {
      addr = 3;
      I2C_OnOff = 1;
    }else if (inputString.substring(0,2) == "zf")
    {
      addr = 3;
      I2C_OnOff = 0;
    }
    else if (inputString.substring(0,2) == "xo")
    {
      addr = 2;
      I2C_OnOff = 1;
    }else if (inputString.substring(0,2) == "xf")
    {
      addr = 2;
      I2C_OnOff = 0;
    }
    //Serial.println(inputString.length());                                            
    inputString = "";                                                        
  }
  if (cnt == 0)
  {
    Serial.println(String(speedVal) + '\n');
  }
  
  cnt++;
  if(cnt > 80)
  {
    cnt = 0;
  }
  Wire.beginTransmission(addr);                                                
  Wire.write(I2C_OnOff);                                                    
  Wire.endTransmission();                                                   
}

void serial_flush(void) {
  
  while (Serial.available()) Serial.read();
}

void readstring()
{
  //delay(150);
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
      if (echo == true)
      {
        int sLen = inputString.length();
        
        Serial.println(inputString.substring(0,sLen -3));
      }
      serial_flush();
      return;
    }
  }

  
}
