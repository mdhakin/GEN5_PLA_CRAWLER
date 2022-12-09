#include <AccelStepper.h>
#include <Wire.h>

#define MAX_SPEED 5000
#define motor_id 3
int MTRspeed = 300;
byte b1 = (byte)0;
byte b2 = (byte)0;

int directionVar = 0; // 0 forward  1 rev
AccelStepper stepper;

void WireAction();
void SendData();
void getPositionBytes();
void setDirection();





void getPositionBytes()
{
  int rrr = (int)position1;
    byte myArray[8];
    myArray[0] = (rrr >> 8) & 0xFF;
    myArray[1] = rrr & 0xFF;
    
    b1 = myArray[0];
    b2 = myArray[1];
    return;

}
byte I2C_Command;


int motorcontroller_mode = 0;

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  
 stepper.setMaxSpeed(10000);
 stepper.setSpeed(MTRspeed);

 Wire.begin(motor_id);
 Wire.onReceive(WireAction);
 Wire.onRequest(SendData);
}

void loop() {
  stepper.setSpeed(MTRspeed);
  stepper.runSpeed();
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
    myArray[2] = (byte)1;
  }else{
    myArray[2] = (byte)0;
  }
  
  myArray[3] = (byte)motor_id;
  myArray[4] = (byte)0;
  myArray[5] = (byte)0;
  myArray[6] = (byte)0;
  myArray[7] = (byte)0;
  
  Wire.write(myArray, 8);
}


void WireAction()
{
  char instruction[3] = "   ";
  char instructionValue[5] = "     ";
  int i = 0;
  while (0 < Wire.available())
  {
    char c = Wire.read();
    if(i < 3)
    {
       instruction[i] = c;
    }else{
        instructionValue[i-3] = c;
    }
    i++;
  }
  i = 0;
  String sInstruction = instruction;
  String sInstructionValue = instructionValue;
  int tempSpeed = (int)instructionValue;

  if(sInstruction[1] != 'O' && tempSpeed < MAX_SPEED)
  {
    MTRspeed = tempSpeed;
    return;
  }else if (sInstruction[1] == 'O')  // Command issued is the change direction command
  {
    setDirection();
    return;
  }
  
}

void setDirection()
{
  if(directionVar == 0)
  {
    directionVar = 1;
    stepper.setPinsInverted(true,false,false);
      stepper.runSpeed();
  }else
  {
    directionVar = 0;
    stepper.setPinsInverted(false,false,false);
      stepper.runSpeed();
  }
}
