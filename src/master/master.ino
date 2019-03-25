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
  radio.setRegistrationCallback(registrationCallback);
}

void registrationCallback(registration_payload payload,RF24NetworkHeader header) {
  printRegistration(payload);
  radio.sendResponse("OK",payload);
}

void requestCallback(request_payload payload,RF24NetworkHeader header) {
  printRequest(payload);
}

void responseCallback(response_payload payload,RF24NetworkHeader header) {
  printResponse(payload);
}

void loop() {    
  radio.update();

  if(millis() - displayTimer > 10000){
    displayTimer = millis();
    radio.printMesh();
  }

  if (millis() - sendTimer > 10000) {
    sendTimer = millis();
    radio.sendRequest("Moisture",1);
    counter++;
  }
}
