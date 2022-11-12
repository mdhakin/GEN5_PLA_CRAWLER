#include <AccelStepper.h>
#include <Wire.h>
void setDirection();
int MTRspeed = 0;

int backwards = 1;
int directionVar = 0; // 0 forward  1 rev
AccelStepper stepper;

void WireAction();
byte I2C_Command;


void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  //Serial.begin(9600);
 //MTRspeed = 10;
 stepper.setMaxSpeed(800);
 stepper.setSpeed(MTRspeed);

 Wire.begin(3);
 Wire.onReceive(WireAction);
 

}

void loop() {
  // put your main code here, to run repeatedly:
  //delay(300);
/*
if(MTRspeed > 50)
{
  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);                       // wait for a second
  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  delay(1000);
}else
{
  digitalWrite(LED_BUILTIN, LOW);
}
  */
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
    //Serial.println(MTRspeed);
   }else if(tempSpeed == 200)
   {
    setDirection();
    
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
