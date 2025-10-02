/**
 * @file SaltBridge.cpp
 * @brief Implementation of Salt Bridge
 */

#include "SaltBridge.h"
#include <iostream>

namespace SharedCore {

SaltBridge& SaltBridge::getInstance() {
    static SaltBridge instance;
    return instance;
}

SaltBridge::SaltBridge()
    : debugMode_(false)
{
}

void SaltBridge::registerHandler(MessageType type, MessageHandler handler) {
    handlers_[type] = handler;
}

Message SaltBridge::sendMessage(const Message& message) {
    if (debugMode_) {
        std::cout << "[SaltBridge] Sending message from " << message.source 
                  << " to " << message.target << std::endl;
    }
    
    auto it = handlers_.find(message.type);
    if (it != handlers_.end()) {
        return it->second(message);
    }
    
    // No handler found
    Message errorMsg(MessageType::ERROR, message.target, message.source, 
                     "No handler registered for message type");
    return errorMsg;
}

std::string SaltBridge::requestScriptProcessing(const std::string& scriptContent) {
    Message msg(MessageType::SCRIPT_PROCESS, "CHTL", "CHTLJS", scriptContent);
    Message response = sendMessage(msg);
    return response.payload;
}

std::string SaltBridge::requestCHTLJSParsing(const std::string& code) {
    Message msg(MessageType::PARSE_REQUEST, "CHTL", "CHTLJS", code);
    Message response = sendMessage(msg);
    return response.payload;
}

std::string SaltBridge::requestCHTLParsing(const std::string& code) {
    Message msg(MessageType::PARSE_REQUEST, "CHTLJS", "CHTL", code);
    Message response = sendMessage(msg);
    return response.payload;
}

std::string SaltBridge::queryContext(const std::string& query) {
    Message msg(MessageType::CONTEXT_QUERY, "CHTLJS", "CHTL", query);
    Message response = sendMessage(msg);
    return response.payload;
}

void SaltBridge::shareContext(const std::string& key, const std::string& value) {
    sharedContext_[key] = value;
}

std::string SaltBridge::getSharedContext(const std::string& key) const {
    auto it = sharedContext_.find(key);
    return (it != sharedContext_.end()) ? it->second : "";
}

} // namespace SharedCore
