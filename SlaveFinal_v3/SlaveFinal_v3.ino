#include <AccelStepper.h>
#include <Wire.h>

// Used to hold data that comes from the serial port
String readString; 
String LastCommand = "";
String LastCommandValue = "";
// holds serial port incoming messages
//String inputString = "";

// Message from the serial port is ready
//bool stringComplete = false; 

#define MAX_SPEED 5000
#define motor_id 3
int MTRspeed = 300;

byte b1 = (byte)0;
byte b2 = (byte)0;
byte b3 = (byte)0;
byte b4 = (byte)0;
byte b5 = (byte)0;
byte b6 = (byte)0;
byte b7 = (byte)0;
byte b8 = (byte)0;

int directionVar = 0; // 0 forward  1 rev
AccelStepper stepper;

void WireAction();
void SendData();
void setDirection();

// Flush the serial buffer
//void serial_flush(void);

// read input commands
//void readstring();

/*
digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);                       // wait for a second
  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  delay(1000);  


*/

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
 // Serial.begin(9600);
 stepper.setMaxSpeed(10000);
 stepper.setSpeed(MTRspeed);
  LastCommandValue = (String)MTRspeed;
 Wire.begin(motor_id);
 Wire.onReceive(WireAction);
 Wire.onRequest(SendData);
}

void loop() {
  stepper.setSpeed(MTRspeed);
  stepper.runSpeed();
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
  long tt = stepper.currentPosition(); 
  
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
  myArray[6] = (byte)0;//transmitSpeed[2];
  myArray[7] = (byte)0;//transmitSpeed[3];
  
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
  int tempSpeed = sInstructionValue.toInt();
  LastCommand = sInstruction.substring(0,3);
  if(sInstruction[1] == 'V')
  {
    MTRspeed = tempSpeed;
    LastCommandValue = (String)MTRspeed;
    //return;
  }else if (sInstruction[1] == 'O')  // Command issued is the change direction command
  {
    setDirection();
    LastCommandValue = sInstructionValue.substring(0,5);
    
    //return;
  }
  instruction[0] = ' ';
  instruction[1] = ' ';
  instruction[2] = ' ';
  return;
}

void setDirection()
{
  if(directionVar == 0)
  {
    digitalWrite(LED_BUILTIN, LOW);
    directionVar = 1;
    stepper.setPinsInverted(true,false,false);
    stepper.runSpeed();
  }else if(directionVar == 1)
  {
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
