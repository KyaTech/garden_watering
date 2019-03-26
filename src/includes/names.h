#define request_symbol 'Q'
#define response_symbol 'S'
#define registration_symbol 'G'
#define command_symbol 'C'
#define MAX_CHAR_SIZE 10
#define SHORT_CHAR_SIZE 5

struct radio_payload {};

struct request_payload : radio_payload{
    unsigned long request_id;
    char attribute_requested[MAX_CHAR_SIZE];
    char additional_value[SHORT_CHAR_SIZE];
};

struct response_payload : radio_payload {
    unsigned long request_id;
    char value[MAX_CHAR_SIZE];
};

struct registration_payload : radio_payload {
  	unsigned long request_id;
    char module_type[MAX_CHAR_SIZE];
    unsigned short node_id;
};

struct command_payload : radio_payload {
    unsigned long request_id;
    char command[MAX_CHAR_SIZE];
    char additional_value[SHORT_CHAR_SIZE];
};

enum SimpleResponse {
  OK,
  ERROR
};

void printHeader(RF24NetworkHeader& header) {
  Serial.println("Received Header from \"" + String(header.from_node) + "\" to \"" + String(header.to_node) +  "\" type: \"" + String(header.type) + "\" id: \"" + String(header.id) + "\"");
}

void printRequest(request_payload& request) {
  Serial.println("Request: #" + String(request.request_id) + " requested \"" + String(request.attribute_requested) + "\" additional_value: \"" + request.additional_value +"\"");
}
void printResponse(response_payload& response) {
  Serial.println("Response: #" + String(response.request_id) + " value \"" + String(response.value) + "\"");
}
void printRegistration(registration_payload& reg_payload) {
  Serial.println("Registration: #" + String(reg_payload.request_id) + " module_type: \"" + String(reg_payload.module_type) + "\" from node " + String(reg_payload.node_id));
} 
void printCommand(command_payload& command) {
  Serial.println("Command: #" + String(command.request_id) + " command \"" + String(command.command) + "\" additional_value: \"" + command.additional_value + "\"");
}