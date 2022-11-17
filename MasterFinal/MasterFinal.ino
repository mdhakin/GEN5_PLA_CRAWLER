#include <Wire.h>        
                                                
String readString;                                                          
byte I2C_OnOff = 0;

#define BLUE_LIGHT 2
#define YELLOW_LIGHT 3

#define TrackA 4
#define TrackB 6
#define TrackC 5
#define TrackD 3

#define MAX_SPEED 255

#define ENCODER_CONTROLLER 11

void HandleReport(String sVal);
void serial_flush(void);
void updateLights();
void PrintInfo();
void ChangeInitialDirectionForSetUp(int trackid);
void readstring();
void SerialHandler(String sVal);  
void adjustSpeed(String sVal);

int currentMsgSize = 0;


// Hold the speeds of each track
int ASpeed = 0;
int BSpeed = 0;
int CSpeed = 0;
int DSpeed = 0;


// Hold the Direction of each track
int ADir = 0;
int BDir = 0;
int CDir = 0;
int DDir = 0;

int encoder1Angle = 0;    // Not implemented
int encoder2Angle = 0;    // Not implemented
int vehicleAngle = 0;     // Not implemented

// holds serial port incoming messages
String inputString = "";

// Message from the serial port is ready
bool stringComplete = false; 
                                                           
void setup()
{
  pinMode(BLUE_LIGHT, OUTPUT);
  pinMode(YELLOW_LIGHT, OUTPUT);
  Wire.begin();                                                             
  Serial.begin(9600);
    
  digitalWrite(BLUE_LIGHT, HIGH);
  digitalWrite(YELLOW_LIGHT, LOW);
  updateLights();

  //delay(2000);
  //ChangeInitialDirectionForSetUp((int)TrackB);
  Serial.println("Ready"); 
  
}

void PrintInfo()
{
  /*
  Serial.println("                        GEN 3 DEVELOPMENT PLATFORM");
  Serial.println("              By Matthew Hakin November 12 2022");
  Serial.println("========================================================================");
  Serial.println("                      Language Reference");
  Serial.println("      There are 4 Tracks A, B, C, D");
  Serial.println("      X is used to address all the Tracks at once");
  Serial.println("      A40 Sets Track A to a speed of 400");
  Serial.println("      X40 Sets all tracks to a speed of 400");
  Serial.println("      Track A is front left");
  Serial.println("      Track B is front right");
  Serial.println("      Track C is rear left");
  Serial.println("      Track D is rear right");
  Serial.println("      RSA = Report Velocity on track A");
  Serial.println("      RPA = Report Direction on track A");
  Serial.println("      RTA = Report ticks for Ttack A");
  Serial.println("      +A increases Track a Speed by 10");
  Serial.println("      +X increases all Tracks Speeds by 10");
  Serial.println("      -A decreases Track a Speed by 10");
  Serial.println("      -X decreases all Tracks Speeds by 10");
  Serial.println("      AO will change directions on track A");
  Serial.println("      XO will change direcions on all tracks");
  Serial.println("      FL - Flash Lights");
  Serial.println("      S will stop all motion");
  Serial.println("      E1 will return Encoder1s value");           // Not implemented
  Serial.println("      E2 will return Encoder1s value");           // Not implemented
  Serial.println("      E3 will return the angle of the crawler");  // Not implemented
  Serial.println("***************************************************");
  Serial.println("                 ||    -    ||                     ");
  Serial.println("                 ||- /   \- ||                     ");
  Serial.println("                 ||  |   |  ||                     ");
  Serial.println("                     |   |                         ");
  Serial.println("                     -----                         ");
  Serial.println("                     |   |                         ");
  Serial.println("                 ||  |   |  ||                     ");
  Serial.println("                 ||- \   /- ||                     ");
  Serial.println("                 ||    -    ||                     ");
  Serial.println("***************************************************");*/
}


