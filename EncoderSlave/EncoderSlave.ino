#include <Wire.h>  

void WireAction();
void DataSend();
byte I2C_Command;
volatile long x1 , counter1 = 0;
volatile long x2 , counter2 = 0;
void A();
void B();

void C();
void D();

byte turn = 0;

void setup() {
  Serial.begin(9600);
  pinMode(2, INPUT_PULLUP); // 0
  pinMode(3, INPUT_PULLUP); // 1
  pinMode(20, INPUT_PULLUP); // 2
  pinMode(21, INPUT_PULLUP); // 3
  
  attachInterrupt(0, A, RISING);
  attachInterrupt(1, B, RISING);
  attachInterrupt(2, C, RISING);
  attachInterrupt(3, D, RISING);
  
  Wire.begin(11);
  
  Wire.onRequest(DataSend);
  Wire.onReceive(WireAction);
}

void loop() {
  // put your main code here, to run repeatedly:
  if(counter1 != x1)
  {
    x1 = counter1;
    Serial.print("Counter1 ");
    Serial.println(x1);
  }
  
  if( counter2 != x2 )
  {
    x2 = counter2;
    Serial.print("Counter2 ");
    Serial.println(x2);
    
  }
}

void A() 
{
  if(digitalRead(3)==LOW) 
  {
    counter1++;
  }else
  {
    counter1--;
  }
}
   
void B() 
{
  if(digitalRead(2)==LOW) 
  {
    counter1--;
  }else
  {
    counter1++;
  }
}

void C() 
{
  if(digitalRead(20)==LOW) 
  {
    counter2++;
  }else
  {
    counter2--;
  }
}

void D() 
{
  if(digitalRead(21)==LOW) 
  {
    counter2--;
  }else
  {
    counter2++;
  }
}

void DataSend()
{
  long tt;
   if(turn == 0)
   {
    tt = x1;
    turn = 1;
   }else
   {
    tt = x2;
    turn = 0;
   }
   
    int rrr = (int)tt;
    byte myArray[2];
    myArray[0] = (rrr >> 8) & 0xFF;
    myArray[1] = rrr & 0xFF;
    
    Wire.write(myArray, 2);
}

void WireAction()
{
  return;
}
