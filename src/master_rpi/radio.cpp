#include "radio.hpp"

// starts the mesh network
void Radio::beginMesh(uint8_t nodeID)
{
    _mesh.setNodeID(nodeID);
    if (nodeID != 0)
    {
        printf("Connecting to the mesh...\n");
    }
    else
    {
        printf("Creating mesh ...\n");
    }
    _mesh.begin();
    _rf24.setPALevel(RF24_PA_MAX);
    _rf24.setDataRate(RF24_1MBPS);
    _rf24.setCRCLength(RF24_CRC_16);

    _cts = cancellation_token_source();
    auto token = _cts.get_token();

    _background = create_task([token] {
        cancel_current_task();
    },
                              token);
}

// accepts the callback and runs it
void Radio::registrate(void (*registrationFunction)())
{
    this->_registrationFunction = registrationFunction;
    _registrationFunction();
}
void Radio::setRequestCallback(void (*requestCallback)(request_payload, RF24NetworkHeader))
{
    this->_requestCallback = requestCallback;
}
void Radio::setResponseCallback(void (*responseCallback)(response_payload, RF24NetworkHeader))
{
    this->_responseCallback = responseCallback;
}
void Radio::setRegistrationCallback(void (*registrationCallback)(registration_payload, RF24NetworkHeader))
{
    this->_registrationCallback = registrationCallback;
}
void Radio::setCommandCallback(void (*commandCallback)(command_payload, RF24NetworkHeader))
{
    this->_commandCallback = commandCallback;
}

// return if this node is a master node by checking the node_id
bool Radio::isMaster()
{
    return this->_mesh.getNodeID() == 0;
}

// function which updates the network, if available reads from the network and calls the callbacks
// also run the DHCP server if this is the master node
void Radio::update()
{
    this->_mesh.update();

    if (this->isMaster())
    {
        _mesh.DHCP();
    }

    this->checkConnection();

    while (this->packageAvailable())
    {
        RF24NetworkHeader header = this->peekHeader();
        switch (header.type)
        {
        case request_symbol:
            _requestCallback(this->readRequest(), header);
            break;
        case response_symbol:
            _last_response = response_payload();
            _last_response = this->readResponse();
            _responseCallback(_last_response, header);
            break;
        case registration_symbol:
            _registrationCallback(this->readRegistration(), header);
            break;
        case command_symbol:
            _commandCallback(this->readCommand(), header);
            break;
        default:
            _network.read(header, 0, 0);
            break;
        }
    }
}

// return if a package is available for this node
bool Radio::packageAvailable()
{
    return _network.available();
}

// function for only peeking the header and not reading it.
// usefull for type requests before reading the payload into the datatype
RF24NetworkHeader Radio::peekHeader()
{
    RF24NetworkHeader header;
    _network.peek(header);
    return header;
}

/**** BASIC READING FUNCTIONS *****/

// function for receiving requests
request_payload Radio::readRequest()
{
    RF24NetworkHeader header;
    request_payload payload;
    _network.read(header, &payload, sizeof(payload));
    return payload;
}
// function for receiving responses
response_payload Radio::readResponse()
{
    RF24NetworkHeader header;
    response_payload payload;
    _network.read(header, &payload, sizeof(payload));
    return payload;
}
// function for receiving commands
command_payload Radio::readCommand()
{
    RF24NetworkHeader header;
    command_payload payload;
    _network.read(header, &payload, sizeof(payload));
    return payload;
}
// function for receiving registrations
registration_payload Radio::readRegistration()
{
    RF24NetworkHeader header;
    registration_payload payload;
    _network.read(header, &payload, sizeof(payload));
    return payload;
}

/**** END ****/

/**** BASIC SENDING FUNCTIONS *****/

// function for sending requests
bool Radio::sendRequest(request_payload &payload, uint16_t node)
{
    this->stopTask();
    printRequest(payload, true);
    if (!_mesh.write(&payload, request_symbol, sizeof(payload), node))
    {
        this->checkConnection();
        _last_failed_request_id = payload.request_id;
        this->updateAndLog();
        return false;
    }
    else
    {
        this->updateAndLog();
        return true;
    }
}
// function for sending responses
bool Radio::sendResponse(response_payload &payload, uint16_t node)
{
    printResponse(payload, true);
    if (!_mesh.write(&payload, response_symbol, sizeof(payload), node))
    {
        this->checkConnection();
        _last_failed_request_id = payload.request_id;
        return false;
    }
    else
    {
        return true;
    }
}
// function for sending registrations
bool Radio::sendRegistration(registration_payload &payload)
{
    this->stopTask();
    printRegistration(payload, true);
    if (!_mesh.write(&payload, registration_symbol, sizeof(payload), 0))
    {
        this->checkConnection();
        _last_failed_request_id = payload.request_id;
        this->updateAndLog();
        return false;
    }
    else
    {
        this->updateAndLog();
        return true;
    }
    
}
// function for sending commands with a struct given
bool Radio::sendCommand(command_payload &payload, uint16_t node)
{
    this->stopTask();
    printCommand(payload, true);
    if (!_mesh.write(&payload, command_symbol, sizeof(payload), node))
    {
        this->checkConnection();
        _last_failed_request_id = payload.request_id;
        this->updateAndLog();
        return false;
    }
    else
    {
        this->updateAndLog();
        return true;
    }
}

