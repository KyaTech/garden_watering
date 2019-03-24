
/** RF24Mesh_Example.ino by TMRh20

   This example sketch shows how to manually configure a node via RF24Mesh, and send data to the
   master node.
   The nodes will refresh their network address as soon as a single write fails. This allows the
   nodes to change position in relation to each other and the master node.
*/

#include "RF24.h"
#include "RF24Network.h"
#include "RF24Mesh.h"
#include <SPI.h>

#include "C:\Users\aykbo\OneDrive\Dokumente\Arduino Projektkurs\main_project\src\includes\names.h"
#include "C:\Users\aykbo\OneDrive\Dokumente\Arduino Projektkurs\main_project\src\includes\Radio.h"
//#include <printf.h>




/**
   User Configuration: nodeID - A unique identifier for each radio. Allows addressing
   to change dynamically with physical changes to the mesh.

   In this example, configuration takes place below, prior to uploading the sketch to the device
   A unique value from 1-255 must be configured for each node.
   This will be stored in EEPROM on AVR devices, so remains persistent between further uploads, loss of power, etc.

 **/
#define nodeID 1
Radio radio;

uint32_t displayTimer = 0;
unsigned long counter = 0;


void setup() {
  Serial.begin(115200);
  radio.beginMesh(nodeID);
  radio.setRequestCallback(requestCallback);
  radio.setResponseCallback(responseCallback);
}

void requestCallback(request_payload payload) {
  printRequest(payload);

  response_payload response;
  sprintf(response.request_id,"%s",payload.request_id);
  String(random(14,23)).toCharArray(response.value,MAX_CHAR_SIZE);
  radio.sendResponse(response,0);

  Serial.print("Send ");
  printResponse(response);
}

void responseCallback(response_payload payload) {
  printResponse(payload);
}

void loop() {
  radio.update();
}