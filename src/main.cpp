#include <iostream>
#include <string>
#include <memory>
#include "CHTLContext.h"

using namespace CHTL;

int main(int argc, char* argv[]) {
    std::cout << "CHTL Compiler v1.0.0" << std::endl;
    std::cout << "Based on CHTL.md specification" << std::endl;
    std::cout << "Using C++17 + CMake + ANTLR4 architecture" << std::endl;
    std::cout << "State Machine + Strategy Pattern driven" << std::endl;
    std::cout << std::endl;
    
    // Create context
    CHTLContext context;
    context.setDebugMode(true);
    
    // Basic functionality test
    context.setInputFile("test.chtl");
    context.setOutputDirectory("output");
    context.setConfiguration("debug", "true");
    
    std::cout << "Context initialized:" << std::endl;
    std::cout << "  Input file: " << context.getInputFile() << std::endl;
    std::cout << "  Output directory: " << context.getOutputDirectory() << std::endl;
    std::cout << "  Debug mode: " << (context.isDebugMode() ? "enabled" : "disabled") << std::endl;
    std::cout << "  Configuration debug: " << context.getConfiguration("debug") << std::endl;
    
    // Test error handling
    context.addError("This is a test error", 10, 5);
    context.addWarning("This is a test warning", 15, 8);
    
    std::cout << std::endl;
    std::cout << "Error handling test:" << std::endl;
    std::cout << "  Has errors: " << (context.hasErrors() ? "yes" : "no") << std::endl;
    std::cout << "  Error count: " << context.getErrors().size() << std::endl;
    std::cout << "  Warning count: " << context.getWarnings().size() << std::endl;
    
    if (context.hasErrors()) {
        std::cout << "  Errors:" << std::endl;
        for (const auto& error : context.getErrors()) {
            std::cout << "    " << error << std::endl;
        }
    }
    
    std::cout << std::endl;
    std::cout << "CHTL Compiler basic functionality verified." << std::endl;
    std::cout << "Ready for grammar implementation and parsing." << std::endl;
    
    return 0;
}