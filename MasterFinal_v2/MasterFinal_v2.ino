/*
Gen 6 control program 
By Matthew Hakin
December 6 2022
=======================================
Reference
All i2c commands must be 8 bytes long

***** To issue commands to one or all the tracks
X - All tracks
A 
B 
C 
D 

AV-00100 will set track A to a velocity of 100

AO will reverse track A's direction

R is to request info from a track 

RAV00000 will request track A's velocity
RAA00000 will request track A'S acceleration
RAD00000 wil request track A's direction
RAP00000 will request track A's position

RT - tiltsensor
RE1 Encoder 1
RE2 Encoder 2
*/

#include <Wire.h>        

// Used to hold data that comes from the serial port
String readString; 

// holds serial port incoming messages
String inputString = "";

// Message from the serial port is ready
bool stringComplete = false; 

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

unsigned long startMillis;  //some global variables available anywhere in the program
unsigned long currentMillis;
const unsigned long period = 1000;  //the value is a number of milliseconds
unsigned long ticks = 0;



// Variables to hold encoder angles and tilt sensor value
int encoder1Angle = 0;    // Not implemented
int encoder2Angle = 0;    // Not implemented
int tiltSensorValue = 0;  // Not implemented

// Set pins for the lights
#define BLUE_LIGHT 2
#define YELLOW_LIGHT 3

// Id numbers assigned to tracks
#define TrackA 3
#define TrackB 4
#define TrackC 5
#define TrackD 6

// maximum motor speed
#define MAX_SPEED 10000

// i2c address for the encoder controller
#define ENCODER_CONTROLLER 11

// *******************************************************************
// *******************************************************************
// Function prototypes
// *******************************************************************
// *******************************************************************

// Flush the serial buffer
void serial_flush(void);

// read input commands
void readstring();

// This function takes a string that contains the id of the track and the speed to adjust to.
// Example XV-00000 sets all tracks to 0
// Example AV-01000 sets track A to 1000
void changeSpeed(String newSpeed, int track);
void changeAcceleration(String newAccel, int track);

// This function is the first stop once a command is received and verified
// this is a function that cleans up the main loop
void processCommand(String command);

// This function returns the numeric version of a track ID sent in a char
int getIDFromChar(char letter);

// requests position from motor controller
int requestPosition(int motorID);

// Move the Wire handling routines out of main loop
void I2C_Message_Handler(byte aa, byte bb, byte cc, byte dd, byte ee, byte ff, byte gg, byte hh );
void ENCODER_Message_Handler(byte aa, byte bb, byte cc, byte dd, byte ee, byte ff, byte gg, byte hh );

void setup()
{
  startMillis = millis();  //initial start time
  pinMode(BLUE_LIGHT, OUTPUT);
  pinMode(YELLOW_LIGHT, OUTPUT);
  Wire.begin();  
                                                          
  Serial.begin(9600);
    
  digitalWrite(BLUE_LIGHT, HIGH);
  digitalWrite(YELLOW_LIGHT, LOW);
  
  Serial.println("Ready"); 
}


void printLogo()
{
  Serial.println("***************** Mistras Robotics ****************");
  Serial.println("***************************************************");
  Serial.println("                 ||    -    ||                     ");
  Serial.println("                 ||- /   \\- ||                     ");
  Serial.println("                 ||  |   |  ||                     ");
  Serial.println("                     |   |                         ");
  Serial.println("                     -----                         ");
  Serial.println("                     |   |                         ");
  Serial.println("                 ||  |   |  ||                     ");
  Serial.println("                 ||- \\   /- ||                     ");
  Serial.println("                 ||    -    ||                     ");
  Serial.println("***************************************************");
  Serial.println("************** By Matthew Hakin *******************");
  Serial.println("");
  Serial.println("");
  Serial.println("");
  Serial.print("Ready>> ");
}


