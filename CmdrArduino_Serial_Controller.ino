/********************
* Based on CMDR Library
* This program is based on UART serial communication
* It has been written with intention of converting
* to a full blown web based DCC controller 
*
* Written by Damian Glinojecki
* 
********************/

#include <DCCPacket.h>
#include <DCCPacketQueue.h>
#include <DCCPacketScheduler.h>


DCCPacketScheduler dps;
unsigned int analog_value;
char speed_byte, old_speed = 0;
byte count = 0;
byte prev_state = 1;
byte F0 = 0;
int train_number = 0;

// keeps the speed saved in memory for each train.
// this is a temporary solution
// in the future the program will accept a number which it 
// is supposed to set the speed to. (much better this way)
int train_speed[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

void setup() {
  Serial.begin(115200);
  dps.setup();
}

void SerialRead()
{
  // see if serial is availale 
  if(Serial.available())
  {
    // read the serial
    char temp = Serial.read();
    {
      if(temp == '+')
      {
        train_speed[train_number]++;
        // make sure the speed is never 0
        if(train_speed[train_number] == 0)
          train_speed[train_number]++; 
        if(train_speed[train_number] > 126)
          train_speed[train_number] = 126;
        Serial.print("Train Speed: ");
        Serial.println(train_speed[train_number]);
        dps.setSpeed128(train_number,DCC_SHORT_ADDRESS,train_speed[0]);     
      }
      else if(temp == '-')
      {
        train_speed[train_number]--;
        
        // make sure the speed is never 0
        if(train_speed[train_number] == 0)
          train_speed[train_number]--;    
        // make sure the speed is never less than 126 (max negative speed)  
        if(train_speed[train_number] < -126)
          train_speed[train_number] = -126;
          
        Serial.print("Train Speed: ");
        Serial.println(train_speed[train_number]);
        dps.setSpeed128(train_number,DCC_SHORT_ADDRESS,train_speed[0]);
      }
      else if(temp == 'e')
      {
        // emergency stop
        dps.eStop();
        Serial.println("Emergency Stop");
      }
      else if(temp == 'b')
      {
        // toggle all the functions (for now)
        F0 ^= 1;
        Serial.println(F0,BIN);
        dps.setFunctions0to4(train_number,DCC_SHORT_ADDRESS,F0);
        dps.setFunctions5to8(train_number,DCC_SHORT_ADDRESS,F0);
        dps.setFunctions9to12(train_number,DCC_SHORT_ADDRESS,F0);
      }
      else if(temp == 't')
      {
          delay(100);
          char temp_num[2];
          temp_num[0] = Serial.read();
          temp_num[1] = Serial.read();
          train_number = atoi(temp_num);
          Serial.print("read in: ");
          Serial.print(temp_num[0]);
          Serial.println(temp_num[1]);
          Serial.print("train number: ");
          Serial.println(train_number);    
   
      }
      
    }
    
  }
  
}

void loop() {
  
  SerialRead();
  dps.update();
  
  ++count;
}
