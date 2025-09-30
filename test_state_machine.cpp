#include "CHTL/CHTLState.h"
#include <iostream>
#include <vector>
#include <string>

int main() {
    std::cout << "Testing CHTL State Machine..." << std::endl;
    
    CHTL::CHTLStateManager stateManager;
    stateManager.initialize();
    
    // Test initial state
    std::cout << "Initial state: " << stateManager.getCurrentState()->getStateName() << std::endl;
    
    // Test token processing
    std::vector<std::string> tokens = {"html", "{", "style", "}", "text", "}"};
    
    for (const auto& token : tokens) {
        std::cout << "Processing token: '" << token << "'" << std::endl;
        bool success = stateManager.processToken(token);
        if (success) {
            std::cout << "  -> New state: " << stateManager.getCurrentState()->getStateName() << std::endl;
        } else {
            std::cout << "  -> Error: " << stateManager.getErrorMessage() << std::endl;
            break;
        }
    }
    
    // Test error handling
    std::cout << "\nTesting error handling..." << std::endl;
    stateManager.reset();
    bool success = stateManager.processToken("invalid_token");
    if (!success) {
        std::cout << "Error correctly caught: " << stateManager.getErrorMessage() << std::endl;
    }
    
    std::cout << "\nState machine test completed!" << std::endl;
    return 0;
}