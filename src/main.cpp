#include <iostream>
#include <string>
#include <vector>
#include "CHTL/CHTLContext.h"

int main(int argc, char* argv[]) {
    std::cout << "CHTL Compiler v1.0.0" << std::endl;
    std::cout << "====================" << std::endl;
    
    if (argc < 2) {
        std::cout << "Usage: chtl <input_file> [options]" << std::endl;
        std::cout << "Options:" << std::endl;
        std::cout << "  --html    Generate HTML output" << std::endl;
        std::cout << "  --css     Generate CSS output" << std::endl;
        std::cout << "  --js      Generate JavaScript output" << std::endl;
        std::cout << "  --all     Generate all outputs (default)" << std::endl;
        std::cout << "  --debug   Show debug information" << std::endl;
        return 1;
    }
    
    // Check for help flag
    std::string firstArg = argv[1];
    if (firstArg == "--help" || firstArg == "-h") {
        std::cout << "Usage: chtl <input_file> [options]" << std::endl;
        std::cout << "Options:" << std::endl;
        std::cout << "  --html    Generate HTML output" << std::endl;
        std::cout << "  --css     Generate CSS output" << std::endl;
        std::cout << "  --js      Generate JavaScript output" << std::endl;
        std::cout << "  --all     Generate all outputs (default)" << std::endl;
        std::cout << "  --debug   Show debug information" << std::endl;
        return 0;
    }
    
    std::string inputFile = argv[1];
    bool generateHTML = false;
    bool generateCSS = false;
    bool generateJS = false;
    bool generateAll = true;
    bool debug = false;
    
    // Parse command line arguments
    for (int i = 2; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--html") {
            generateHTML = true;
            generateAll = false;
        } else if (arg == "--css") {
            generateCSS = true;
            generateAll = false;
        } else if (arg == "--js") {
            generateJS = true;
            generateAll = false;
        } else if (arg == "--all") {
            generateAll = true;
        } else if (arg == "--debug") {
            debug = true;
        } else {
            std::cout << "Unknown option: " << arg << std::endl;
            return 1;
        }
    }
    
    // If no specific output is requested, generate all
    if (!generateHTML && !generateCSS && !generateJS) {
        generateAll = true;
    }
    
    try {
        // Create CHTL context
        CHTL::CHTLContext context;
        
        // Compile the input file
        std::cout << "Compiling " << inputFile << "..." << std::endl;
        
        if (!context.compileFile(inputFile)) {
            std::cerr << "Compilation failed!" << std::endl;
            
            auto errors = context.getAllErrors();
            for (const auto& error : errors) {
                std::cerr << "Error: " << error << std::endl;
            }
            
            return 1;
        }
        
        std::cout << "Compilation successful!" << std::endl;
        
        // Show debug information if requested
        if (debug) {
            std::cout << "\nDebug Information:" << std::endl;
            std::cout << context.getDebugInfo() << std::endl;
        }
        
        // Generate output
        if (generateAll || generateHTML) {
            std::string html = context.generateHTML();
            if (!html.empty()) {
                std::cout << "\nHTML Output:" << std::endl;
                std::cout << "============" << std::endl;
                std::cout << html << std::endl;
            }
        }
        
        if (generateAll || generateCSS) {
            std::string css = context.generateCSS();
            if (!css.empty()) {
                std::cout << "\nCSS Output:" << std::endl;
                std::cout << "===========" << std::endl;
                std::cout << css << std::endl;
            }
        }
        
        if (generateAll || generateJS) {
            std::string js = context.generateJS();
            if (!js.empty()) {
                std::cout << "\nJavaScript Output:" << std::endl;
                std::cout << "==================" << std::endl;
                std::cout << js << std::endl;
            }
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}