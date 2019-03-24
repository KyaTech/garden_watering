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

unsigned long displayTimer = millis() - 1000;
unsigned long sendTimer = 0;
unsigned long counter = 0;


void setup() {
  Serial.begin(115200);
  radio.beginMesh(0);
  radio.setRequestCallback(requestCallback);
  radio.setResponseCallback(responseCallback);
}

void requestCallback(request_payload payload) {
  printRequest(payload);
}

void responseCallback(response_payload payload) {
  printResponse(payload);
}

void loop() {    
  radio.update();

  if(millis() - displayTimer > 10000){
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

  if (millis() - sendTimer > 10000) {
    sendTimer = millis();

    request_payload payload;
    char request_id[7] = {"000000"};
    radio.generateRequestID(request_id);

    String(request_id).toCharArray(payload.request_id,7);
    String("Moisture").toCharArray(payload.attribute_requested,MAX_CHAR_SIZE);

    if (radio.sendRequest(payload,1)) {
      Serial.print("Send "); 
      printRequest(payload);
    }

    counter++;
  }
}
