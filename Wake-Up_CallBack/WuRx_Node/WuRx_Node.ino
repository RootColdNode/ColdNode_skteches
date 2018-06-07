/**
 * Copyright (c) 2018 ColdNode <contact@coldnode.com>
 * 
 * This file is part of the ColdNode project.
 * 
 * ColdNode is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * any later version.
 * 
 * ColdNode is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with ColdNode; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 
 * USA
 * 
 * Author: Sadok Bdiri
 * Creation date: 07/06/2018
 */
#include "HardwareSerial.h"

#define RFCHANNEL        0       // Let's use channel 0
#define SYNCWORD1        0xD3    // Synchronization word, high byte
#define SYNCWORD0        0x91    // Synchronization word, low byte
#define SOURCE_ADDR      0x97    // Sender address
#define WuRX_ADDR        0xD697  // WuRx decoder address


CCPACKET txPacket;
/**
 * This function is called whenever a wireless packet is received
 */
void rfPacketReceived(CCPACKET *packet)
{
  
}

/*
 * WuRx function definition
 */
void wakeUpNow()              // here the interrupt is handled after wakeup
{ 
  detachInterrupt(IntP);      // disables interrupt 0 on pin 2 so the
  panstamp.wakeUp();
  
}
void setup()
{

  /*
   * Wake up Setup
   */
   
  Serial.begin(9600);
  Serial.print("Booting\n");
  
  digitalWrite(Green_LED, HIGH);
  digitalWrite(Red_LED, HIGH);
  digitalWrite(Blue_LED, HIGH);
  delay(1000);
  digitalWrite(Green_LED, LOW);
  digitalWrite(Red_LED, LOW);
  digitalWrite(Blue_LED, LOW);
  //pinMode(A0, OUTPUT);
  
  // Init RF IC
 
  coldnode.radio.setChannel(RFCHANNEL);
  coldnode.radio.setSyncWord(SYNCWORD1, SYNCWORD0);
  coldnode.radio.setDevAddress(SOURCE_ADDR);
  coldnode.setPacketRxCallback(rfPacketReceived);     // Declare RF callback function
                         
 //Enable WuRx//
  coldnode.wurx.powerOn();
  coldnode.wurx.InitAS3933();
  coldnode.wurx.setSourceWakeUpPacketAddress(WuRX_ADDR);
  coldnode.wurx.clearWake();                    
  //Attaching the interrupt to the WuRx's pin
  attachInterrupt(IntP, wakeUpNow, RISING);

  for (int i=0; i<4;i++){
    digitalWrite(Green_LED, !digitalRead(Green_LED));
    delay(50);
  }
  Serial.print("Sleeping...\n");
  Serial.end();
  coldnode.switchWuRxToAntenna(); // Redirect the Antenna to the WuRx 
  coldnode.sleep();                                   // Power-down the node   
}
void loop(){
  digitalWrite(9, HIGH);
  Serial.begin(9600);
  Serial.print("Woke-Up!");
  delay(1000);
  sendACK();
  Serial.end();
  attachInterrupt(IntP, wakeUpNow, RISING);
  coldnode.sleep();
  

}
void sendACK(){
  
  coldnode.switchCC430ToAntenna();
  coldnode.radio.setTxPowerAmp(0x8D,0); // Maximum Transmission power for OOK
  txPacket.length = 2;  // Let's send a single data byte plus the destination address
  txPacket.data[0] = 0x96;   // First data byte has to be the destination address
  txPacket.data[1] = 0xAA;            // Self-incrementing value       
  coldnode.radio.sendData(txPacket);     // Transmit packet
  for (int i=0; i<4;i++){
    digitalWrite(Blue_LED, !digitalRead(Blue_LED));
    delay(50);
  }
 }


