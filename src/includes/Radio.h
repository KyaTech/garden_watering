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
        void (*_requestCallback)(request_payload);
        void (*_responseCallback)(response_payload);
    public:
        void beginMesh(uint8_t nodeID) {
            _mesh.setNodeID(nodeID);
            Serial.println(F("Connecting to the mesh..."));
            _mesh.begin();
        }
        void setRequestCallback(void (*requestCallback)(request_payload)) {
            this->_requestCallback = requestCallback;
        }
        void setResponseCallback(void (*responseCallback)(response_payload)) {
            this->_responseCallback = responseCallback;
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
                        _requestCallback(this->readRequest());
                        break;
                    case response_symbol:
                        _responseCallback(this->readResponse());
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
            if (!_mesh.write(&payload,request_symbol,sizeof(payload),node)) {
                this->checkConnection();
                return false;
            } else {
                return true;
            }
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
                return false;
            } else {
                return true;
            }
        }
        void generateRequestID(char request_id[7]) {
            sprintf(request_id,"%04d%02d",_request_counter,_mesh.getNodeID());
            _request_counter++;
        };
        void checkConnection() {
            if (!this->isMaster()) {
                if (!_mesh.checkConnection()) {
                    Serial.println(F("Reconnecting"));
                    _mesh.renewAddress();
                }
            }
        }

};

#endif