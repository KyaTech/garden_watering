#include "func.hpp"

using namespace std;

void printHeader(RF24NetworkHeader &header) {
    printf("Received Header from \"%d\" to \"%d\" type: \"%c\" id: \"%d\"\n", header.from_node, header.to_node,
           header.type, header.id);
}

void printRequest(request_payload_struct &request, bool send) {
    printf("%sRequest: #%lu requested \"%s\" additional_value: \"%s\"\n", send ? "Send " : "", request.request_id,
           request.attribute_requested, request.additional_value);
}

void printRequest(request_payload_struct &request) {
    printRequest(request, false);
}

void printResponse(response_payload_struct &response, bool send) {
    printf("%sResponse: #%lu value \"%s\"\n", send ? "Send " : "", response.request_id, response.value);
}

void printResponse(response_payload_struct &response) {
    printResponse(response, false);
}

void printCommand(command_payload_struct &command, bool send) {
    printf("%sCommand: #%lu command \"%s\" additional_value: \"%s\"\n", send ? "Send " : "", command.request_id,
           command.command, command.additional_value);
}

void printCommand(command_payload_struct &command) {
    printCommand(command, false);
}

void printRegistration(registration_payload_struct &reg_payload, int16_t nodeID) {
    if (reg_payload.module_type == ModuleType::SENSOR) {
        printf("Registration: #%lu module_type: SENSOR from node %d at index: %d at pin: %d \n", reg_payload.request_id,
               nodeID, reg_payload.index, reg_payload.pin);
    } else {
        printf("Registration: #%lu module_type: VALVE from node %d at index: %d at pin: %d \n", reg_payload.request_id,
               nodeID, reg_payload.index, reg_payload.pin);
    }
} 