void flashLights()
{
  int cDelay = 500;

  for(int i = 0;i<10;i++)
  {
    digitalWrite(BLUE_LIGHT, LOW);
    digitalWrite(YELLOW_LIGHT, LOW);
    delay(cDelay);
    digitalWrite(BLUE_LIGHT, HIGH);
    digitalWrite(YELLOW_LIGHT, HIGH);
    delay(cDelay);
  }
  
  
  
updateLights();

}
void loop()
{
    readstring();
    inputString.toUpperCase();
    // PrintInfo()
    if (stringComplete)
    {
      if(inputString[0] == 'L')
      {
        ChangeInitialDirectionForSetUp((int)TrackB);
        flashLights();
        inputString = "";
        return;
      }else if(inputString[0] == 'F' && inputString[1] == 'L')
      {
        flashLights();
        inputString = "";
      }else if(inputString[0] == 'I' && inputString[1] == 'N')
      {
        PrintInfo();
        inputString = "";
      }else if(inputString[0] == 'S')
      {
          inputString = "X0";
          SerialHandler(inputString);
          flashLights();
      }else if(inputString[0] == 'R')
      {
        HandleReport(inputString);
      }
      else if(inputString[0] == '-' || inputString[0] == '+')
      {
        adjustSpeed(inputString);
        inputString = "";
      }
      else
      {
        SerialHandler(inputString);
      }
      
      serial_flush();
    }     
    if(Wire.available()) 
    {        
      byte a = Wire.read();
      byte b = Wire.read();
      
      int bigNum;
      bigNum = a;
      bigNum = (bigNum << 8) | b;
      Serial.println(bigNum);
      
      Serial.println("======");
    }
                                                   
}

