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
        unsigned long _last_failed_request_id = 0;
    public:
        // starts the mesh network
        void beginMesh(uint8_t nodeID) {
            _mesh.setNodeID(nodeID);
            if (nodeID != 0) {
                Serial.println(F("Connecting to the mesh..."));
            } else {
                Serial.println(F("Creating mesh ..."));
            }
            _mesh.begin();
            /*_rf24.setPALevel(RF24_PA_MAX); 
            _rf24.setDataRate(RF24_1MBPS);
            _rf24.setCRCLength(RF24_CRC_16);*/
        }

        // accepts the callback and runs it
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

        // return if this node is a master node by checking the node_id
        bool isMaster() {
            return this->_mesh.getNodeID() == 0;
        }

        // function which updates the network, if available reads from the network and calls the callbacks
        // also run the DHCP server if this is the master node
        void update() {
            this->_mesh.update();

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

        // return if a package is available for this node
        bool packageAvailable() {
            return _network.available();
        }

        // function for only peeking the header and not reading it.
        // usefull for type requests before reading the payload into the datatype
        RF24NetworkHeader peekHeader() {
            RF24NetworkHeader header; 
            _network.peek(header);
            return header;
        }
        
        /**** BASIC READING FUNCTIONS *****/

        // function for receiving requests
        request_payload readRequest() {
            RF24NetworkHeader header;
            request_payload payload;
            _network.read(header, &payload, sizeof(payload));
            return payload;
        }
        // function for receiving responses
        response_payload readResponse() {
            RF24NetworkHeader header;
            response_payload payload;
            _network.read(header, &payload, sizeof(payload));
            return payload;
        }
        // function for receiving commands
        command_payload readCommand() {
            RF24NetworkHeader header;
            command_payload payload;
            _network.read(header, &payload, sizeof(payload));
            return payload;
        }
        // function for receiving registrations
        registration_payload readRegistration() {
            RF24NetworkHeader header;
            registration_payload payload;
            _network.read(header, &payload, sizeof(payload));
            return payload;
        }

        /**** END ****/

        /**** BASIC SENDING FUNCTIONS *****/

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
        // function for sending responses
        bool sendResponse(response_payload& payload,uint16_t node) {
            Serial.print("Send ");
            printResponse(payload);
            if (!_mesh.write(&payload,response_symbol,sizeof(payload),node)) {
                this->checkConnection();
                _last_failed_request_id = payload.request_id;
                return false;
            } else {
                return true;
            }
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

        /**** END ****/

        /*** additional functions for requests ****/
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
        /**** END ****/

        /**** additional functions for responses ****/
        bool sendResponse(String value,radio_payload& r_payload,uint16_t node) {
            response_payload payload;
            payload.request_id = r_payload.request_id;
            value.toCharArray(payload.value,MAX_CHAR_SIZE);
            return sendResponse(payload,node);
        }
        // function for sending responses with a RF24NetworkHeader and the value given 
        bool sendResponse(String value,radio_payload& payload,RF24NetworkHeader& header) {
            return sendResponse(value,payload,_mesh.getNodeID(header.from_node));
        }
        // function for sending standardized responses 
        bool sendSimpleResponse(SimpleResponse type,radio_payload& payload, RF24NetworkHeader& header) {
            switch (type) {
                case SimpleResponse::ERROR:
                    return this->sendResponse(String("ERROR"),payload,header);
                case SimpleResponse::OK:
                    return this->sendResponse(String("OK"),payload,header);
                default:
                    return false;
            }
        }

        /**** END ****/

        /**** additional functions for commands ****/

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

        /**** END ****/

        /**** additional functions for registrations ****/

        // function for sending registrations
        bool sendRegistration(ModuleType type,int index, int pin) {
            registration_payload payload;
            payload.request_id = this->generateRequestID();
            payload.module_type = type;
            payload.index = index;
            payload.pin = pin;
            sendRegistration(payload);
            return payload.request_id;
        }
        // function for sending registrations
        bool sendRegistration(ModuleType type) {
            return sendRegistration(type,-1,-1);
        }

        /**** END ****/

        // function for genrerating request_ids based on request_counter and node_id
        unsigned long generateRequestID() {
            unsigned long request_id = 0;
            request_id += _mesh.getNodeID();
            request_id += (_request_counter * 100);
            _request_counter++;
            return request_id;
        };

        // function for checking connection and reconnecting as well as reregistration at master if necessary
        void checkConnection() {
            if (!this->isMaster()) {
                if (!_mesh.checkConnection()) {
                    Serial.println(F("Reconnecting ..."));
                    _mesh.renewAddress();
                    this->_registrationFunction();
                }
            }
        }

        // function which takes a request_id and waits for a response with this id
        response_payload waitForAnswer(unsigned long request_id) {
            unsigned long startTime = millis();
            if (_last_failed_request_id == request_id) {
                response_payload nullPayload;
                return nullPayload;
            }
            while((millis()-startTime) < 500)  {
                this->update();
                if (_last_response.request_id == request_id) {
                    break;
                }
            }
            return _last_response;
        }

        // function which prints out all nodes connected to the network
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

        int16_t getNodeID(uint32_t adress) {
            return _mesh.getNodeID(adress);
        }
};

#endif