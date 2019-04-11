//
//  Created by Ivan Mejia on 12/24/16.
//
// MIT License
//
// Copyright (c) 2016 ivmeroLabs.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//

#include <std_micro_service.hpp>
#include "microsvc_controller.hpp"

#include "radio.hpp"

using namespace web;
using namespace http;

void MeshMasterRestServer::initRestOpHandlers() {
    _listener.support(methods::GET, std::bind(&MeshMasterRestServer::handleGet, this, std::placeholders::_1));
    _listener.support(methods::PUT, std::bind(&MeshMasterRestServer::handlePut, this, std::placeholders::_1));
    _listener.support(methods::POST, std::bind(&MeshMasterRestServer::handlePost, this, std::placeholders::_1));
    _listener.support(methods::DEL, std::bind(&MeshMasterRestServer::handleDelete, this, std::placeholders::_1));
    _listener.support(methods::PATCH, std::bind(&MeshMasterRestServer::handlePatch, this, std::placeholders::_1));
}

void MeshMasterRestServer::handleGet(http_request message) {
    auto path = requestPath(message);
    if (!path.empty()) {
        if (path[0] == "nodes") {
            string nodeString = path[1];
            int node = std::stoi(nodeString);

            if (path.size() > 2 && !path[2].empty()) {

                try {
                    if (path[2] == "battery") {
                        response_payload_struct response_mesh = _radio->waitForAnswer(
                                _radio->sendRequest("Battery", node));

                        json::value response;
                        response["request_id"] = json::value::number((uint32_t) response_mesh.request_id);
                        response["battery"] = json::value::number(std::stoi(string(response_mesh.value)));
                        message.reply(status_codes::OK, response);
                        return;
                    } else if (path[2] == "sensors") {
                        response_payload_struct response_mesh{};
                        if (path.size() > 3 && !path[3].empty()) {
                            string indexString = path[3];
                            int index = std::stoi(indexString);
                            response_mesh = _radio->waitForAnswer(_radio->sendRequest("Moisture", indexString, node));
                        } else {
                            response_mesh = _radio->waitForAnswer(_radio->sendRequest("Moisture", node));
                        }

                        json::value response;
                        response["request_id"] = json::value::number((uint32_t) response_mesh.request_id);
                        response["value"] = json::value::string(response_mesh.value);
                        message.reply(status_codes::OK, response);
                        return;

                    } else if (path[2] == "valves") {
                        response_payload_struct response_mesh{};
                        if (path.size() > 3 && !path[3].empty()) {
                            string indexString = path[3];
                            int index = std::stoi(indexString);
                            response_mesh = _radio->waitForAnswer(_radio->sendRequest("State", indexString, node));

                            json::value response;
                            response["request_id"] = json::value::number((uint32_t) response_mesh.request_id);
                            response["state"] = json::value::string(response_mesh.value);
                            message.reply(status_codes::OK, response);
                            return;
                        } else {
                            json::value response;
                            response["error"] = json::value();
                            response["error"]["message"] = json::value::string(
                                    "Please specify an index in the form of a identifier");
                            response["error"]["wanted_url_form"] = json::value::string("/nodes/<node>/valves/<index>");
                            message.reply(status_codes::OK, response);
                            return;
                        }
                    }

                } catch (ResponseNotFoundException &e) {
                    e.printException();

                    json::value response;
                    response["error"] = json::value();
                    response["error"]["request_id"] = json::value::number((uint32_t) e.getRequestID());
                    response["error"]["internalMessage"] = json::value::string(e.giveMessage());
                    message.reply(status_codes::BadRequest, response);
                } catch (PayloadNotSendableException &e) {
                    e.printException();

                    json::value response;
                    response["error"] = json::value();
                    response["error"]["request_id"] = json::value::number((uint32_t) e.getRequestID());
                    response["error"]["internalMessage"] = json::value::string(e.giveMessage());
                    message.reply(status_codes::BadRequest, response);
                }
            } else {
                json::value response;
                response["error"] = json::value();
                response["error"]["message"] = json::value::string(
                        "Please give more information about your destination");
                response["error"]["wanted_url_form"] = json::value::string("/nodes/<node>/...");
                message.reply(status_codes::OK, response);
                return;
            }

        } else {
            json::value response;
            response["error"] = json::value();
            response["error"]["message"] = json::value::string("Please specify a node in the form of a identifier");
            response["error"]["wanted_url_form"] = json::value::string("/nodes/<node>");
            message.reply(status_codes::OK, response);
            return;
        }

        message.reply(status_codes::NotFound);

    }
}

