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

#pragma once

#include <cpprest/http_listener.h>
#include <cpprest/json.h>
#include <cpprest/uri.h>
#include <cpprest/ws_client.h>
#include <cpprest/containerstream.h>
#include <cpprest/interopstream.h>
#include <cpprest/rawptrstream.h>
#include <cpprest/producerconsumerstream.h>
#include <basic_controller.hpp>
#include "radio.hpp"
#include "exceptions.hpp"

using namespace cfx;

class MeshMasterRestServer : public BasicController, Controller {

private:
    bool _toggle = false;
    Radio *_radio = nullptr;
    string _callback_url = "http://localhost:8000/submit/callbacks";
    static json::value responseNotImpl(const http::method &method);

public:
    MeshMasterRestServer() : BasicController() {}

    ~MeshMasterRestServer() = default;

    void setRadio(Radio *radio) {
        _radio = radio;
    }

    void handleGet(http_request message) override;

    void handlePut(http_request message) override;

    void handlePost(http_request message) override;

    void handlePatch(http_request message) override;

    void handleDelete(http_request message) override;

    void handleHead(http_request message) override;

    void handleOptions(http_request message) override;

    void handleTrace(http_request message) override;

    void handleConnect(http_request message) override;

    void handleMerge(http_request message) override;

    void initRestOpHandlers() override;

    void registrationCallback(registration_payload_struct payload, int16_t node);
};