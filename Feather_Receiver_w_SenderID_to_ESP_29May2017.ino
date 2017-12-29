/* RFM69 library and code by Felix Rusu - felix@lowpowerlab.com
// Get libraries at: https://github.com/LowPowerLab/
// Make sure you adjust the settings in the configuration section below !!!
// **********************************************************************************
// Copyright Felix Rusu, LowPowerLab.com
// Library and code by Felix Rusu - felix@lowpowerlab.com
// **********************************************************************************
// License
// **********************************************************************************
// This program is free software; you can redistribute it 
// and/or modify it under the terms of the GNU General    
// Public License as published by the Free Software       
// Foundation; either version 3 of the License, or        
// (at your option) any later version.                    
//                                                        
// This program is distributed in the hope that it will   
// be useful, but WITHOUT ANY WARRANTY; without even the  
// implied warranty of MERCHANTABILITY or FITNESS FOR A   
// PARTICULAR PURPOSE. See the GNU General Public        
// License for more details.                              
//                                                        
// You should have received a copy of the GNU General    
// Public License along with this program.
// If not, see <http://www.gnu.org/licenses></http:>.
//                                                        
// Licence can be viewed at                               
// http://www.gnu.org/licenses/gpl-3.0.txt
//
// Please maintain this license information along with authorship
// and copyright notices in any redistribution of this code
// **********************************************************************************/

#include <RH_RF95.h>
#include <SPI.h>
#include <SoftwareSerial.h>
SoftwareSerial ESP(13,12); //RX,TX
char dest[50];
char ser[50];
String Humid;
String Temp;
String Batt;
String ID;
//*********************************************************************************************
// *********** IMPORTANT SETTINGS - YOU MUST CHANGE/ONFIGURE TO FIT YOUR HARDWARE *************
//*********************************************************************************************

//Match frequency to the hardware version of the radio on your Feather
#define RF95_FREQ 915.0

//*********************************************************************************************
#define SERIAL_BAUD   115200

#define RFM95_CS 8
#define RFM95_RST 4
#define RFM95_INT 7

RH_RF95 rf95(RFM95_CS, RFM95_INT);

#define LED           13  // onboard blinky


void setup() {
   pinMode(LED, OUTPUT);     
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);
 
  //while (!Serial);
  Serial.begin(9600);
  delay(100);
  ESP.begin(9600);
  delay(100);
 
  Serial.println("Feather LoRa RX Test!");
  
  // manual reset
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);
  
 while (!rf95.init()) {
    Serial.println("LoRa radio init failed");
    while (1);
  }
  Serial.println("LoRa radio init OK!");
 
  // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM
  if (!rf95.setFrequency(RF95_FREQ)) {
    Serial.println("setFrequency failed");
    while (1);
  }
  Serial.print("Set Freq to: "); Serial.println(RF95_FREQ);
  rf95.setTxPower(23, false);
  
  // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on
 
  // The default transmitter power is 13dBm, using PA_BOOST.
  // If you are using RFM95/96/97/98 modules which uses the PA_BOOST transmitter pin, then 
  // you can set transmitter powers from 5 to 23 dBm:

}

void loop() {

if (rf95.available())
  {
    // Should be a message for us now   
    uint8_t buf[100];
    uint8_t len = sizeof(buf);
    
    if (rf95.recv(buf, &len))
    {
      digitalWrite(LED, HIGH);
      RH_RF95::printBuffer("Received: ", buf, len);
      Serial.print("Got: ");
      Serial.println((char*)buf);
      Serial.print("RSSI: ");
      Serial.println(rf95.lastRssi(), DEC);
      delay(10);
      memcpy(dest, (char*)buf,strlen((char*)buf));
    }

   String message(dest);

   int humidPos = message.indexOf("H")+1;
   int commaHum = message.indexOf(",");
   int tempPos = message.indexOf("T")+1;
   int commaTemp = message.indexOf(",",tempPos);
   int batPos = message.indexOf("B")+1;
   int commaBat = message.indexOf(",",batPos);
   int idPos = message.indexOf("I")+1;
   int commaID = message.indexOf(",",idPos);

   Humid = message.substring(humidPos,commaHum);
   Temp = message.substring(tempPos,commaTemp);
   Batt = message.substring(batPos,commaBat);
   ID = message.substring(idPos,commaID);

   Serial.println(Humid);
   Serial.println(Temp);
   Serial.println(Batt);
   Serial.println(ID);

    Serial.println(Humid);
    ESP.println("ESPreceiveSoil("+Humid+","+ID+")");
    delay(1000);
    ESP.println("ESPreceiveTemp("+Temp+","+ID+")");
    delay(1000);
    ESP.println("ESPreceiveBat("+Batt+","+ID+")");
    delay(1000);
    ESP.println("ESPreceiveID("+ID+")");
    delay(1000);

  Humid = "";
  Temp = "";
  Batt = "";
  ID="";
}
}


