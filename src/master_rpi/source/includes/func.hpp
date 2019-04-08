#ifndef FUNC_H
#define FUNC_H

#include <RF24/RF24.h>
#include <RF24Network/RF24Network.h>
#include <RF24Mesh/RF24Mesh.h>
#include <string>
#include "payloads.hpp"

void printHeader(RF24NetworkHeader& header);
void printRequest(request_payload_struct& request, bool send);
void printRequest(request_payload_struct& request);
void printResponse(response_payload_struct& response, bool send);
void printResponse(response_payload_struct& response);
void printCommand(command_payload_struct& command, bool send);
void printCommand(command_payload_struct& command);
void printRegistration(registration_payload_struct& reg_payload, int16_t nodeID);

#endif