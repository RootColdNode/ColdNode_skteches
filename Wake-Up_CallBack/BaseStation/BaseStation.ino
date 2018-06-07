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
#define SOURCE_ADDR      0x96    // Sender address
#define WuRX_ADDR        0xD696  // Decoder Address

         
/*
 * Global variables
 */
CCPACKET txPacket;               // packet object
CCPACKET fullPacket;
/**
 * This function is called whenever a wireless packet is received
 */
void rfPacketReceived(CCPACKET *packet)
{   
  Serial.print("Received!\n");
  if (packet->length > 1)
  {
    Serial.print("Received Acknowledgment");
  }
}

void setup()
{
  Serial.begin(9600);
  Serial.print("Booting\n");

  digitalWrite(Green_LED, HIGH);
  digitalWrite(Red_LED, HIGH);
  digitalWrite(Blue_LED, HIGH);
  delay(1000);
  digitalWrite(Green_LED, LOW);
  digitalWrite(Red_LED, LOW);
  digitalWrite(Blue_LED, LOW);
  
  // Init RF IC
  coldnode.radio.setChannel(RFCHANNEL);
  coldnode.radio.setSyncWord(SYNCWORD1, SYNCWORD0);
  coldnode.radio.setDevAddress(SOURCE_ADDR);
  coldnode.radio.setTxPowerAmp(0xC0,0); // Maximum Transmission power for OOK
  coldnode.switchCC430ToAntenna();
  
  coldnode.wurx.setDestWakeUpPacketAddress_Man(&txPacket,0xD697);
  for (int i=0; i<4;i++){
    digitalWrite(9, !digitalRead(9));
    delay(50);
  }

  Serial.print("Sleeping...\n");
  coldnode.setPacketRxCallback(rfPacketReceived);     // Declare RF callback function
}

void loop()
{
    Serial.println("Broadcast WuPt");
    coldnode.sendWuPacket(txPacket, fullPacket); // fullpacket is needed to construct the preamble of the wake-up packet, temporarily and ugly for now and will be removed, so don't be concerned about it :)
    coldnode.switchCC430ToAntenna();
    for (int i=0; i<4;i++){
      digitalWrite(9, !digitalRead(9));
      delay(50);
    }
    delay(2000);
}


