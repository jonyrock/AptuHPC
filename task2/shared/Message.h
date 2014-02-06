#pragma once

#include <string>

struct Message {
    std::string senderName;
    std::string message;

    Message(std::string senderName_, std::string message_) :
    senderName(senderName_),
    message(message_) {
        
    }
};