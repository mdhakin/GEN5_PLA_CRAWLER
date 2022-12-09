#include <AccelStepper.h>
#include <Wire.h>


void setDirection();
int MTRspeed = 300;

int backwards = 1;
int directionVar = 0; // 0 forward  1 rev
AccelStepper stepper;

void WireAction();
void SendData();
byte I2C_Command;

// if == 0 then the setting is for the A4988 controllers
// if == 1 then the setting is for the TMC controllers 
int motorcontroller_mode = 0;

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  
 stepper.setMaxSpeed(10000);
 stepper.setSpeed(MTRspeed);

 Wire.begin(6);
 Wire.onReceive(WireAction);
 Wire.onRequest(SendData);

}

void loop() {
  // put your main code here, to run repeatedly:
  

  stepper.setSpeed(MTRspeed);
  stepper.runSpeed();
}
void SendData()
{
  long tt = stepper.currentPosition(); 
  
   
    int rrr = (int)tt;
    byte myArray[2];
    myArray[0] = (rrr >> 8) & 0xFF;
    myArray[1] = rrr & 0xFF;
    
    Wire.write(myArray, 2);

}
void WireAction()
{
  I2C_Command = Wire.read();
  int tempSpeed = (int)I2C_Command;
   if(tempSpeed < 251)
   {
    if(motorcontroller_mode == 1)
    {
      MTRspeed = ((int)I2C_Command * 10 * backwards) * 2;
    }else
    {
      MTRspeed = ((int)I2C_Command * 10 * backwards);
    }
    
    
   }else if(tempSpeed == 254)
   {
    setDirection();
    
   }else if(tempSpeed == 253)
   {
    motorcontroller_mode = 1;
    
   }else if(tempSpeed == 252)
   {
    motorcontroller_mode = 0;
    
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