void adjustSpeed(String sVal)
{
  int adjustVal = 1;
  if(sVal[0] == '+')
  {
    if(sVal[1] == 'A')
    {
        if(ASpeed < MAX_SPEED - adjustVal)
        {
            ASpeed = ASpeed + adjustVal;
            Serial.println(ASpeed);
            Wire.beginTransmission(TrackA);                                                
            Wire.write(ASpeed);                                              
            Wire.endTransmission();
        }
    }

    if(sVal[1] == 'B')
    {
        if(BSpeed < MAX_SPEED - adjustVal)
        {
            BSpeed = BSpeed + adjustVal;
            Serial.println(BSpeed);
            Wire.beginTransmission(TrackB);                                                
            Wire.write(BSpeed);                                              
            Wire.endTransmission();
        }
    }

    if(sVal[1] == 'C')
    {
        if(CSpeed < MAX_SPEED - adjustVal)
        {
            CSpeed = CSpeed + adjustVal;
            Serial.println(CSpeed);
            Wire.beginTransmission(TrackC);                                                
            Wire.write(CSpeed);                                              
            Wire.endTransmission();
        }
    }

    if(sVal[1] == 'D')
    {
        if(DSpeed < MAX_SPEED - adjustVal)
        {
            DSpeed = DSpeed + adjustVal;
            Serial.println(DSpeed);
            Wire.beginTransmission(TrackD);                                                
            Wire.write(DSpeed);                                              
            Wire.endTransmission();
        }
    }

    if(sVal[1] == 'X')
    {
      
        if(ASpeed < MAX_SPEED - adjustVal)
        {
            ASpeed = ASpeed + adjustVal;
            Serial.println(ASpeed);
            Wire.beginTransmission(TrackA);                                                
            Wire.write(ASpeed);                                              
            Wire.endTransmission();
        }

        if(DSpeed < MAX_SPEED - adjustVal)
        {
            DSpeed = DSpeed + adjustVal;
            Serial.println(DSpeed);
            Wire.beginTransmission(TrackD);                                                
            Wire.write(DSpeed);                                              
            Wire.endTransmission();
        }
        if(BSpeed < MAX_SPEED - adjustVal)
        {
            BSpeed = BSpeed + adjustVal;
            Serial.println(BSpeed);
            Wire.beginTransmission(TrackB);                                                
            Wire.write(BSpeed);                                              
            Wire.endTransmission();
        }

        if(CSpeed < MAX_SPEED - adjustVal)
        {
            CSpeed = CSpeed + adjustVal;
            Serial.println(CSpeed);
            Wire.beginTransmission(TrackC);                                                
            Wire.write(CSpeed);                                              
            Wire.endTransmission();
        }
    }
  }else if(sVal[0] == '-')
  {
    if(sVal[1] == 'A')
    {
        if(ASpeed > adjustVal)
        {
            ASpeed = ASpeed - adjustVal;
            Serial.println(ASpeed);
            Wire.beginTransmission(TrackA);                                                
            Wire.write(ASpeed);                                              
            Wire.endTransmission();
        }
    }
    if(sVal[1] == 'B')
    {
        if(BSpeed > adjustVal)
        {
            BSpeed = BSpeed - adjustVal;
            Serial.println(BSpeed);
            Wire.beginTransmission(TrackB);                                                
            Wire.write(BSpeed);                                              
            Wire.endTransmission();
        }
    }

    if(sVal[1] == 'C')
    {
        if(CSpeed > adjustVal)
        {
            CSpeed = CSpeed - adjustVal;
            Serial.println(CSpeed);
            Wire.beginTransmission(TrackC);                                                
            Wire.write(CSpeed);                                              
            Wire.endTransmission();
        }
    }

    if(sVal[1] == 'D')
    {
        if(DSpeed > adjustVal)
        {
            DSpeed = DSpeed - adjustVal;
            Serial.println(DSpeed);
            Wire.beginTransmission(TrackD);                                                
            Wire.write(DSpeed);                                              
            Wire.endTransmission();
            
        }
     }

    if(sVal[1] == 'X')
    {
        if(ASpeed > adjustVal)
        {
            ASpeed = ASpeed - adjustVal;
            Serial.println(ASpeed);
            Wire.beginTransmission(TrackA);                                                
            Wire.write(ASpeed);                                              
            Wire.endTransmission();
        }

        if(DSpeed > adjustVal)
        {
            DSpeed = DSpeed - adjustVal;
            Serial.println(DSpeed);
            Wire.beginTransmission(TrackD);                                                
            Wire.write(DSpeed);                                              
            Wire.endTransmission();
        }
        
        if(BSpeed > adjustVal)
        {
            BSpeed = BSpeed - adjustVal;
            Serial.println(BSpeed);
            Wire.beginTransmission(TrackB);                                                
            Wire.write(BSpeed);                                              
            Wire.endTransmission();
        }

        if(CSpeed > adjustVal)
        {
            CSpeed = CSpeed - adjustVal;
            Serial.println(CSpeed);
            Wire.beginTransmission(TrackC);                                                
            Wire.write(CSpeed);                                              
            Wire.endTransmission();
        
        } 
    }
  }
}

