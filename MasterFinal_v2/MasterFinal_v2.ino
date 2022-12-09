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

// This function is the first stop once a command is received and verified
// this is a function that cleans up the main loop
void processCommand(String command);

// This function returns the numeric version of a track ID sent in a char
int getIDFromChar(char letter);

// requests position from motor controller
int requestPosition(int motorID);

void setup()
{
  pinMode(BLUE_LIGHT, OUTPUT);
  pinMode(YELLOW_LIGHT, OUTPUT);
  Wire.begin();                                                             
  Serial.begin(9600);
    
  digitalWrite(BLUE_LIGHT, HIGH);
  digitalWrite(YELLOW_LIGHT, LOW);
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
  Serial.println("Ready"); 
}

void loop()
{
    readstring();
    inputString.toUpperCase();
    // PrintInfo()
    if (stringComplete)
    {
        processCommand(inputString);
    }

    if(Wire.available()) 
    {        
      byte a = Wire.read();
      byte b = Wire.read();
      byte c = Wire.read();
      byte d = Wire.read();
      byte e = Wire.read();
      byte f = Wire.read();
      byte g = Wire.read();
      byte h = Wire.read();

      int bigNum;
      bigNum = a;
      bigNum = (bigNum << 8) | b;
      int mtrDir = (int)c;
      int messageRecFrom = (int)d;

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
      
      Serial.print("Motor ");
      Serial.print(messageRecFrom);
      Serial.print(" position = ");
      Serial.println(bigNum);
      Serial.print(messageRecFrom);
      Serial.print(" Direction = ");
      Serial.println(mtrDir);
      Serial.println("======");
    }
}

// newSpeed must be formatted as 8 character string example AV-00000
void changeSpeed(String newSpeed, int track)
{
    Serial.println(newSpeed);
    char msg[8] = { ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ' };
    msg[0] = newSpeed[0];
    msg[0] = newSpeed[1];
    msg[0] = newSpeed[2];
    msg[0] = newSpeed[3];
    msg[0] = newSpeed[4];
    msg[0] = newSpeed[5];
    msg[0] = newSpeed[6];
    msg[0] = newSpeed[7];
    Wire.beginTransmission(track);  // in define like TrackA                                              
    Wire.write(msg);                                              
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

void processCommand(String command)
{

    if(command[0] == 'A' || 
    command[0] == 'B' || 
    command[0] == 'C' || 
    command[0] == 'D')              // If it is a speed command to a specific track
    {
        int trackId = getIDFromChar(command[0]);
        if(trackId != 0 && command[1] == 'V')
        {
            changeSpeed(command, trackId);
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
