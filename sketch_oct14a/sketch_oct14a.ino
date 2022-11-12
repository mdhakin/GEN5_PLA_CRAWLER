// ConstantSpeed.pde
// -*- mode: C++ -*-
//
// Shows how to run AccelStepper in the simplest,
// fixed speed mode with no accelerations
/// \author  Mike McCauley (mikem@airspayce.com)
// Copyright (C) 2009 Mike McCauley
// $Id: ConstantSpeed.pde,v 1.1 2011/01/05 01:51:01 mikem Exp mikem $
 
#include <AccelStepper.h>
#include <Wire.h> 
void BlinkLED(int Press);
void setDirection();
int speed;int DO_Blink = 13;
byte I2C_OnOff;  
void serial_flush(void);
void readstring();
int directionVar = 0; // 0 forward  1 rev
String inputString = "";
bool stringComplete = false; 
int Mtrspeed;
AccelStepper stepper; // Defaults to AccelStepper::FULL4WIRE (4 pins) on 2, 3, 4, 5

int printLocationCnt = 0;
void setup()
{  
 
   Mtrspeed = 100;
   Serial.begin(9600);
   Serial.println("Hello from arduino");
   stepper.setMaxSpeed(1000);
   stepper.setAcceleration(1000.0);
   stepper.setSpeed(Mtrspeed); 

   
  pinMode(DO_Blink, OUTPUT);                    
  Wire.begin(1);                                
  Wire.onReceive(BlinkLED);   
  stepper.setMaxSpeed(750);
  
  stepper.setSpeed(Mtrspeed);  

        
}

/*
if(inputString.substring(0,2) == "cd")
    {
      stepper.setPinsInverted(true,false,false);
      stepper.runSpeed();
    }else
    {

*/

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
 
void loop()
{  
  if(printLocationCnt >= 25000)
  {
    Serial.println(stepper.currentPosition());
    printLocationCnt = 0;
  }
  printLocationCnt++;
readstring();
  if (stringComplete)
  {
    Serial.println("Receved new speed.");
    Serial.println(inputString);


    if(inputString.substring(0,2) == "cd")
    {
      setDirection();
    }else
    {
      if (inputString.toInt() > 580 && inputString.toInt() <630)
      {
        inputString = "631";
      }
      int newVal = inputString.toInt();
    Mtrspeed = newVal;
    stepper.setSpeed(Mtrspeed);
    stepper.runSpeed();
    }
      
    
    
    
    
     serial_flush(),
     inputString = "";
     stringComplete = false;
   }
  
   stepper.runSpeed();
//Serial.println("Hello from arduino");
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

void BlinkLED(int Press)
{
  I2C_OnOff = Wire.read();                      
  if (I2C_OnOff == 1)
  {
   digitalWrite(DO_Blink, HIGH);  
   stepper.setSpeed(0);
    stepper.runSpeed();               
  }
  else if (I2C_OnOff == 0)
  {
   digitalWrite(DO_Blink, LOW);
   stepper.setSpeed(600);
    stepper.runSpeed();                 
  } 
}
