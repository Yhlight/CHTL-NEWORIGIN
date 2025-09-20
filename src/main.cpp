#include <iostream>
#include <string>
#include <vector>
#include "CHTLCommon.h"
#include "UnifiedScanner.h"
#include "CompilerDispatcher.h"

using namespace CHTL;

void printUsage(const char* programName) {
    std::cout << "CHTL Compiler v1.0.0\n";
    std::cout << "Usage: " << programName << " [options] <input_file>\n";
    std::cout << "\nOptions:\n";
    std::cout << "  -h, --help              Show this help message\n";
    std::cout << "  -o, --output <file>     Specify output file\n";
    std::cout << "  --default-struct        Generate HTML with default structure\n";
    std::cout << "  --inline                Generate inline HTML file\n";
    std::cout << "  --inline-css            Inline CSS into HTML\n";
    std::cout << "  --inline-js             Inline JavaScript into HTML\n";
    std::cout << "  -v, --version           Show version information\n";
    std::cout << "  --debug                 Enable debug mode\n";
    std::cout << "\nExamples:\n";
    std::cout << "  " << programName << " input.chtl\n";
    std::cout << "  " << programName << " -o output.html input.chtl\n";
    std::cout << "  " << programName << " --inline --default-struct input.chtl\n";
}

void printVersion() {
    std::cout << "CHTL Compiler v1.0.0\n";
    std::cout << "Copyright (c) 2024 CHTL Team\n";
    std::cout << "MIT License\n";
}

struct MainCompilerOptions {
    std::string inputFile;
    std::string outputFile;
    bool defaultStruct = false;
    bool inlineOutput = false;
    bool inlineCSS = false;
    bool inlineJS = false;
    bool debugMode = false;
    bool showHelp = false;
    bool showVersion = false;
};

MainCompilerOptions parseArguments(int argc, char* argv[]) {
    MainCompilerOptions options;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "-h" || arg == "--help") {
            options.showHelp = true;
        } else if (arg == "-v" || arg == "--version") {
            options.showVersion = true;
        } else if (arg == "-o" || arg == "--output") {
            if (i + 1 < argc) {
                options.outputFile = argv[++i];
            } else {
                std::cerr << "Error: --output requires a filename\n";
                exit(1);
            }
        } else if (arg == "--default-struct") {
            options.defaultStruct = true;
        } else if (arg == "--inline") {
            options.inlineOutput = true;
        } else if (arg == "--inline-css") {
            options.inlineCSS = true;
        } else if (arg == "--inline-js") {
            options.inlineJS = true;
        } else if (arg == "--debug") {
            options.debugMode = true;
        } else if (arg[0] != '-') {
            // Input file
            if (options.inputFile.empty()) {
                options.inputFile = arg;
            } else {
                std::cerr << "Error: Multiple input files specified\n";
                exit(1);
            }
        } else {
            std::cerr << "Error: Unknown option " << arg << "\n";
            exit(1);
        }
    }
    
    return options;
}

int main(int argc, char* argv[]) {
    try {
        MainCompilerOptions mainOptions = parseArguments(argc, argv);
        
        // Convert to CHTL CompilerOptions
        CompilerOptions options;
        options.outputFile = mainOptions.outputFile;
        options.defaultStruct = mainOptions.defaultStruct;
        options.inlineOutput = mainOptions.inlineOutput;
        options.inlineCSS = mainOptions.inlineCSS;
        options.inlineJS = mainOptions.inlineJS;
        options.debugMode = mainOptions.debugMode;
        
        if (mainOptions.showHelp) {
            printUsage(argv[0]);
            return 0;
        }
        
        if (mainOptions.showVersion) {
            printVersion();
            return 0;
        }
        
        if (mainOptions.inputFile.empty()) {
            std::cerr << "Error: No input file specified\n";
            printUsage(argv[0]);
            return 1;
        }
        
        // Check if input file exists
        if (!FileSystem::fileExists(mainOptions.inputFile)) {
            std::cerr << "Error: Input file '" << mainOptions.inputFile << "' not found\n";
            return 1;
        }
        
        // Set default output file if not specified
        if (options.outputFile.empty()) {
            std::string basename = FileSystem::getBasename(mainOptions.inputFile);
            size_t dotPos = basename.find_last_of('.');
            if (dotPos != std::string::npos) {
                basename = basename.substr(0, dotPos);
            }
            options.outputFile = basename + ".html";
        }
        
        std::cout << "CHTL Compiler - Processing " << mainOptions.inputFile << "\n";
        
        // Initialize compiler dispatcher
        CompilerDispatcher dispatcher;
        dispatcher.setOptions(options);
        
        // Compile the file
        dispatcher.compileFile(mainOptions.inputFile);
        
        std::cout << "Compilation completed successfully!\n";
        std::cout << "Output: " << options.outputFile << "\n";
        
        return 0;
        
    } catch (const CHTLException& e) {
        std::cerr << "CHTL Error: " << e.what() << "\n";
        if (e.getLine() > 0) {
            std::cerr << "  Location: line " << e.getLine();
            if (e.getColumn() > 0) {
                std::cerr << ", column " << e.getColumn();
            }
            std::cerr << "\n";
        }
        return 1;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
        
    } catch (...) {
        std::cerr << "Unknown error occurred\n";
        return 1;
    }
}