#ifndef Radio_h
#define Radio_h

#include "RF24.h"
#include "RF24Network.h"
#include "RF24Mesh.h"
#include <SPI.h>
#include "Arduino.h"
#include <stdio.h>

class Radio {
    private:
        RF24 _rf24 = RF24(7, 8); 
        RF24Network _network = RF24Network(_rf24);
        RF24Mesh _mesh = RF24Mesh(_rf24,_network); 
        unsigned long _request_counter = 1;
        void (*_requestCallback)(request_payload, RF24NetworkHeader);
        void (*_responseCallback)(response_payload, RF24NetworkHeader);
        void (*_registrationCallback)(registration_payload, RF24NetworkHeader);
        void (*_commandCallback)(command_payload, RF24NetworkHeader);
        void (*_registrationFunction) ();
        response_payload _last_response;
        unsigned long _last_failed_request_id;
    public:
        void beginMesh(uint8_t nodeID) {
            _mesh.setNodeID(nodeID);
            if (nodeID != 0) {
                Serial.println(F("Connecting to the mesh..."));
            } else {
                Serial.println(F("Creating mesh ..."));
            }
            _mesh.begin();
        }
        void registrate(void (*registrationFunction)()) {
            this->_registrationFunction = registrationFunction;
            _registrationFunction();
        } 
        void setRequestCallback(void (*requestCallback)(request_payload, RF24NetworkHeader)) {
            this->_requestCallback = requestCallback;
        }
        void setResponseCallback(void (*responseCallback)(response_payload, RF24NetworkHeader)) {
            this->_responseCallback = responseCallback;
        }
        void setRegistrationCallback(void (*registrationCallback)(registration_payload, RF24NetworkHeader)) {
            this->_registrationCallback = registrationCallback;
        }
        void setCommandCallback(void (*commandCallback)(command_payload, RF24NetworkHeader)) {
            this->_commandCallback = commandCallback;
        }
        bool isMaster() {
            return _mesh.getNodeID() == 0;
        }
        RF24Mesh& getMesh() {
            return _mesh;
        }
        RF24Network& getNetwork() {
            return _network;
        }
        void update() {
            _mesh.update();

            if(this->isMaster()) {
                _mesh.DHCP();
            }

            this->checkConnection();
            
            while (this->packageAvailable()) {
                RF24NetworkHeader header = this->peekHeader();
                switch (header.type) {
                    case request_symbol:
                        _requestCallback(this->readRequest(),header);
                        break;
                    case response_symbol:
                        _last_response = this->readResponse();
                        _responseCallback(_last_response,header);
                        break;
                    case registration_symbol:
                        _registrationCallback(this->readRegistration(),header);
                        break;
                    case command_symbol:
                        _commandCallback(this->readCommand(),header);
                        break;
                    default: 
                        _network.read(header,0,0);
                        break;
                }
            }

        }
        bool packageAvailable() {
            return _network.available();
        }
        RF24NetworkHeader peekHeader() {
            RF24NetworkHeader header; 
            _network.peek(header);
            return header;
        }
        // function for receiving requests
        request_payload readRequest() {
            RF24NetworkHeader header;
            request_payload payload;
            _network.read(header, &payload, sizeof(payload));
            return payload;
        }
        // function for sending requests
        bool sendRequest(request_payload& payload,uint16_t node) {
            Serial.print("Send ");
            printRequest(payload);
            if (!_mesh.write(&payload,request_symbol,sizeof(payload),node)) {
                this->checkConnection();
                _last_failed_request_id = payload.request_id;
                return false;
            } else {
                return true;
            }
        }
        bool sendRequest(String attribute_requested,String additional_value,uint16_t node) {
            request_payload payload;
            payload.request_id = this->generateRequestID();
            attribute_requested.toCharArray(payload.attribute_requested,MAX_CHAR_SIZE);
            additional_value.toCharArray(payload.additional_value,MAX_CHAR_SIZE);
            return sendRequest(payload, node);
        }
        bool sendRequest(String attribute_requested,uint16_t node) {
            return sendRequest(attribute_requested,String(""),node);
        }
        // function for receiving responses
        response_payload readResponse() {
            RF24NetworkHeader header;
            response_payload payload;
            _network.read(header, &payload, sizeof(payload));
            return payload;
        }
        // function for sending responses
        bool sendResponse(response_payload& payload,uint16_t node) {
            if (!_mesh.write(&payload,response_symbol,sizeof(payload),node)) {
                this->checkConnection();
                _last_failed_request_id = payload.request_id;
                return false;
            } else {
                Serial.print("Send ");
                printResponse(payload);
                return true;
            }
        }
        bool sendResponse(String value,request_payload& req_payload,uint16_t node) {
            response_payload payload;
            payload.request_id = req_payload.request_id;
            value.toCharArray(payload.value,MAX_CHAR_SIZE);
            return sendResponse(payload,node);
        }
        bool sendResponse(String value,command_payload& com_payload,uint16_t node) {
            response_payload payload;
            payload.request_id = com_payload.request_id;
            value.toCharArray(payload.value,MAX_CHAR_SIZE);
            return sendResponse(payload,node);
        }
        bool sendResponse(String value,registration_payload& reg_payload) {
            response_payload payload;
            payload.request_id = reg_payload.request_id;
            value.toCharArray(payload.value,MAX_CHAR_SIZE);
            return sendResponse(payload,reg_payload.node_id);
        }
        bool sendSimpleResponse(SimpleResponse type,registration_payload& reg_payload) {
            switch (type) {
                case SimpleResponse::ERROR:
                    return this->sendResponse(String("ERROR"),reg_payload);
                case SimpleResponse::OK:
                    return this->sendResponse(String("OK"),reg_payload);
                default:
                    return false;
            }
        }
        bool sendSimpleResponse(SimpleResponse type,request_payload& req_payload,RF24NetworkHeader& header) {
            switch (type) {
                case SimpleResponse::ERROR:
                    return this->sendResponse(String("ERROR"),req_payload,header);
                case SimpleResponse::OK:
                    return this->sendResponse(String("OK"),req_payload,header);
                default:
                    return false;
            }
        }
        bool sendSimpleResponse(SimpleResponse type,command_payload& com_payload,RF24NetworkHeader& header) {
            switch (type) {
                case SimpleResponse::ERROR:
                    return this->sendResponse(String("ERROR"),com_payload,header);
                case SimpleResponse::OK:
                    return this->sendResponse(String("OK"),com_payload,header);
                default:
                    return false;
            }
        }
        // function for sending responses with a RF24NetworkHeader and the value given 
        bool sendResponse(String value,request_payload& req_payload,RF24NetworkHeader& header) {
            return sendResponse(value,req_payload,_mesh.getNodeID(header.from_node));
        }
        // function for sending responses with a RF24NetworkHeader and the value given 
        bool sendResponse(String value,command_payload& com_payload,RF24NetworkHeader& header) {
            return sendResponse(value,com_payload,_mesh.getNodeID(header.from_node));
        }
        // function for receiving commands
        command_payload readCommand() {
            RF24NetworkHeader header;
            command_payload payload;
            _network.read(header, &payload, sizeof(payload));
            return payload;
        }
        // function for sending commands with a struct given
        bool sendCommand(command_payload& payload,uint16_t node) {
            Serial.print("Send ");
            printCommand(payload);
            if (!_mesh.write(&payload,command_symbol,sizeof(payload),node)) {
                this->checkConnection();
                _last_failed_request_id = payload.request_id;
                return false;
            } else {
                return true;
            }
        }
        // function for sending commands without the struct given instead command and additional_value
        bool sendCommand(String command, String additional_value, uint16_t node) {
            command_payload payload;
            payload.request_id = this->generateRequestID();
            command.toCharArray(payload.command,sizeof(payload.command));
            additional_value.toCharArray(payload.additional_value,sizeof(payload.additional_value));
            return sendCommand(payload,node);
        }
        // function for sending commands without the struct and additional_value given instead command only
        bool sendCommand(String command, uint16_t node) {
            return sendCommand(command,String(""),node);
        }
        // function for receiving registrations
        registration_payload readRegistration() {
            RF24NetworkHeader header;
            registration_payload payload;
            _network.read(header, &payload, sizeof(payload));
            return payload;
        }
        // function for sending registrations
        bool sendRegistration(registration_payload& payload) {
            Serial.print("Send ");
            printRegistration(payload);
            if (!_mesh.write(&payload,registration_symbol,sizeof(payload),0)) {
                this->checkConnection();
                _last_failed_request_id = payload.request_id;
                return false;
            } else {
                return true;
            }
        }
        // function for sending registrations
        bool sendRegistration(ModuleType type,int index, int pin) {
            registration_payload payload;
            payload.request_id = this->generateRequestID();
            payload.node_id = _mesh.getNodeID();
            payload.module_type = type;
            payload.index = index;
            payload.pin = pin;
            sendRegistration(payload);
            return payload.request_id;
        }
        // function for sending registrations
        bool sendRegistration(ModuleType type) {
            registration_payload payload;
            payload.request_id = this->generateRequestID();
            payload.node_id = _mesh.getNodeID();
            payload.module_type = type;
            payload.index = -1;
            payload.pin = -1;
            sendRegistration(payload);
            return payload.request_id;
        }
        unsigned long generateRequestID() {
            unsigned long request_id = 0;
            request_id += _mesh.getNodeID();
            request_id += (_request_counter * 100);
            _request_counter++;
            return request_id;
        };
        void checkConnection() {
            if (!this->isMaster()) {
                if (!_mesh.checkConnection()) {
                    Serial.println(F("Reconnecting ..."));
                    _mesh.renewAddress();
                    this->_registrationFunction();
                }
            }
        }
        response_payload waitForAnswer(unsigned long request_id) {
            unsigned long startTime = millis();
            if (_last_failed_request_id == request_id) {
                return;
            }
            while((millis()-startTime) < 500)  {
                this->update();
                if (_last_response.request_id == request_id) {
                    break;
                }
            }
            return _last_response;
        }
        void printMesh() {
            Serial.println(" ");
            Serial.println(F("********Assigned Addresses********"));
            Serial.print("NodeID: ");
            Serial.print(_mesh.getNodeID());
            Serial.println(" RF24Network Address: 00");

            for(int i=0; i < _mesh.addrListTop; i++){
                Serial.print("NodeID: ");
                Serial.print(_mesh.addrList[i].nodeID);
                Serial.print(" RF24Network Address: 0");
                Serial.println(_mesh.addrList[i].address,OCT);
            }
            Serial.println(F("**********************************"));
            Serial.println(" ");
        }

};

#endif