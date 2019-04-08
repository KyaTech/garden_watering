#ifndef PAYLOADS_H
#define PAYLOADS_H

#define request_symbol 'Q'
#define response_symbol 'S'
#define registration_symbol 'G'
#define command_symbol 'C'
#define MAX_CHAR_SIZE 10
#define SHORT_CHAR_SIZE 5

#include <stdio.h>
#include <string>
#include <cstring>
using namespace std;

enum SimpleResponse {
  OK,
  ERROR
};

enum ModuleType {
  SENSOR = 0,
  VALVE = 1
};

enum PayloadType {
    REQUEST,
    RESPONSE,
    COMMAND,
    REGISTRATION
};

struct radio_payload_struct {
    unsigned long request_id;
};

struct response_payload_struct : public radio_payload_struct {
    char value[MAX_CHAR_SIZE];
};


struct request_payload_struct : public radio_payload_struct{
    char attribute_requested[MAX_CHAR_SIZE];
    char additional_value[SHORT_CHAR_SIZE];
};

struct command_payload_struct : public radio_payload_struct {
    char command[MAX_CHAR_SIZE];
    char additional_value[SHORT_CHAR_SIZE];
};

struct registration_payload_struct : public radio_payload_struct {
    unsigned char module_type;
    unsigned char index;
    unsigned char pin;
};

class RadioPayload {
    private:
        uint16_t _toNode;
        virtual void _buildPayload() {};
    public:
        RadioPayload() {};
        RadioPayload(uint16_t toNode) : _toNode(toNode) {};

        uint16_t getToNode() {
            return _toNode;
        }
        void setToNode(uint16_t toNode) {
            _toNode = toNode;
        };

        unsigned long request_id;
        virtual void printPayload(bool send) {};
        void printPayload() {
            this->printPayload(false);
        };
        virtual PayloadType getType() {};
        virtual char getSymbol() {};
        virtual const void* asSendable() {};
        virtual size_t sizeOfPayload() {};
};

class RequestPayload : public RadioPayload{
    private:
        request_payload_struct _payload;
        void _buildPayload() override;
    public:
        string attribute_requested;
        string additional_value;
        void printPayload(bool send) override;
        PayloadType getType() override;
        char getSymbol() override;
        const void* asSendable() override;
        size_t sizeOfPayload() override;
};

class ResponsePayload : public RadioPayload{
    private:
        response_payload_struct _payload;
        void _buildPayload() override;
    public:
        string value;
        void printPayload(bool send) override;
        PayloadType getType() override;
        char getSymbol() override;
        const void* asSendable() override;
        size_t sizeOfPayload() override;
};

class RegistrationPayload : public RadioPayload{
    private:
        registration_payload_struct _payload;
        void _buildPayload() override;
    public:
        unsigned char module_type;
        unsigned char index;
        unsigned char pin;
        void printPayload(bool send) override;
        PayloadType getType() override;
        char getSymbol() override;
        const void* asSendable() override;
        size_t sizeOfPayload() override;
};

class CommandPayload : public RadioPayload{
    private:
        command_payload_struct _payload;
        void _buildPayload() override;
    public:
        string command;
        string additional_value;
        void printPayload(bool send) override;
        PayloadType getType() override;
        char getSymbol() override;
        const void* asSendable() override;
        size_t sizeOfPayload() override;
};

#endif