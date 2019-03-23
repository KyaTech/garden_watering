 /** RF24Mesh_Example_Master.ino by TMRh20
  * 
  *
  * This example sketch shows how to manually configure a node via RF24Mesh as a master node, which
  * will receive all data from sensor nodes.
  *
  * The nodes can change physical or logical position in the network, and reconnect through different
  * routing nodes as required. The master node manages the address assignments for the individual nodes
  * in a manner similar to DHCP.
  *
  */
  
  
#include "RF24Network.h"
#include "RF24.h"
#include "RF24Mesh.h"
#include <SPI.h>
//Include eeprom.h for AVR (Uno, Nano) etc. except ATTiny
#include <EEPROM.h>

#include "C:\Users\aykbo\OneDrive\Dokumente\Arduino Projektkurs\main_project\src\includes\names.h"
#include "C:\Users\aykbo\OneDrive\Dokumente\Arduino Projektkurs\main_project\src\includes\Radio.h"

/***** Configure the chosen CE,CS pins *****/
Radio radio;

uint32_t displayTimer = 0;
uint32_t sendTimer = 0;

unsigned long counter = 0;


void setup() {
  Serial.begin(115200);
  radio.beginMesh(0);
}


void loop() {    
  radio.update();
  radio.DHCP();
  
  
  // Check for incoming data from the sensors
  if(radio.packageAvailable()){
    RF24NetworkHeader header = radio.peekHeader();
    
    uint32_t dat=0;
    switch(header.type) {
      payload_t payload;
      case 'P': 
        payload = radio.readMillisPayload();
        Serial.print("Received packet #");
        Serial.print(payload.counter);
        Serial.print(" at ");
        Serial.println(payload.ms);
        break;
      default: radio.getNetwork().read(header,0,0); Serial.println(header.type);break;
    }
  }
  
  if(millis() - displayTimer > 5000){
    displayTimer = millis();
    Serial.println(" ");
    Serial.println(F("********Assigned Addresses********"));
     for(int i=0; i < radio.getMesh().addrListTop; i++){
       Serial.print("NodeID: ");
       Serial.print(radio.getMesh().addrList[i].nodeID);
       Serial.print(" RF24Network Address: 0");
       Serial.println(radio.getMesh().addrList[i].address,OCT);
     }
    Serial.println(F("**********************************"));
    Serial.println(" ");
  }

  if (millis() - sendTimer > 2000) {
    sendTimer = millis();
    payload_t payload = {sendTimer,counter};

    if (radio.sendMillisPayload(payload,1)) {
      Serial.println("Send packet #" + String(payload.counter) + " at " + String(payload.ms) + " sucessfully");
    }

    counter++;
  }
}