void loop()
{
  currentMillis = millis();
  if (currentMillis - startMillis >= period)  //test whether the period has elapsed
  {
    ticks++;
    startMillis = currentMillis;  //IMPORTANT to save the start time of the current LED state.
    //Wire.requestFrom(ENCODER_CONTROLLER, 8);  // request position from slave 0x08
    int sensorValue = analogRead(A3);
    tiltSensorValue = sensorValue;
  }

  
    readstring();
    inputString.toUpperCase();

    if(stringComplete && inputString.substring(0,4) == "LOGO")
      {
        printLogo();
        inputString = "";
        stringComplete = false;
      }
    
    if (stringComplete)
    {
      
      
        if(inputString[0] != 'R' && inputString.length() == 9 && inputString[1] != 'O')
        {
          processCommand(inputString);
          inputString = "";
          stringComplete = false;
         
        }else if(inputString[0] == 'R' &&  inputString[1] == 'E' &&  inputString[2] == '1')
        {
          Serial.print("Encoder 1 value: ");
          Serial.println(encoder1Angle);
          inputString = "";
          stringComplete = false;
         
        }else if(inputString[0] == 'R' &&  inputString[1] == 'E' &&  inputString[2] == '2')
        {
          Serial.print("Encoder 2 value: ");
          Serial.println(encoder2Angle);
          inputString = "";
          stringComplete = false;
         
        }else if(inputString[0] == 'R' &&  inputString[1] == 'T')
        {
          Serial.print("Tilt Sensor: ");
          Serial.println(tiltSensorValue);
          inputString = "";
          stringComplete = false;
         
        }else if (inputString[0] == 'S' && inputString.length() != 9 && inputString[1] == 'C')
        {
          inputString = "";
          stringComplete = false;
        }else if (inputString[0] != 'R' && inputString.length() != 9 && inputString[1] != 'O')
        {
          Serial.print("Input Length ");
          Serial.println(inputString.length());
          inputString = "";
          stringComplete = false;
          Serial.println("Input not formatted correctly");
          return;
        }else if(inputString[0] == 'R' && inputString[1] != 'O')
        {
          processCommand(inputString);
          inputString = "";
          stringComplete = false;
        }else if(inputString[1] == 'O')
        {
          processCommand(inputString);
          inputString = "";
          stringComplete = false;
        }
        
    }
    
    
    if(Wire.available()) 
    { 
      byte rec[10];
      int i = 0;
      while(0 < Wire.available())
      {
        rec[i] = Wire.read();
        i++;     
      }
      byte a = rec[0];
      byte b = rec[1];
      byte c = rec[2];
      byte d = rec[3];
      byte e = rec[4];
      byte f = rec[5];
      byte g = rec[6];
      byte h = rec[7];
      
      if (d == 11)
      {
        ENCODER_Message_Handler(a,b,c,d,e,f,g,h);
      }else{
        I2C_Message_Handler(a,b,c,d,e,f,g,h);
      }

      
      return;
    }
}

void ENCODER_Message_Handler(byte aa, byte bb, byte cc, byte dd, byte ee, byte ff, byte gg, byte hh )
{
  int ENCODER1;
      ENCODER1 = aa;
      ENCODER1 = (ENCODER1 << 8) | bb;

  int ENCODER2;
      ENCODER2 = gg;
      ENCODER2 = (ENCODER2 << 8) | hh;

  encoder1Angle = ENCODER1;
  encoder2Angle = ENCODER2;

}


void I2C_Message_Handler(byte aa, byte bb, byte cc, byte dd, byte ee, byte ff, byte gg, byte hh )
{
  int bigNum;
      bigNum = aa;
      bigNum = (bigNum << 8) | bb;
      int mtrDir = (int)cc;
      int messageRecFrom = (int)dd;

      String TrackDescription = "";

      if (messageRecFrom == 3)
      {
        TrackDescription = "Left Front(A)";
      }else if (messageRecFrom == 4)
      {
        TrackDescription = "Right Front(B)";
      }else if (messageRecFrom == 5)
      {
        TrackDescription = "Left Rear(C)";
      }else if (messageRecFrom == 4)
      {
        TrackDescription = "Right Rear(D)";
      }else if (messageRecFrom == 11)
      {
        
      }
      
      Serial.print("Response received from ");
        Serial.println(TrackDescription);
      if(messageRecFrom == 3)
      {
        ASpeed = bigNum;
        ADir = mtrDir;
      }else if(messageRecFrom == 4)
      {
        BSpeed = bigNum;
        BDir = mtrDir;
      }else if(messageRecFrom == 5)
      {
        CSpeed = bigNum;
        CDir = mtrDir;
      }else if(messageRecFrom == 6)
      {
        DSpeed = bigNum;
        DDir = mtrDir;
      }

      
      Serial.println("");
      Serial.println("");
      Serial.println("Report:");
      Serial.print("Motor ");
      Serial.print(TrackDescription);
      Serial.print(" position = ");
      Serial.println(bigNum);
      
      Serial.print("Direction = ");
      if(mtrDir == 0)
      {
        Serial.println("Forward");
      }else
      {
        Serial.println("Reverse");
      }
      
      Serial.print("spd:");
      Serial.print((char)ee);
      Serial.print((char)ff);
      Serial.println("00");
      Serial.println("==================");
      Serial.println("");
      Serial.println("");
      Serial.println("");
      return;
}

