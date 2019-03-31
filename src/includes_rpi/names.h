#include <string>
using namespace std;

#define request_symbol 'Q'
#define response_symbol 'S'
#define registration_symbol 'G'
#define command_symbol 'C'
#define MAX_CHAR_SIZE 10
#define SHORT_CHAR_SIZE 5

enum SimpleResponse {
  OK,
  ERROR
};

enum ModuleType {
  SENSOR = 0,
  VALVE = 1
};

struct radio_payload {
    unsigned long request_id;
};

struct request_payload : radio_payload{
    char attribute_requested[MAX_CHAR_SIZE];
    char additional_value[SHORT_CHAR_SIZE];
};

struct response_payload : radio_payload {
    char value[MAX_CHAR_SIZE];
};

struct registration_payload : radio_payload {
    unsigned char module_type;
    unsigned char index;
    unsigned char pin;
};

struct command_payload : radio_payload {
    char command[MAX_CHAR_SIZE];
    char additional_value[SHORT_CHAR_SIZE];
};

void printHeader(RF24NetworkHeader& header) {
    printf("Received Header from \"%d\" to \"%d\" type: \"%c\" id: \"%d\"\n",header.from_node,header.to_node,header.type,header.id);
}

void printRequest(request_payload& request, bool send) {
    printf("%sRequest: #%lu requested \"%s\" additional_value: \"%s\"\n",send?"Send ":"",request.request_id,request.attribute_requested,request.additional_value);
}

void printRequest(request_payload& request) {
    printRequest(request,false);
}

void printResponse(response_payload& response, bool send) {
    printf("%sResponse: #%lu value \"%s\"\n",send?"Send ":"",response.request_id,response.value);
}

void printResponse(response_payload& response) {
    printResponse(response,false);
}

void printCommand(command_payload& command, bool send) {
    printf("%sCommand: #%lu command \"%s\" additional_value: \"%s\"\n",send?"Send ":"",command.request_id,command.command,command.additional_value);
} 

void printCommand(command_payload& command) {
    printCommand(command,false);
}

void printRegistration(registration_payload& reg_payload, int16_t nodeID) {
    if (reg_payload.module_type == ModuleType::SENSOR) {
        printf("Registration: #%lu module_type: SENSOR from node %d at index: %d at pin: %d \n",reg_payload.request_id,nodeID,reg_payload.index,reg_payload.pin);
    } else {
        printf("Registration: #%lu module_type: VALVE from node %d at index: %d at pin: %d \n",reg_payload.request_id,nodeID,reg_payload.index,reg_payload.pin);
    }
} 