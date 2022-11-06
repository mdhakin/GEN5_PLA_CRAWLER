#include <AccelStepper.h>
#include <Wire.h>

int MTRspeed = 0;

int backwards = 1;

AccelStepper stepper;

void WireAction();
byte I2C_Command;


void setup() {
  Serial.begin(9600);
 //MTRspeed = 10;
 stepper.setMaxSpeed(800);
 stepper.setSpeed(MTRspeed);

 Wire.begin(3);
 Wire.onReceive(WireAction);
 

}

void loop() {
  // put your main code here, to run repeatedly:
  //delay(300);
  
  stepper.setSpeed(MTRspeed);
  stepper.runSpeed();
}

void WireAction()
{
  I2C_Command = Wire.read();
  int tempSpeed = (int)I2C_Command;
   if(tempSpeed < 81)
   {
    MTRspeed = (int)I2C_Command * 10 * backwards;
    Serial.println(MTRspeed);
   }else
   {
    //backwards = backwards *-1;
    
   }
    
    
  
  
}