void MeshMasterRestServer::handlePatch(http_request message) {
    message.reply(status_codes::NotImplemented, responseNotImpl(methods::PATCH));
}

void MeshMasterRestServer::handlePut(http_request message) {
    message.reply(status_codes::NotImplemented, responseNotImpl(methods::PUT));
}

void MeshMasterRestServer::handlePost(http_request message) {
    auto path = requestPath(message);
    if (!path.empty()) {
        if (path[0] == "nodes") {
            string nodeString = path[1];
            int node = std::stoi(nodeString);

            if (path.size() > 2 && !path[2].empty()) {

                try {
                    if (path[2] == "valves") {
                        if (path.size() > 4 && !path[3].empty() && !path[4].empty()) {
                            string indexString = path[3];
                            int index = std::stoi(indexString);
                            string command = path[4];


                            bool isValidCommand = (command == "ON" || command == "OFF");
                            if (!isValidCommand) {
                                //throw InvalidCommandException();
                            }

                            response_payload_struct response_mesh = _radio->waitForAnswer(
                                    _radio->sendCommand(command, indexString, node));

                            json::value response;
                            response["request_id"] = json::value::number((uint32_t) response_mesh.request_id);
                            response["message"] = json::value::string(response_mesh.value);
                            message.reply(status_codes::OK, response);
                            return;
                        } else {
                            json::value response;
                            response["error"] = json::value();
                            response["error"]["message"] = json::value::string(
                                    "Please specify an index, and a command in the form of a identifier");
                            response["error"]["wanted_url_form"] = json::value::string(
                                    "/nodes/<node>/valves/<index>/<ON/OFF>");
                            message.reply(status_codes::OK, response);
                            return;
                        }
                    }
                } catch (ResponseNotFoundException &e) {
                    e.printException();

                    json::value response;
                    response["error"] = json::value();
                    response["error"]["request_id"] = json::value::number((uint32_t) e.getRequestID());
                    response["error"]["internalMessage"] = json::value::string(e.giveMessage());
                    message.reply(status_codes::BadRequest, response);
                } catch (PayloadNotSendableException &e) {
                    e.printException();

                    json::value response;
                    response["error"] = json::value();
                    response["error"]["request_id"] = json::value::number((uint32_t) e.getRequestID());
                    response["error"]["internalMessage"] = json::value::string(e.giveMessage());
                    message.reply(status_codes::BadRequest, response);
                }
            }
        }
    }

    message.reply(status_codes::NotFound);
}

void MeshMasterRestServer::handleDelete(http_request message) {
    message.reply(status_codes::NotImplemented, responseNotImpl(methods::DEL));
}

void MeshMasterRestServer::handleHead(http_request message) {
    message.reply(status_codes::NotImplemented, responseNotImpl(methods::HEAD));
}

void MeshMasterRestServer::handleOptions(http_request message) {
    message.reply(status_codes::NotImplemented, responseNotImpl(methods::OPTIONS));
}

void MeshMasterRestServer::handleTrace(http_request message) {
    message.reply(status_codes::NotImplemented, responseNotImpl(methods::TRCE));
}

void MeshMasterRestServer::handleConnect(http_request message) {
    message.reply(status_codes::NotImplemented, responseNotImpl(methods::CONNECT));
}

void MeshMasterRestServer::handleMerge(http_request message) {
    message.reply(status_codes::NotImplemented, responseNotImpl(methods::MERGE));
}

json::value MeshMasterRestServer::responseNotImpl(const http::method &method) {
    auto response = json::value::object();
    response["serviceName"] = json::value::string("C++ Mircroservice Sample");
    response["http_method"] = json::value::string(method);
    return response;
}