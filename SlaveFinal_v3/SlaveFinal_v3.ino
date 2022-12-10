#include <AccelStepper.h>
#include <Wire.h>

// Used to hold data that comes from the serial port
String readString; 

long TrackPosition = 0;
// holds serial port incoming messages
//String inputString = "";

// Message from the serial port is ready
//bool stringComplete = false; 

#define MAX_SPEED 5000
#define motor_id 3
#define Status1 7  // Is in motion
#define Status2 8  // unassigned status signal

int MTRspeed = 300;
int Aceleration = 100;

byte b1 = (byte)0;
byte b2 = (byte)0;
byte b3 = (byte)0;
byte b4 = (byte)0;
byte b5 = (byte)0;
byte b6 = (byte)0;
byte b7 = (byte)0;
byte b8 = (byte)0;

unsigned long startMillis;  //some global variables available anywhere in the program
unsigned long currentMillis;
const unsigned long period = 1000;  //the value is a number of milliseconds
unsigned long ticks = 0;
const long distanceInterval = 50;

int directionVar = 0; // 0 forward  1 rev
AccelStepper stepper;

void WireAction();
void SendData();
void setDirection();
void setcurrentposition(long newPosition);
void CalculatePosition();
// Flush the serial buffer
//void serial_flush(void);

// read input commands
//void readstring();













void setup() {
  startMillis = millis();  //initial start time
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(Status2, OUTPUT);
  pinMode(Status1, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  digitalWrite(Status2, LOW);
  digitalWrite(Status1, LOW);
 // Serial.begin(9600);
 stepper.setMaxSpeed(10000);
 stepper.setSpeed(MTRspeed);
 stepper.setAcceleration	((float)Aceleration);
 setcurrentposition(0);
 stepper.runSpeed();
 
 Wire.begin(motor_id);
 Wire.onReceive(WireAction);
 Wire.onRequest(SendData);
}











void loop() {
  stepper.setSpeed(MTRspeed);
  stepper.runSpeed();
  
  if (currentMillis - startMillis >= period)  //test whether the period has elapsed
  {
    ticks++;
    CalculatePosition();
    if(stepper.isRunning())
    {
        digitalWrite(Status1, HIGH);
    }else
    {
      digitalWrite(Status1, LOW);
    }
    startMillis = currentMillis;  //IMPORTANT to save the start time of the current LED state.

  }
/*
  readstring();
    inputString.toUpperCase();
   
    if (stringComplete)
    {
          Serial.println("Processing inputString");

          if(inputString[0] == 'R')
          {
            Serial.print("Current Speed = ");
            Serial.println(MTRspeed);

            Serial.print("Direction = ");
            Serial.println(directionVar);
          }else if(inputString[0] == 'L')
          {
            Serial.print("Last Command = ");
            Serial.println(LastCommand);
            Serial.print("Last Command Value = ");
            Serial.println(LastCommandValue);
          }
          inputString = "";
          stringComplete = false;
           
    }
    */
}










void SendData()
{
  long tt = (TrackPosition / distanceInterval); 
  
  int rrr = (int)tt;
  byte myArray[8];
  myArray[0] = (rrr >> 8) & 0xFF;
  myArray[1] = rrr & 0xFF;
  if(directionVar == 1)
  {
    myArray[2] = (byte)directionVar;
  }else{
    myArray[2] = (byte)directionVar;
  }

  String transmitSpeed = (String)MTRspeed;
  
  myArray[3] = (byte)motor_id;
  myArray[4] = (byte)transmitSpeed[0];
  myArray[5] = (byte)transmitSpeed[1];
  myArray[6] = (byte)0;
  myArray[7] = (byte)0;
  
  Wire.write(myArray, 8);
}













void WireAction(int howmany)
{
  char instruction[3] = { ' ', ' ', ' ' };
  char instructionValue[5] = { ' ', ' ', ' ', ' ', ' ' };
  int i = 0;
  while (0 < Wire.available())
  {
    char c = Wire.read();
    if(i < 3)
    {
       instruction[i] = c;
    }else if(i >=3 && i < 8){
        instructionValue[i-3] = c;
    }
    c = ' ';
    i++;
    
  }
  i = 0;
  
  String sInstruction = instruction;
  String sInstructionValue = instructionValue;
  int tempValue = sInstructionValue.toInt();
  
  if(sInstruction[1] == 'V')
  {
    MTRspeed = tempValue;
    stepper.setSpeed(MTRspeed);
  
    stepper.runSpeed();
   
  }else if (sInstruction[1] == 'O')  // Command issued is the change direction command
  {
    setDirection();
   
  }else if (sInstruction[0] == 'S' && sInstruction[1] == 'C') 
  {
    long newPosition = (long)tempValue;
    setcurrentposition(newPosition);
    
  }else if (sInstruction[1] == 'A')  // Command issued is the change Acceleration command
  {
    Aceleration = sInstructionValue.toInt();
    stepper.setAcceleration((float)Aceleration);
    
  }
  instruction[0] = ' ';
  instruction[1] = ' ';
  instruction[2] = ' ';
  return;
}











void setcurrentposition(long newPosition)
{
  stepper.setCurrentPosition(newPosition);
}






void CalculatePosition()
{
  if(directionVar == 0)
  {
    TrackPosition = TrackPosition + stepper.currentPosition();
    setcurrentposition(0);
    
   
  }else if(directionVar == 1)
  {
    TrackPosition = TrackPosition - stepper.currentPosition();
    setcurrentposition(0);
    
  }
}













void setDirection()
{
  if(directionVar == 0)
  {
    TrackPosition = TrackPosition + stepper.currentPosition();
    setcurrentposition(0);
    digitalWrite(LED_BUILTIN, LOW);
    directionVar = 1;
    
    stepper.setPinsInverted(true,false,false);
    stepper.runSpeed();
  }else if(directionVar == 1)
  {
    TrackPosition = TrackPosition - stepper.currentPosition();
    setcurrentposition(0);
    digitalWrite(LED_BUILTIN, HIGH);
    directionVar = 0;
    stepper.setPinsInverted(false,false,false);
    stepper.runSpeed();
  }
}
/*
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
*/
