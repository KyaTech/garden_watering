#define request_symbol 'Q'
#define response_symbol 'S'
#define register_symbol 'G'
#define MAX_CHAR_SIZE 10

struct request_payload {
    char request_id[7];
    char attribute_requested[MAX_CHAR_SIZE];
};

struct response_payload {
    char request_id[7];
    char value[MAX_CHAR_SIZE];
};

struct register_payload {
  	char request_id[7];
    char sensor_type[MAX_CHAR_SIZE];
    unsigned short node_id;
};

void printRequest(request_payload& request) {
  Serial.println("Request: #" + String(request.request_id) + " requested \"" + String(request.attribute_requested) + "\"");
}
void printResponse(response_payload& response) {
  Serial.println("Response: #" + String(response.request_id) + " value \"" + String(response.value) + "\"");
}