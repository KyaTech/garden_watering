#ifndef Radio_h
#define Radio_h

#include "RF24.h"
#include "RF24Network.h"
#include "RF24Mesh.h"
#include <SPI.h>
#include "Arduino.h"

class Radio {
    private:
        RF24 _rf24 = RF24(7, 8); 
        RF24Network _network = RF24Network(_rf24);
        RF24Mesh _mesh = RF24Mesh(_rf24,_network); 
    public:
        void beginMesh(uint8_t nodeID) {
            _mesh.setNodeID(nodeID);
            Serial.println(F("Connecting to the mesh..."));
            _mesh.begin();
        }
        RF24Mesh& getMesh() {
            return _mesh;
        }
        RF24Network& getNetwork() {
            return _network;
        }
        void update() {
            _mesh.update();
        }
        bool packageAvailable() {
            return _network.available();
        }
        RF24NetworkHeader peekHeader() {
            RF24NetworkHeader header; 
            _network.peek(header);
            return header;
        }
        // Testfunction for receiving payload_t-payloads (see names.h)
        payload_t readMillisPayload() {
            RF24NetworkHeader header;
            payload_t payload;
            _network.read(header, &payload, sizeof(payload));
            return payload;
        }
        // Testfunction for sending payload_t-payloads (see names.h)
        bool sendMillisPayload(payload_t& payload,uint16_t node) {
            if (!_mesh.write(&payload,'P',sizeof(payload),node)) {
                if (!_mesh.checkConnection()) {
                    _mesh.renewAddress();
                }
                return false;
            } else {
                return true;
            }
        }
        void DHCP() {
            _mesh.DHCP();
        }
};

#endif