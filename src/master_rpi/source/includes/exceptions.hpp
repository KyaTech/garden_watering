#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <string>
using namespace std;

class PrintableException : public std::exception {
    public:
        virtual std::string giveMessage() = 0;
        void printException() {
            std::cout << "!!!" << this->giveMessage() << "!!!" << std::endl;
        }
};

class ResponseNotFoundException : public PrintableException {
    private:
        unsigned long _request_id;
    public:
        ResponseNotFoundException(unsigned long request_id) : _request_id(request_id){};
        std::string giveMessage() override {
            stringstream strstream;
            strstream << "Could not get the response #" << _request_id;
            return strstream.str();
        }
        unsigned long getRequestID() {
            return _request_id;
        }
};

class PayloadNotSendableException : public PrintableException {
    private:
        unsigned long _request_id = 0;
        string _message;

    public:
        PayloadNotSendableException(string message,unsigned long request_id) : _message(message) ,_request_id(request_id) {};
        PayloadNotSendableException(string message) : _message(message) {};
        std::string giveMessage() override {
            stringstream strstream;
            if (_request_id == 0) {
                strstream << _message;
            } else {
                strstream << _message << " #"<< _request_id;
            }
            return strstream.str();
        }
        unsigned long getRequestID() {
            return _request_id;
        }
};

#endif