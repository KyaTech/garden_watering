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
  SENSOR,
  VALVE
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
    unsigned short node_id;
    ModuleType module_type;
    int index;
    int pin;
};

struct command_payload : radio_payload {
    char command[MAX_CHAR_SIZE];
    char additional_value[SHORT_CHAR_SIZE];
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
  if (reg_payload.module_type == ModuleType::VALVE) {
    Serial.println("Registration: #" + String(reg_payload.request_id) + " module_type: VALVE from node " + String(reg_payload.node_id) + " at index: " + String(reg_payload.index) + " at pin: " + String(reg_payload.pin));
  } else if (reg_payload.module_type == ModuleType::SENSOR) {
    Serial.println("Registration: #" + String(reg_payload.request_id) + " module_type: SENSOR from node " + String(reg_payload.node_id) + " at index: " + String(reg_payload.index) + " at pin: " + String(reg_payload.pin));
  }
} 
void printCommand(command_payload& command) {
  Serial.println("Command: #" + String(command.request_id) + " command \"" + String(command.command) + "\" additional_value: \"" + command.additional_value + "\"");
}