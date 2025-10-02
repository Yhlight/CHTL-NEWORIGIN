/**
 * @file SaltBridge.h
 * @brief Salt Bridge - Communication bridge between CHTL and CHTL JS compilers
 * 
 * The Salt Bridge provides bidirectional communication between CHTL and CHTL JS
 * compilers, allowing them to request services from each other.
 */

#ifndef SALT_BRIDGE_H
#define SALT_BRIDGE_H

#include <string>
#include <memory>
#include <functional>
#include <unordered_map>

namespace SharedCore {

/**
 * @enum MessageType
 * @brief Types of messages that can be sent across the salt bridge
 */
enum class MessageType {
    PARSE_REQUEST,          // Request to parse code
    GENERATE_REQUEST,       // Request to generate code
    CONTEXT_QUERY,          // Query context information
    SYMBOL_RESOLVE,         // Resolve a symbol
    STYLE_PROCESS,          // Process style block
    SCRIPT_PROCESS,         // Process script block
    RESPONSE,               // Response message
    ERROR                   // Error message
};

/**
 * @struct Message
 * @brief Message structure for salt bridge communication
 */
struct Message {
    MessageType type;
    std::string source;      // Source compiler (CHTL or CHTLJS)
    std::string target;      // Target compiler
    std::string payload;     // Message payload (JSON or serialized data)
    std::string requestId;   // Unique request ID for tracking
    
    Message(MessageType t = MessageType::RESPONSE,
            const std::string& src = "",
            const std::string& tgt = "",
            const std::string& data = "")
        : type(t), source(src), target(tgt), payload(data) {}
};

/**
 * @typedef MessageHandler
 * @brief Callback function type for handling messages
 */
using MessageHandler = std::function<Message(const Message&)>;

/**
 * @class SaltBridge
 * @brief Main salt bridge class for inter-compiler communication
 */
class SaltBridge {
public:
    static SaltBridge& getInstance();
    
    // Message handling
    void registerHandler(MessageType type, MessageHandler handler);
    Message sendMessage(const Message& message);
    
    // CHTL to CHTL JS communication
    std::string requestScriptProcessing(const std::string& scriptContent);
    std::string requestCHTLJSParsing(const std::string& code);
    
    // CHTL JS to CHTL communication
    std::string requestCHTLParsing(const std::string& code);
    std::string queryContext(const std::string& query);
    
    // Context sharing
    void shareContext(const std::string& key, const std::string& value);
    std::string getSharedContext(const std::string& key) const;
    
    // Debug
    void setDebugMode(bool debug) { debugMode_ = debug; }
    bool isDebugMode() const { return debugMode_; }

private:
    SaltBridge();
    ~SaltBridge() = default;
    
    // Disable copy and move
    SaltBridge(const SaltBridge&) = delete;
    SaltBridge& operator=(const SaltBridge&) = delete;
    
    std::unordered_map<MessageType, MessageHandler> handlers_;
    std::unordered_map<std::string, std::string> sharedContext_;
    bool debugMode_;
};

} // namespace SharedCore

#endif // SALT_BRIDGE_H
