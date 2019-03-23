#define request_symbol 'Q'
#define response_symbol 'S'
#define MAX_CHAR_SIZE 100

struct request_payload {
    unsigned long request_id;
    char attribute_requested[MAX_CHAR_SIZE];
};

struct response_payload {
    unsigned long request_id;
    char value[MAX_CHAR_SIZE];
};

struct payload_t {
  unsigned long ms;
  unsigned long counter;
};

void printRequest(request_payload& request) {
  Serial.println("Request: #" + String(request.request_id) + " requested \"" + String(request.attribute_requested) + "\"");
}
void printResponse(response_payload& response) {
  Serial.println("Response: #" + String(response.request_id) + " value \"" + String(response.value) + "\"");
}