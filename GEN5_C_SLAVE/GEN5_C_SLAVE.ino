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
int speed;int DO_Blink = 13;
byte I2C_OnOff;  
void serial_flush(void);
void readstring();

String inputString = "";
bool stringComplete = false; 
int Mtrspeed;
AccelStepper stepper; // Defaults to AccelStepper::FULL4WIRE (4 pins) on 2, 3, 4, 5
 
void setup()
{  
   Mtrspeed = 300;
   Serial.begin(9600);
   stepper.setMaxSpeed(9000);
   stepper.setSpeed(Mtrspeed); 

   
  pinMode(DO_Blink, OUTPUT);                    
  Wire.begin(1);                                
  Wire.onReceive(BlinkLED);   
        
}
 
void loop()
{  
readstring();
  if (stringComplete)
  {
    Serial.println("Receved new speed.");
    Serial.println(inputString);

    int newVal = inputString.toInt();
    Mtrspeed = newVal;
    stepper.setSpeed(Mtrspeed);
    stepper.runSpeed();
    
    
     serial_flush(),
     inputString = "";
     stringComplete = false;
   }
  
   stepper.runSpeed();
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
