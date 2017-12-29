
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
// **********************************************************************************/  //get it here: https://www.github.com/lowpowerlab/rfm69

#include <Adafruit_SleepyDog.h>
#include <RH_RF95.h>
#include <I2CSoilMoistureSensor.h>
#include <Wire.h>

I2CSoilMoistureSensor sensor;

//*********************************************************************************************
// *********** IMPORTANT SETTINGS - YOU MUST CHANGE/ONFIGURE TO FIT YOUR HARDWARE *************
//*********************************************************************************************

#define NODEID        3    // The unique identifier of this node

//*********************************************************************************************
#define SERIAL_BAUD   115200

#define RFM95_CS 8
#define RFM95_RST 4
#define RFM95_INT 7

#define RF95_FREQ 915.0
 
// Singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);

#define LED           13  // onboard blinky
#define VBATPIN       9  //battery pin



int wakeUp = 720; //720 is for 1 hour; //60 for 5 minutes
int sleepCount = 0;

float tempC = 0;
float humidity = 0;

void setup() {
  Serial.begin(SERIAL_BAUD);
  Wire.begin();
  sensor.begin();
  delay(1000);
//while (!Serial){
    //nada
 // }
  Serial.println("Feather LoRa TX Test!");
 
  // manual reset
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);

  // Initialize radio
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

char combinedArray[100]={};
char BATT[20]={};
char Hum[20]={};
char Tem[20]={};
char IDD[20]={};
delay(10);  
rf95.sleep();  //sleeping portion of code
Watchdog.sleep(5000);  //max sleepydog is 8000.  have to run multiple loops for longer delay
sleepCount++;  //counter for sleep loops.

if (sleepCount >= wakeUp)
{//------------------------------start if sleep count
while (sensor.isBusy()) delay(50);
humidity = 0.3155*(sensor.getCapacitance())-104.73;
tempC = sensor.getTemperature()/(float)10;
Serial.println(tempC);
float measuredvbat = analogRead(VBATPIN);
measuredvbat *= 2;    // we divided by 2, so multiply back
measuredvbat *= 3.3;  // Multiply by 3.3V, our reference voltage
measuredvbat /= 1024; // convert to voltage

String battery = String(measuredvbat);
String humid = String(humidity);
String temper = String(tempC);
String BAT = "B"+battery+",";
String Humidity = "H"+humid+",";
String Temperature ="T"+temper+",";
String ID = String(NODEID);
String node = "I"+ID+",";

  //humid.toCharArray(Hum,20);
  Humidity.toCharArray(Hum,sizeof(Hum));
  Temperature.toCharArray(Tem,sizeof(Tem));
  BAT.toCharArray(BATT,sizeof(BATT));
  node.toCharArray(IDD,sizeof(IDD));
  strcat(combinedArray, Hum);
  
  strcat(combinedArray,Tem);
  strcat(combinedArray, BATT);
  strcat(combinedArray, IDD);
 


  for(int i=0;i<21;i++){
    if (i==5 || i==10 || i==15 || i==20){
    Serial.println("Sending..."); 
    delay(10);
    rf95.send(combinedArray+'\0',strlen(combinedArray)+1);
    Serial.println(combinedArray);
    Serial.println("Waiting for packet to complete..."); 
    delay(10);
    rf95.waitPacketSent();
    }
     delay(1000);
    }
  
  sleepCount=0;
  humidity=0;
  tempC=0;
  

} //-------------end if sleep count
}

/*void Blink(byte PIN, byte DELAY_MS, byte loops)
{
  for (byte i=0; i<loops; i++)
  {
    digitalWrite(PIN,HIGH);
    delay(DELAY_MS);
    digitalWrite(PIN,LOW);
    delay(DELAY_MS);
  }
}


