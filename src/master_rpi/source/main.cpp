

/** RF24Mesh_Example_Master.ino by TMRh20
 *
 * Note: This sketch only functions on -Arduino Due-
 *
 * This example sketch shows how to manually configure a node via RF24Mesh as a master node, which
 * will receive all data from sensor nodes.
 *
 * The nodes can change physical or logical position in the network, and reconnect through different
 * routing nodes as required. The master node manages the address assignments for the individual nodes
 * in a manner similar to DHCP.
 *
 */

#include <string>
#include <sstream>
#include <iostream>

#include <RF24/RF24.h>
#include <RF24Network/RF24Network.h>
#include <RF24Mesh/RF24Mesh.h>

#include <usr_interrupt_handler.hpp>
#include <runtime_utils.hpp>
#include "microsvc_controller.hpp"

#include "payloads.hpp"
#include "func.hpp"
#include "radio.hpp"

using namespace web;
using namespace cfx;
using namespace std;


Radio radio = Radio();

void registrationCallback(registration_payload_struct payload, RF24NetworkHeader header) {
    printRegistration(payload, radio.getNodeID(header.from_node));
    radio.sendSimpleResponse(SimpleResponse::OK, payload, header);
}

void requestCallback(request_payload_struct payload, RF24NetworkHeader header) {
    printRequest(payload);
}

void responseCallback(response_payload_struct payload, RF24NetworkHeader header) {
    printResponse(payload);
}

int main(int argc, char **argv) {

    MeshMasterRestServer server;
    server.setEndpoint("http://0.0.0.0:8080/api/v1");
    server.setRadio(&radio);

    radio.beginMesh(0);
    radio.setRequestCallback(requestCallback);
    radio.setResponseCallback(responseCallback);
    radio.setRegistrationCallback(registrationCallback);

    try {
        // wait for server initialization...
        server.accept().wait();
        std::cout << "Modern C++ Microservice now listening for requests at: " << server.endpoint() << '\n';

        radio.updateAndLog();
        InterruptHandler::waitForUserInterrupt();

        server.shutdown().wait();
    }
    catch (std::exception &e) {
        std::cerr << "something wrong happen! :(" << '\n';
    }
    catch (...) {
        RuntimeUtils::printStackTrace();
    }

    return 0;
}

      
      
      