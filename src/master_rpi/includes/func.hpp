#ifndef FUNC_H
#define FUNC_H

#include <RF24/RF24.h>
#include <RF24Network/RF24Network.h>
#include <RF24Mesh/RF24Mesh.h>
#include <string>
#include "names.hpp"

void printHeader(RF24NetworkHeader& header);
void printRequest(request_payload& request, bool send);
void printRequest(request_payload& request);
void printResponse(response_payload& response, bool send);
void printResponse(response_payload& response);
void printCommand(command_payload& command, bool send);
void printCommand(command_payload& command);
void printRegistration(registration_payload& reg_payload, int16_t nodeID);

#endif