void ChangeInitialDirectionForSetUp(int trackid)
{
  int DirectionChangeCommand = 254;
  
  Wire.beginTransmission(trackid);                                                
  Wire.write(DirectionChangeCommand);                                              
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

void requestPosition()
{
   Wire.requestFrom(3, 2);    // request potentiometer position from slave 0x08
}

// Format R*A
void HandleReport(String sVal)
{

  if(sVal[2] == 'A')
  {
    if(sVal[1] == 'S')
    {
      Serial.print("Track A Speed: ");
      Serial.println(ASpeed);
    }else if(sVal[1] == 'P')
    {
      Serial.print("Track A Direction: ");
      Serial.println(ADir);
    }else if(sVal[1] == 'T')
    {
      Wire.requestFrom(TrackA,2);
    }
  }else if(sVal[2] == 'B')
  {
    if(sVal[1] == 'S')
    {
      Serial.print("Track B Speed: ");
      Serial.println(BSpeed);
    }else if(sVal[1] == 'P')
    {
      Serial.print("Track B Direction: ");
      Serial.println(BDir);
    }else if(sVal[1] == 'T')
    {
      Wire.requestFrom(TrackB,2);
    }
  }else if(sVal[2] == 'C')
  {
    if(sVal[1] == 'S')
    {
      Serial.print("Track C Speed: ");
      Serial.println(CSpeed);
    }else if(sVal[1] == 'P')
    {
      Serial.print("Track C Direction: ");
      Serial.println(CDir);
    }else if(sVal[1] == 'T')
    {
      Wire.requestFrom(TrackC,2);
    }
  }else if(sVal[2] == 'D')
  {
    if(sVal[1] == 'S')
    {
      Serial.print("Track D Speed: ");
      Serial.println(DSpeed);
    }else if(sVal[1] == 'P')
    {
      Serial.print("Track D Direction: ");
      Serial.println(DDir);
    }else if(sVal[1] == 'T')
    {
      Wire.requestFrom(TrackD,2);
    }
  }
  inputString = "";
  return;
}

void SerialHandler(String sVal)
{
       currentMsgSize = sVal.length();
       
        Wire.beginTransmission(11); 
        inputString = "";
        stringComplete = false;
        
      if(sVal[0] =='A' && sVal[1] != 'O')
      {
        String newStr = sVal.substring(1);
        int UpdateValue = newStr.toInt();
        if(UpdateValue >= MAX_SPEED)
        {
          UpdateValue = 80;
        }
        ASpeed = UpdateValue;
        Serial.println(UpdateValue);
        Wire.beginTransmission(TrackA);                                                
        Wire.write(UpdateValue);                                              
        Wire.endTransmission();
      }else if(sVal[0] == 'B' && sVal[1] != 'O')
      {
        String newStr = sVal.substring(1);
        int UpdateValue = newStr.toInt();
        if(UpdateValue >= MAX_SPEED)
        {
          UpdateValue = 80;
        }
        BSpeed = UpdateValue;
        Wire.beginTransmission(TrackB);                                                
        Wire.write(UpdateValue);                                              
        Wire.endTransmission();
      }else if(sVal[0] == 'C' && sVal[1] != 'O')
      {
        String newStr = sVal.substring(1);
        int UpdateValue = newStr.toInt();
        if(UpdateValue >= MAX_SPEED)
        {
          UpdateValue = 80;
        }
        CSpeed = UpdateValue;
        Wire.beginTransmission(TrackC);                                                
        Wire.write(UpdateValue);                                              
        Wire.endTransmission();
      }else if(sVal[0] == 'D' && sVal[1] != 'O')
      {
        String newStr = sVal.substring(1);
        int UpdateValue = newStr.toInt();
        if(UpdateValue >= MAX_SPEED)
        {
          UpdateValue = 80;
        }
        DSpeed = UpdateValue;
        Wire.beginTransmission(TrackD);                                                
        Wire.write(UpdateValue);                                              
        Wire.endTransmission();
      }else if(sVal[0] == 'X' && sVal[1] != 'O')
      {
        String newStr = sVal.substring(1);
        int UpdateValue = newStr.toInt();
        if(UpdateValue >= MAX_SPEED)
        {
          UpdateValue = 80;
        }
        ASpeed = UpdateValue;
        BSpeed = UpdateValue;
        CSpeed = UpdateValue;
        DSpeed = UpdateValue;

        
        Wire.beginTransmission(TrackA);                                                
        Wire.write(UpdateValue); 
                                                        
        Wire.endTransmission();
        
        //**********************
        
        Wire.beginTransmission(TrackB);                                                
        Wire.write(UpdateValue); 
                                                        
        Wire.endTransmission(); 
        
        //********************
        
        Wire.beginTransmission(TrackC);                                                
        Wire.write(UpdateValue); 
                                                        
        Wire.endTransmission(); 
        
        //*********************
        
        Wire.beginTransmission(TrackD);                                                
        Wire.write(UpdateValue); 
                                                        
        Wire.endTransmission(); 
        // *************************
  }else if(sVal[1] == 'O')
  {
    if(sVal[0] == 'A')
    {
      
        int DirectionChangeCommand = 254;
        if(ADir == 0)
        {
          ADir = 1;
        }else
        {
          ADir = 0;
        }
        Serial.println("Changing Direction on track A");
        Wire.beginTransmission(TrackA);                                                
        Wire.write(DirectionChangeCommand);                                              
        Wire.endTransmission();
    }else if(sVal[0] == 'B')
    {
      int DirectionChangeCommand = 254;
        if(BDir == 0)
        {
          BDir = 1;
        }else
        {
          BDir = 0;
        }
        Serial.println("Changing Direction on track B");
        Wire.beginTransmission(TrackB);                                                
        Wire.write(DirectionChangeCommand);                                              
        Wire.endTransmission();
    }else if(sVal[0] == 'C')
    {
      int DirectionChangeCommand = 254;
        if(CDir == 0)
        {
          CDir = 1;
        }else
        {
          CDir = 0;
        }
        Serial.println("Changing Direction on track C");
        Wire.beginTransmission(TrackC);                                                
        Wire.write(DirectionChangeCommand);                                              
        Wire.endTransmission();
    }else if(sVal[0] == 'D')
    {
      int DirectionChangeCommand = 254;
        if(DDir == 0)
        {
          DDir = 1;
        }else
        {
          DDir = 0;
        }
        Serial.println("Changing Direction on track D");
        Wire.beginTransmission(TrackD);                                                
        Wire.write(DirectionChangeCommand);                                              
        Wire.endTransmission();
    }else if(sVal[0] == 'X')
    {
      int DirectionChangeCommand = 254;
      if(ADir == 0)
        {
          ADir = 1;
        }else
        {
          ADir = 0;
        }
        Serial.println("Changing Direction on track A");
        Wire.beginTransmission(TrackA);                                                
        Wire.write(DirectionChangeCommand);                                              
        Wire.endTransmission();

        if(BDir == 0)
        {
          BDir = 1;
        }else
        {
          BDir = 0;
        }
        Serial.println("Changing Direction on track B");
        Wire.beginTransmission(TrackB);                                                
        Wire.write(DirectionChangeCommand);                                              
        Wire.endTransmission();

        if(CDir == 0)
        {
          CDir = 1;
        }else
        {
          CDir = 0;
        }
        Serial.println("Changing Direction on track C");
        Wire.beginTransmission(TrackC);                                                
        Wire.write(DirectionChangeCommand);                                              
        Wire.endTransmission();

        if(DDir == 0)
        {
          DDir = 1;
        }else
        {
          DDir = 0;
        }
        Serial.println("Changing Direction on track D");
        Wire.beginTransmission(TrackD);                                                
        Wire.write(DirectionChangeCommand);                                              
        Wire.endTransmission();
        
    }
    
  }
      updateLights();
      return;
}

void updateLights()
{
  if((ASpeed == BSpeed) && (BSpeed == CSpeed) && (CSpeed == DSpeed) && ASpeed > 0)
  {
    digitalWrite(YELLOW_LIGHT, HIGH);
  }else if(!(ASpeed == BSpeed) && (BSpeed == CSpeed) && (CSpeed == DSpeed) && ASpeed > 0)
  {
    digitalWrite(YELLOW_LIGHT, HIGH);
  }else if((ASpeed == BSpeed) && (BSpeed == CSpeed) && (CSpeed == DSpeed) && ASpeed == 0)
  {
    //digitalWrite(BLUE_LIGHT, LOW);
    digitalWrite(YELLOW_LIGHT, LOW);
  }else if((ASpeed > 0 || BSpeed > 0 || CSpeed > 0 || DSpeed > 0) && !((ASpeed == BSpeed) && (BSpeed == CSpeed) && (CSpeed == DSpeed)))
  {
    //XdigitalWrite(BLUE_LIGHT, LOW);
    digitalWrite(YELLOW_LIGHT, HIGH);
  }
  digitalWrite(BLUE_LIGHT, HIGH);
}

void WireAction()
{
  return;
}
