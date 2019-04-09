#include "payloads.hpp"
using namespace std;

//RequestPayload
void RequestPayload::printPayload(bool send) {
    this->_buildPayload();
    printf("%sRequest: #%lu requested \"%s\" additional_value: \"%s\"\n",send?"Send ":"",this->request_id,this->attribute_requested.c_str(),this->additional_value.c_str());
}
PayloadType RequestPayload::getType() {
    return PayloadType::REQUEST;
}
char RequestPayload::getSymbol() {
    return request_symbol;
}
const void* RequestPayload::asSendable() {
    this->_buildPayload();
    return &(this->_payload);
}

size_t RequestPayload::sizeOfPayload() {
    this->_buildPayload();
    return sizeof(this->_payload);
}

void RequestPayload::_buildPayload() {
    _payload.request_id = request_id;
    strcpy(_payload.attribute_requested,attribute_requested.c_str());
    strcpy(_payload.additional_value,additional_value.c_str());
}

//ResponsePayload
void ResponsePayload::printPayload(bool send) {
    this->_buildPayload();
    printf("%sResponse: #%lu value \"%s\"\n",send?"Send ":"",this->request_id,this->value.c_str());
}
PayloadType ResponsePayload::getType() {
    return PayloadType::RESPONSE;
}
char ResponsePayload::getSymbol() {
    return response_symbol;
}
const void* ResponsePayload::asSendable() {
    this->_buildPayload();
    return &(this->_payload);
}
size_t ResponsePayload::sizeOfPayload() {
    this->_buildPayload();
    return sizeof(this->_payload);
}
void ResponsePayload::_buildPayload() {
    _payload.request_id = request_id;
    strcpy(_payload.value,value.c_str());
}

//RegistrationPayload
void RegistrationPayload::printPayload(bool send) {
    this->_buildPayload();
    if (this->module_type == ModuleType::SENSOR) {
        printf("Registration: #%lu module_type: SENSOR at index: %d at pin: %d \n",this->request_id,this->index,this->pin);
    } else {
        printf("Registration: #%lu module_type: VALVE at index: %d at pin: %d \n",this->request_id,this->index,this->pin);
    }
}
PayloadType RegistrationPayload::getType() {
    return PayloadType::REGISTRATION;
}
char RegistrationPayload::getSymbol() {
    return registration_symbol;
}
const void* RegistrationPayload::asSendable() {
    this->_buildPayload();
    return &(this->_payload);
}
size_t RegistrationPayload::sizeOfPayload() {
    this->_buildPayload();
    return sizeof(this->_payload);
}
void RegistrationPayload::_buildPayload() {
    _payload.request_id = request_id;
    _payload.module_type = module_type;
    _payload.index = index;
    _payload.pin = pin;
}

//CommandPayload
void CommandPayload::printPayload(bool send) {
    this->_buildPayload();
    printf("%sCommand: #%lu command \"%s\" additional_value: \"%s\"\n",send?"Send ":"",this->request_id,this->command.c_str(),this->additional_value.c_str());
}
PayloadType CommandPayload::getType() {
    return PayloadType::COMMAND;
}
char CommandPayload::getSymbol() {
    return command_symbol;
}
const void* CommandPayload::asSendable() {
    this->_buildPayload();
    return &(this->_payload);
}
size_t CommandPayload::sizeOfPayload() {
    this->_buildPayload();
    return sizeof(this->_payload);
}

void CommandPayload::_buildPayload() {
    _payload.request_id = request_id;
    strcpy(_payload.command,command.c_str());
    strcpy(_payload.additional_value,additional_value.c_str());
}





