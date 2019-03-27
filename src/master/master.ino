/**
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
  radio.sendSimpleResponse(SimpleResponse::OK,payload,header);
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

  if (Serial.available() > 0) {
    String s = readStringFromSerial();

    if (s.startsWith("ON") || s.startsWith("OFF")) {
      String command = s.substring(0,s.indexOf(" "));
      String index = s.substring(s.indexOf(" ") + 1, s.length());
      radio.sendCommand(command,index,1);
    } else {
      radio.sendRequest(s,1);
    }
  }

  
}

String readStringFromSerial() {
  String ret;
  int c = readByte();
  while (c >= 0)
  {
    ret += (char)c;
    c = readByte();
  }
  return ret;
}

int readByte()
{
  int c = Serial.read();
  if (c < 0) {
    delay(1);
    return Serial.read();
  }
  return c;
}
