#ifndef NAMES_H
#define NAMES_H

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

#endif