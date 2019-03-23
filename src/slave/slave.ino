
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
}

void loop() {

  radio.update();

  // Send to the master node every second
  if (millis() - displayTimer >= 1000) {
    displayTimer = millis();
    payload_t payload = {displayTimer, counter};
    if (radio.sendMillisPayload(payload,0)) {
      Serial.println("Send packet #" + String(payload.counter) + " at " + String(payload.ms) + " sucessfully");
    }
    counter++;
  }

  while (radio.packageAvailable()) {
    RF24NetworkHeader header = radio.peekHeader();
    switch (header.type) {
      payload_t payload;
      case 'P':
        payload = radio.readMillisPayload();
        Serial.print("Received packet #");
        Serial.print(payload.counter);
        Serial.print(" at ");
        Serial.println(payload.ms);
        break;
      default: 
        radio.getNetwork().read(header,0,0);
        break;
    }
  }
}