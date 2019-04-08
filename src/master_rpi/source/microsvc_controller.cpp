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

void MeshMasterRestServer::initRestOpHandlers()
{
    _listener.support(methods::GET, std::bind(&MeshMasterRestServer::handleGet, this, std::placeholders::_1));
    _listener.support(methods::PUT, std::bind(&MeshMasterRestServer::handlePut, this, std::placeholders::_1));
    _listener.support(methods::POST, std::bind(&MeshMasterRestServer::handlePost, this, std::placeholders::_1));
    _listener.support(methods::DEL, std::bind(&MeshMasterRestServer::handleDelete, this, std::placeholders::_1));
    _listener.support(methods::PATCH, std::bind(&MeshMasterRestServer::handlePatch, this, std::placeholders::_1));
}

void MeshMasterRestServer::handleGet(http_request message)
{
    auto path = requestPath(message);
    if (!path.empty())
    {
        if (path[0] == "requests" && path[1] == "sensor")
        {
            string nodeString = getQueryString(message,"node");
            
            if (!nodeString.empty()) {
                int node = std::stoi(nodeString);
                string index = getQueryString(message, "index");
                
                response_payload_struct response_mesh;
                if (index.empty()) {
                    response_mesh = _radio->waitForAnswer(_radio->sendRequest("Moisture",node));
                } else {
                    response_mesh = _radio->waitForAnswer(_radio->sendRequest("Moisture",index,node));
                }

                json::value response;
                response["request_id"] = json::value::number((unsigned int)response_mesh.request_id);
                response["value"] = json::value::string(response_mesh.value);
                message.reply(status_codes::OK, response);
                return;
            } else {
                json::value response;
                response["message"] = json::value::string("The queried node is not available.");
                message.reply(status_codes::BadRequest,response);
            }

            
        }
    }

    message.reply(status_codes::NotFound);
    
}

void MeshMasterRestServer::handlePatch(http_request message)
{
    message.reply(status_codes::NotImplemented, responseNotImpl(methods::PATCH));
}

void MeshMasterRestServer::handlePut(http_request message)
{
    message.reply(status_codes::NotImplemented, responseNotImpl(methods::PUT));
}

void MeshMasterRestServer::handlePost(http_request message)
{
    auto path = requestPath(message);
    if (!path.empty())
    {
        if (path[0] == "controls" && path[1] == "valve")
        {
    
            message
            .extract_json()
            .then([=](json::value request) {
                try
                {
                    json::value controls = request.at("control");
                    json::value index = request.at("index");
                    json::value node = request.at("node");
                    if (controls.is_string() && index.is_integer() && node.is_integer()) {
                        response_payload_struct response_mesh = _radio->waitForAnswer(_radio->sendCommand(controls.as_string(),std::to_string(index.as_integer()), node.as_integer()));

                        json::value response;
                        response["request_id"] = json::value::number((unsigned int)response_mesh.request_id);
                        response["message"] = json::value::string(response_mesh.value);
                        message.reply(status_codes::OK, response);
                    } else {
                        throw json::json_exception(U("Wrong datatypes"));  
                    }

                }
                catch (json::json_exception &e)
                {
                    json::value response;
                    response["message"] = json::value::string("There may be one or more incorrect elements in your request.");
                    message.reply(status_codes::BadRequest,response);
                }
            });
            return;
            
        }
    }

    message.reply(status_codes::NotFound);
    
}

void MeshMasterRestServer::handleDelete(http_request message)
{
    message.reply(status_codes::NotImplemented, responseNotImpl(methods::DEL));
}

void MeshMasterRestServer::handleHead(http_request message)
{
    message.reply(status_codes::NotImplemented, responseNotImpl(methods::HEAD));
}

void MeshMasterRestServer::handleOptions(http_request message)
{
    message.reply(status_codes::NotImplemented, responseNotImpl(methods::OPTIONS));
}

void MeshMasterRestServer::handleTrace(http_request message)
{
    message.reply(status_codes::NotImplemented, responseNotImpl(methods::TRCE));
}

void MeshMasterRestServer::handleConnect(http_request message)
{
    message.reply(status_codes::NotImplemented, responseNotImpl(methods::CONNECT));
}

void MeshMasterRestServer::handleMerge(http_request message)
{
    message.reply(status_codes::NotImplemented, responseNotImpl(methods::MERGE));
}

json::value MeshMasterRestServer::responseNotImpl(const http::method &method)
{
    auto response = json::value::object();
    response["serviceName"] = json::value::string("C++ Mircroservice Sample");
    response["http_method"] = json::value::string(method);
    return response;
}