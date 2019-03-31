 
 
 /** RF24Mesh_Example_Master.ino by TMRh20
  * 
  * Note: This sketch only functions on -Arduino Due-
  *
  * This example sketch shows how to manually configure a node via RF24Mesh as a master node, which
  * will receive all data from sensor nodes.
  *
  * The nodes can change physical or logical position in the network, and reconnect through different
  * routing nodes as required. The master node manages the address assignments for the individual nodes
  * in a manner similar to DHCP.
  *
  */
  
#include <RF24/RF24.h>
#include <RF24Network/RF24Network.h>
#include "RF24Mesh/RF24Mesh.h"
#include "examples_RPi/includes_rpi/names.h"
#include "examples_RPi/includes_rpi/Radio.h"
#include <iostream>
#include <string>
#include <sstream>
using namespace std;

Radio radio = Radio();

unsigned long displayTimer = millis() - 1000;
unsigned long sendTimer = 0;
unsigned long counter = 0;
unsigned long actionTimer = 0;

bool toggle = false;

void registrationCallback(registration_payload payload,RF24NetworkHeader header) {
  printRegistration(payload,radio.getNodeID(header.from_node));
  radio.sendSimpleResponse(SimpleResponse::OK,payload,header);
}

void requestCallback(request_payload payload,RF24NetworkHeader header) {
  printRequest(payload);
}

void responseCallback(response_payload payload,RF24NetworkHeader header) {
  printResponse(payload);
}

int main(int argc, char** argv) {
  
  radio.beginMesh(0);
  radio.setRequestCallback(requestCallback);
  radio.setResponseCallback(responseCallback);
  radio.setRegistrationCallback(registrationCallback);

  while(1) {
  
    radio.update();

    if(millis() - displayTimer > 10000){
      displayTimer = millis();
      radio.printMesh();
    }

    if(millis() - actionTimer > 5000) {
      actionTimer = millis();
      if(toggle) {
        radio.sendCommand("ON","0",1);
      } else {
        radio.sendCommand("OFF","0",1);
      }
      toggle = !toggle;
    }
  
  }

  return 0;
}

      
      
      
