
/**
*/

#include <RF24.h>
#include <RF24Network.h>
#include <RF24Mesh.h>
#include <SPI.h>

#include "includes\settings.h"
#include "includes\names.h"
#include "includes\Radio.h"
//#include <printf.h>

/**
   User Configuration: nodeID - A unique identifier for each radio. Allows addressing
   to change dynamically with physical changes to the mesh.

   In this example, configuration takes place below, prior to uploading the sketch to the device
   A unique value from 1-255 must be configured for each node.
   This will be stored in EEPROM on AVR devices, so remains persistent between further uploads, loss of power, etc.

 **/

#define nodeID 1
#define Moisture
//#define Valves

Radio radio;

uint32_t displayTimer = 0;
unsigned long counter = 0;

#ifdef Moisture
// no reading pin required, it automaticly used D5 for reading, because this is the best pin for frequencies
#include <FreqCounter.h>
#include <RunningMedian.h>
int moisturePowerPin = 4;
void (*resetFunc)(void) = 0;
bool reset = false;
#endif

#ifdef Valves
int valvePins[] = {2, 3};
int valvePinsLength = (sizeof(valvePins) / sizeof(int));
#endif

void setup()
{
  Serial.begin(115200);
  radio.beginMesh(nodeID);
  radio.setRequestCallback(requestCallback);
  radio.setResponseCallback(responseCallback);
  radio.setCommandCallback(commandCallback);
  radio.registrate(registrationProcess);

#ifdef Valves
  for (int i = 0; i < valvePinsLength; i++)
  {
    pinMode(valvePins[i], OUTPUT);
  }
#endif

#ifdef Moisture
  pinMode(moisturePowerPin, OUTPUT);
#endif
}

void registrationProcess()
{
#ifdef Moisture
  radio.waitForAnswer(radio.sendRegistration(ModuleType::SENSOR));
#endif
#ifdef Valves
  for (int i = 0; i < valvePinsLength; i++)
  {
    radio.waitForAnswer(radio.sendRegistration(ModuleType::VALVE, i, valvePins[i]));
  }
#endif
}

void commandCallback(command_payload payload, RF24NetworkHeader header)
{
  printCommand(payload);

// area for accepting valve controls
#ifdef Valves

  String command = String(payload.command);
  if (command == "ON")
  {
    int index = String(payload.additional_value).toInt();

    if (index > (valvePinsLength - 1))
    {
      radio.sendSimpleResponse(SimpleResponse::ERROR, AdditionalInformation::INVALIDINDEX, payload, header);
    }
    else
    {
      digitalWrite(valvePins[index], HIGH);
      radio.sendSimpleResponse(SimpleResponse::OK, payload, header);
    }
  }
  else if (command == "OFF")
  {
    int index = String(payload.additional_value).toInt();

    if (index > (valvePinsLength - 1))
    {
      radio.sendSimpleResponse(SimpleResponse::ERROR, AdditionalInformation::INVALIDINDEX, payload, header);
    }
    else
    {
      digitalWrite(valvePins[index], LOW);
      radio.sendSimpleResponse(SimpleResponse::OK, payload, header);
    }
  }
  else
  {
    radio.sendSimpleResponse(SimpleResponse::ERROR, AdditionalInformation::INVALIDCOMMAND, payload, header);
  }
#endif
}

void requestCallback(request_payload payload, RF24NetworkHeader header)
{
  printHeader(header);
  printRequest(payload);

  String attribute_requested = String(payload.attribute_requested);

  if (attribute_requested == "Battery")
  {
    radio.sendResponse(String(random(0, 100)), payload, header);
  }
// area for accepting mositure-requests
#ifdef Moisture
  else if (attribute_requested == "Moisture")
  {
    radio.sendResponse(String(requestMoisture()), payload, header);
  }
#endif
#ifdef Valves
  else if (attribute_requested == "State")
  {
    if (String(payload.additional_value).length() != 0)
    {
      int index = String(payload.additional_value).toInt();
      if (index > (valvePinsLength - 1))
      {
        radio.sendSimpleResponse(SimpleResponse::ERROR, AdditionalInformation::INVALIDINDEX, payload, header);
      }
      else
      {
        if (digitalRead(valvePins[index]) == LOW)
        {
          radio.sendResponse("OFF", payload, header);
        }
        else if (digitalRead(valvePins[index]) == HIGH)
        {
          radio.sendResponse("ON", payload, header);
        }
      }
    }
  }
#endif
  else
  {
    radio.sendSimpleResponse(SimpleResponse::ERROR, AdditionalInformation::INVALIDREQUEST, payload, header);
  }
}

void responseCallback(response_payload payload, RF24NetworkHeader header)
{
  printResponse(payload);
}

void loop()
{
  if (reset)
  {
    Serial.println("[loop] resetting");
    Serial.flush();
    resetFunc();
    reset = false;
  }
  else
  {
    radio.update();
  }
}

#ifdef Moisture
#define DEVICE_ADRESS 2
#define MEASURE 11
#define READ 12

float requestMoisture()
{
  Serial.flush();
  float value = readMoisture();

  reset = true;
  return value;
}

float readMoisture()
{

  digitalWrite(sensorPowerPin, HIGH);
  delay(50);

  RunningMedian data = RunningMedian(measurementIntervalls);
  for (int i = 0; i < measurementIntervalls; i++)
  {
    data.add(readMoistureOnce());
    delay(1);
  }
  digitalWrite(sensorPowerPin, LOW);

  float med = data.getMedian();
  Serial.println("[Measurement] Med " + String(med) + "kHz");

  return med;
}

float readMoistureOnce()
{
  FreqCounter::f_comp = 1;
  FreqCounter::start(measurementTime);
  while (FreqCounter::f_ready == 0)
    ;
  unsigned long frq = FreqCounter::f_freq * 10;
  float frq_kHz = frq / ((float)1000);

  Serial.println("[Measurement] " + String(frq_kHz) + "kHz");
  return frq_kHz;
}
#endif