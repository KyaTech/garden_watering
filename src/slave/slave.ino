
/**
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

#define Moisture
#define Valves


#ifdef Valves
int valvePins[] = {2,3};
int valvePinsLength = (sizeof(valvePins) / sizeof(int));
#endif


void setup() {
  Serial.begin(115200);
  radio.beginMesh(nodeID);
  radio.setRequestCallback(requestCallback);
  radio.setResponseCallback(responseCallback);
  radio.setCommandCallback(commandCallback);
  radio.registrate(registrationProcess);
  
  #ifdef Valves
  for (int i = 0; i < valvePinsLength; i++) {
    pinMode(valvePins[i],OUTPUT);
  }
  
  #endif
}

void registrationProcess() {
  #ifdef Moisture
  radio.waitForAnswer(radio.sendRegistration(ModuleType::SENSOR));
  #endif
  #ifdef Valves
  for (int i = 0; i < valvePinsLength; i++) {
      radio.waitForAnswer(radio.sendRegistration(ModuleType::VALVE,i,valvePins[i]));
  }
  #endif
}

void commandCallback(command_payload payload,RF24NetworkHeader header) {
  printCommand(payload);

  // area for accepting valve controls
  #ifdef Valves

  String command = String(payload.command);
  if (command == "ON") {
    int index = String(payload.additional_value).toInt();

    if (index > (valvePinsLength - 1)) {
      radio.sendSimpleResponse(SimpleResponse::ERROR,payload,header);
    } else {
      digitalWrite(valvePins[index],HIGH);
      radio.sendSimpleResponse(SimpleResponse::OK,payload,header);
    }

  } else if (command == "OFF") {
    int index = String(payload.additional_value).toInt();

    if (index > (valvePinsLength - 1)) {
      radio.sendSimpleResponse(SimpleResponse::ERROR,payload,header);
    } else {
      digitalWrite(valvePins[index],LOW);
      radio.sendSimpleResponse(SimpleResponse::OK,payload,header);
    }
  } else {
    radio.sendSimpleResponse(SimpleResponse::ERROR,payload,header);
  }
  #endif
}

void requestCallback(request_payload payload, RF24NetworkHeader header) {
  printHeader(header);
  printRequest(payload);

  String attribute_requested = String(payload.attribute_requested);


  if (attribute_requested == "Battery") {
    radio.sendResponse(String(random(0,100)),payload,header);
  } 
  // area for accepting mositure-requests
  #ifdef Moisture
  else if (attribute_requested == "Moisture") {
    radio.sendResponse(String(random(16,26)),payload,header);
  } 
  #endif
  else {
    radio.sendSimpleResponse(SimpleResponse::ERROR,payload,header);
  }
    
}

void responseCallback(response_payload payload,RF24NetworkHeader header) {
  printResponse(payload);
}

void loop() {
  radio.update();
}