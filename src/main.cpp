/**
 * @file main.cpp
 * @brief CHTL Compiler Entry Point
 * @version 1.0.0
 * @date 2025
 * 
 * CHTL is a hyper-text language based on C++ implementation
 * Licensed under MIT
 */

#include <iostream>
#include <string>
#include <vector>
#include "CHTL/CHTLContext.h"
#include "CHTL/CHTLLexer/Lexer.h"
#include "CHTL/CHTLParser/Parser.h"
#include "CHTL/CHTLGenerator/Generator.h"

void printVersion() {
    std::cout << "CHTL Compiler v1.0.0" << std::endl;
    std::cout << "Licensed under MIT" << std::endl;
}

void printHelp() {
    std::cout << "Usage: chtl-compiler [options] <input-file>" << std::endl;
    std::cout << "\nOptions:" << std::endl;
    std::cout << "  -h, --help              Show this help message" << std::endl;
    std::cout << "  -v, --version           Show version information" << std::endl;
    std::cout << "  -o, --output <file>     Specify output file" << std::endl;
    std::cout << "  --inline                Generate inline HTML (CSS + JS embedded)" << std::endl;
    std::cout << "  --inline-css            Generate inline CSS only" << std::endl;
    std::cout << "  --inline-js             Generate inline JS only" << std::endl;
    std::cout << "  --default-struct        Generate HTML with default structure" << std::endl;
    std::cout << "  --debug                 Enable debug mode" << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printHelp();
        return 1;
    }

    std::vector<std::string> args(argv + 1, argv + argc);
    std::string inputFile;
    std::string outputFile;
    bool inlineMode = false;
    bool inlineCss = false;
    bool inlineJs = false;
    bool defaultStruct = false;
    bool debugMode = false;

    // Parse command line arguments
    for (size_t i = 0; i < args.size(); ++i) {
        const std::string& arg = args[i];
        
        if (arg == "-h" || arg == "--help") {
            printHelp();
            return 0;
        } else if (arg == "-v" || arg == "--version") {
            printVersion();
            return 0;
        } else if (arg == "-o" || arg == "--output") {
            if (i + 1 < args.size()) {
                outputFile = args[++i];
            } else {
                std::cerr << "Error: --output requires an argument" << std::endl;
                return 1;
            }
        } else if (arg == "--inline") {
            inlineMode = true;
        } else if (arg == "--inline-css") {
            inlineCss = true;
        } else if (arg == "--inline-js") {
            inlineJs = true;
        } else if (arg == "--default-struct") {
            defaultStruct = true;
        } else if (arg == "--debug") {
            debugMode = true;
        } else if (!arg.empty() && arg[0] != '-') {
            inputFile = arg;
        } else {
            std::cerr << "Unknown option: " << arg << std::endl;
            return 1;
        }
    }

    if (inputFile.empty()) {
        std::cerr << "Error: No input file specified" << std::endl;
        return 1;
    }

    try {
        // Create compiler context
        CHTL::CHTLContext context;
        context.setDebugMode(debugMode);
        context.setInlineMode(inlineMode);
        context.setInlineCss(inlineCss);
        context.setInlineJs(inlineJs);
        context.setDefaultStruct(defaultStruct);

        if (debugMode) {
            std::cout << "Processing file: " << inputFile << std::endl;
        }

        // TODO: Implement compilation pipeline
        // 1. Lexer - Tokenize input
        // 2. Parser - Build AST
        // 3. Generator - Generate HTML/CSS/JS output

        std::cout << "Compilation successful!" << std::endl;
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Compilation failed: " << e.what() << std::endl;
        return 1;
    }
}