// newSpeed must be formatted as 8 character string example AV-00000
void changeAcceleration(String newAccel, int track)
{
  //Serial.println(newSpeed);
    char msg[8] = { ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ' };
    msg[0] = newAccel[0];
    msg[1] = newAccel[1];
    msg[2] = newAccel[2];
    msg[3] = newAccel[3];
    msg[4] = newAccel[4];
    msg[5] = newAccel[5];
    msg[6] = newAccel[6];
    msg[7] = newAccel[7];

    char Acceleration[5] = { ' ', ' ', ' ', ' ', ' ' };
    Acceleration[0] = msg[3];
    Acceleration[1] = msg[4];
    Acceleration[2] = msg[5];
    Acceleration[3] = msg[6];
    Acceleration[4] = msg[7];

    String Acc = Acceleration;
    int iAcel = Acc.toInt();

    Serial.print("Sending ");
    Serial.print(newAccel.substring(0,3));
    Serial.println(iAcel);
    Wire.beginTransmission(track);  // in define like TrackA                                              
    Wire.write(msg);                                              
    Wire.endTransmission(true);
}

// newSpeed must be formatted as 8 character string example AV-00000
void changeSpeed(String newSpeed, int track)
{
    //Serial.println(newSpeed);
    char msg[8] = { ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ' };
    msg[0] = newSpeed[0];
    msg[1] = newSpeed[1];
    msg[2] = newSpeed[2];
    msg[3] = newSpeed[3];
    msg[4] = newSpeed[4];
    msg[5] = newSpeed[5];
    msg[6] = newSpeed[6];
    msg[7] = newSpeed[7];

    char velocity[5] = { ' ', ' ', ' ', ' ', ' ' };
    velocity[0] = msg[3];
    velocity[1] = msg[4];
    velocity[2] = msg[5];
    velocity[3] = msg[6];
    velocity[4] = msg[7];
    
    String spd = velocity;
    int iSpeed = spd.toInt();
    
    if(newSpeed[1] == 'V' && track == 3)
    {
      Serial.print("Track A speed = ");
      ASpeed = iSpeed;
      Serial.println(ASpeed);
    }else if(newSpeed[1] == 'V' && track == 4)
    {
      Serial.print("Track B speed = ");
      BSpeed = iSpeed;
      Serial.println(BSpeed);
    }else if(newSpeed[1] == 'V' && track == 5)
    {
      Serial.print("Track C speed = ");
      CSpeed = iSpeed;
      Serial.println(CSpeed);
    }else if(newSpeed[1] == 'V' && track == 6)
    {
      Serial.print("Track D speed = ");
      DSpeed = iSpeed;
      Serial.println(DSpeed);
    }
    
    
    
    Serial.print("Sending ");
    Serial.print(newSpeed.substring(0,3));
    Serial.println(iSpeed);
    Wire.beginTransmission(track);  // in define like TrackA                                              
    Wire.write(msg);                                              
    Wire.endTransmission(true);
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

void processCommand(String command)
{

    if(command[0] == 'A' || 
    command[0] == 'B' || 
    command[0] == 'C' || 
    command[0] == 'D')              // If it is a speed command to a specific track
    {
        int trackId = getIDFromChar(command[0]);
        if(trackId != 0 && (command[1] == 'V' || command[1] == 'O' || command[1] == 'A'))
        {
          if(command[1] == 'O')
          {
            char com2[9] = { command[0], 'O', '-', '0', '0', '0', '0', '0', '\n' };
            String sCom2 = com2;
            changeSpeed(sCom2, trackId);
            
          }else if(command[1] == 'V')
          {
            changeSpeed(command, trackId);
          }else if(command[1] == 'A')
          {
            changeAcceleration(command, trackId);
          }
           
           
        }
        return;
    }else if(command[0] == 'X')     // If it is supposed to go out to all tracks
    {
        
        if(command[1] == 'V')
        {
            changeSpeed(command, TrackA);
            changeSpeed(command, TrackB);
            changeSpeed(command, TrackC);
            changeSpeed(command, TrackD);
        }
        return;
    }

    if(command[0] == 'R')
    {
      int trackId = getIDFromChar(command[1]);
      
        requestPosition(trackId);
        return;
    }
    return;
}

int getIDFromChar(char letter)
{
    if (letter == 'A')
    {
        return TrackA;
    }else if(letter == 'B')
    {
        return TrackB;
    }else if(letter == 'C')
    {
        return TrackC;
    }else if(letter == 'D')
    {
        return TrackD;
    }
    return 0;
}

int requestPosition(int motorID)
{
   Wire.requestFrom(motorID, 8);  // request position from slave 0x08
   return motorID;
}
