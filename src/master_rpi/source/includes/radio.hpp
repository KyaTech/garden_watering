#ifndef RADIO_H
#define RADIO_H 1

#include <RF24/RF24.h>
#include <RF24Network/RF24Network.h>
#include <RF24Mesh/RF24Mesh.h>

#include <runtime_utils.hpp>
#include <string>
#include <pplx/pplxtasks.h>
#include <pplx/pplxcancellation_token.h>
#include <pplx/pplx.h>

using namespace std;
using namespace concurrency;
using namespace pplx;

#include "payloads.hpp"
#include "func.hpp"

class Radio {
    private:
        RF24 _rf24 = RF24(RPI_V2_GPIO_P1_15, BCM2835_SPI_CS0,BCM2835_SPI_SPEED_8MHZ);
        RF24Network _network = RF24Network(_rf24);
        RF24Mesh _mesh = RF24Mesh(_rf24,_network);
        unsigned long _request_counter = 1;
        void (*_requestCallback)(request_payload_struct, RF24NetworkHeader);
        void (*_responseCallback)(response_payload_struct, RF24NetworkHeader);
        void (*_registrationCallback)(registration_payload_struct, RF24NetworkHeader);
        void (*_commandCallback)(command_payload_struct, RF24NetworkHeader);
        void (*_registrationFunction) ();
        response_payload_struct _last_response;
        unsigned long _last_failed_request_id = 0;
        pplx::task<void> _background;
        pplx::cancellation_token_source _cts;
        bool _taskIsRunning = false;
    public:
        // starts the mesh network
        void beginMesh(uint8_t nodeID);

        // accepts the callback and runs it
        void registrate(void (*registrationFunction)()); 
        void setRequestCallback(void (*requestCallback)(request_payload_struct, RF24NetworkHeader));
        void setResponseCallback(void (*responseCallback)(response_payload_struct, RF24NetworkHeader));
        void setRegistrationCallback(void (*registrationCallback)(registration_payload_struct, RF24NetworkHeader));
        void setCommandCallback(void (*commandCallback)(command_payload_struct, RF24NetworkHeader));

        // return if this node is a master node by checking the node_id
        bool isMaster();

        // function which updates the network, if available reads from the network and calls the callbacks
        // also run the DHCP server if this is the master node
        void update();

        // return if a package is available for this node
        bool packageAvailable();

        // function for only peeking the header and not reading it.
        // usefull for type requests before reading the payload into the datatype
        RF24NetworkHeader peekHeader();

        // function for receiving requests
        request_payload_struct readRequest();
        // function for receiving responses
        response_payload_struct readResponse();
        // function for receiving commands
        command_payload_struct readCommand();
        // function for receiving registrations
        registration_payload_struct readRegistration();


        unsigned long sendRadioPayload(RadioPayload& payload);
        // function for sending requests
        bool sendRequest(request_payload_struct& payload,uint16_t node);
        // function for sending responses
        bool sendResponse(response_payload_struct& payload,uint16_t node);
        // function for sending registrations
        bool sendRegistration(registration_payload_struct& payload);
        // function for sending commands with a struct given
        bool sendCommand(command_payload_struct& payload,uint16_t node);

        //
        unsigned long sendRequest(string attribute_requested,string additional_value,uint16_t node);
        //
        unsigned long sendRequest(string attribute_requested,uint16_t node);

        //
        unsigned long sendResponse(string value,radio_payload_struct& r_payload,uint16_t node);
        // function for sending responses with a RF24NetworkHeader and the value given 
        unsigned long sendResponse(string value,radio_payload_struct& payload,RF24NetworkHeader& header);
        // function for sending standardized responses 
        unsigned long sendSimpleResponse(SimpleResponse type,radio_payload_struct& payload, RF24NetworkHeader& header);

        // function for sending commands without the struct given instead command and additional_value
        unsigned long sendCommand(string command, string additional_value, uint16_t node);
        // function for sending commands without the struct and additional_value given instead command only
        unsigned long sendCommand(string command, uint16_t node);

        // function for sending registrations
        unsigned long sendRegistration(ModuleType type,int index, int pin);
        // function for sending registrations
        unsigned long sendRegistration(ModuleType type);

        // function for genrerating request_ids based on request_counter and node_id
        unsigned long generateRequestID();

        // function for checking connection and reconnecting as well as reregistration at master if necessary
        void checkConnection();

        // function which takes a request_id and waits for a response with this id
        response_payload_struct waitForAnswer(unsigned long request_id);

        // function which prints out all nodes connected to the network
        void printMesh();

        int16_t getNodeID(uint32_t adress);

        void updateAndLog();

        void stopTask();

        
};

#endif