/**** END ****/

/*** additional functions for requests ****/
unsigned long Radio::sendRequest(string attribute_requested, string additional_value, uint16_t node)
{
    request_payload payload;
    payload.request_id = this->generateRequestID();
    strcpy(payload.attribute_requested, attribute_requested.c_str());
    strcpy(payload.additional_value, additional_value.c_str());
    sendRequest(payload, node);
    return payload.request_id;
}
unsigned long Radio::sendRequest(string attribute_requested, uint16_t node)
{
    return sendRequest(attribute_requested, "", node);
}
/**** END ****/

/**** additional functions for responses ****/
unsigned long Radio::sendResponse(string value, radio_payload &r_payload, uint16_t node)
{
    response_payload payload;
    payload.request_id = r_payload.request_id;
    strcpy(payload.value, value.c_str());
    sendResponse(payload, node);
    return payload.request_id;
}
// function for sending responses with a RF24NetworkHeader and the value given
unsigned long Radio::sendResponse(string value, radio_payload &payload, RF24NetworkHeader &header)
{
    return sendResponse(value, payload, _mesh.getNodeID(header.from_node));
}
// function for sending standardized responses
unsigned long Radio::sendSimpleResponse(SimpleResponse type, radio_payload &payload, RF24NetworkHeader &header)
{
    switch (type)
    {
    case SimpleResponse::ERROR:
        return this->sendResponse("ERROR", payload, header);
    case SimpleResponse::OK:
        return this->sendResponse("OK", payload, header);
    default:
        return 0;
    }
}

/**** END ****/

/**** additional functions for commands ****/

// function for sending commands without the struct given instead command and additional_value
unsigned long Radio::sendCommand(string command, string additional_value, uint16_t node)
{
    command_payload payload;
    payload.request_id = this->generateRequestID();
    strcpy(payload.command, command.c_str());
    strcpy(payload.additional_value, additional_value.c_str());
    sendCommand(payload, node);
    return payload.request_id;
}
// function for sending commands without the struct and additional_value given instead command only
unsigned long Radio::sendCommand(string command, uint16_t node)
{
    return sendCommand(command, "", node);
}

/**** END ****/

/**** additional functions for registrations ****/

// function for sending registrations
unsigned long Radio::sendRegistration(ModuleType type, int index, int pin)
{
    registration_payload payload;
    payload.request_id = this->generateRequestID();
    payload.module_type = type;
    payload.index = index;
    payload.pin = pin;
    sendRegistration(payload);
    return payload.request_id;
}
// function for sending registrations
unsigned long Radio::sendRegistration(ModuleType type)
{
    return sendRegistration(type, -1, -1);
}

/**** END ****/

// function for genrerating request_ids based on request_counter and node_id
unsigned long Radio::generateRequestID()
{
    unsigned long request_id = 0;
    request_id += _mesh.getNodeID();
    request_id += (_request_counter * 100);
    _request_counter++;
    return request_id;
};

// function for checking connection and reconnecting as well as reregistration at master if necessary
void Radio::checkConnection()
{
    if (!this->isMaster())
    {
        if (!_mesh.checkConnection())
        {
            printf("Reconnecting ...\n");
            _mesh.renewAddress();
            this->_registrationFunction();
        }
    }
}

// function which takes a request_id and waits for a response with this id
response_payload Radio::waitForAnswer(unsigned long searched_request_id)
{
    unsigned long timout = 3000;
    response_payload nullPayload;
    if (_last_failed_request_id == searched_request_id)
    {
        return nullPayload;
    }
    unsigned long startTime = millis();
    while ((millis() - startTime) < timout)
    {
        if (_last_response.request_id == searched_request_id)
        {
            return _last_response;
        } 
    }
    return nullPayload;
    
}

// function which prints out all nodes connected to the network
void Radio::printMesh()
{
    printf("\n");
    printf("********Assigned Addresses********\n");
    printf("NodeID: %d RF24Network Address: 00\n", _mesh.getNodeID());

    for (int i = 0; i < _mesh.addrListTop; i++)
    {
        printf("NodeID: %d RF24Network Address: 0%d\n", _mesh.addrList[i].nodeID, _mesh.addrList[i].address);
    }
    printf("**********************************\n");
    printf("\n");
}

int16_t Radio::getNodeID(uint32_t adress)
{
    return _mesh.getNodeID(adress);
}

void Radio::updateAndLog()
{
    Radio *this_radio = this;
    _cts = cancellation_token_source();
    auto token = _cts.get_token();

    _background = create_task([this_radio, token] {
        unsigned long displayTimer = millis();
        while (1)
        {
            if (token.is_canceled())
            {
                printf("Stopping this task\n");
                // TODO: Perform any necessary cleanup here...

                // Cancel the current task.
                cancel_current_task();
            }
            else
            {
                this_radio->update();
            }
        }
    },token);

    printf("Started Updating\n");
}

void Radio::stopTask()
{
    _cts.cancel();
    _background.wait();
}
