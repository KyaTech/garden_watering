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
  VALVE = 1,
};

enum AdditionalInformation {
  UNDEFINED = 0,
  INVALIDINDEX = 1,
  INVALIDREQUEST = 2,
  INVALIDCOMMAND = 3
};

struct radio_payload {
    unsigned long request_id;
};

struct request_payload : public radio_payload{
    char attribute_requested[MAX_CHAR_SIZE];
    char additional_value[SHORT_CHAR_SIZE];
};

struct response_payload : public radio_payload {
    char value[SHORT_CHAR_SIZE];
    unsigned char additional_information;
};

struct registration_payload : public radio_payload {
    unsigned char module_type;
    unsigned char index;
    unsigned char pin;
};

struct command_payload : public radio_payload {
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
    Serial.println("Registration: #" + String(reg_payload.request_id) + " module_type: VALVE at index: " + String(reg_payload.index) + " at pin: " + String(reg_payload.pin));
  } else if (reg_payload.module_type == ModuleType::SENSOR) {
    Serial.println("Registration: #" + String(reg_payload.request_id) + " module_type: SENSOR at index: " + String(reg_payload.index) + " at pin: " + String(reg_payload.pin));
  }
} 
void printCommand(command_payload& command) {
  Serial.println("Command: #" + String(command.request_id) + " command \"" + String(command.command) + "\" additional_value: \"" + command.additional_value + "\"");
}