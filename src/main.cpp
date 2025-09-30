#include <iostream>
#include <string>
#include "CHTL/CHTLContext.h"

int main(int argc, char* argv[]) {
    std::cout << "CHTL Compiler v1.0.0" << std::endl;
    std::cout << "===================" << std::endl;
    
    if (argc < 2) {
        std::cout << "Usage: chtl <input_file> [output_dir]" << std::endl;
        std::cout << "       chtl --version" << std::endl;
        std::cout << "       chtl --help" << std::endl;
        return 1;
    }
    
    std::string inputFile = argv[1];
    std::string outputDir = (argc > 2) ? argv[2] : ".";
    
    // Handle special commands
    if (inputFile == "--version") {
        std::cout << "CHTL Compiler v1.0.0" << std::endl;
        return 0;
    }
    
    if (inputFile == "--help") {
        std::cout << "CHTL Compiler - A modern HTML preprocessor" << std::endl;
        std::cout << std::endl;
        std::cout << "Usage:" << std::endl;
        std::cout << "  chtl <input_file> [output_dir]" << std::endl;
        std::cout << "  chtl --version" << std::endl;
        std::cout << "  chtl --help" << std::endl;
        std::cout << std::endl;
        std::cout << "Options:" << std::endl;
        std::cout << "  --version    Show version information" << std::endl;
        std::cout << "  --help       Show this help message" << std::endl;
        std::cout << std::endl;
        std::cout << "Examples:" << std::endl;
        std::cout << "  chtl input.chtl" << std::endl;
        std::cout << "  chtl input.chtl output/" << std::endl;
        return 0;
    }
    
    try {
        // Create CHTL context
        CHTL::CHTLContext context;
        context.initialize();
        
        // Set configuration options
        context.setConfiguration("DEBUG_MODE", "false");
        context.setConfiguration("INDEX_INITIAL_COUNT", "0");
        context.setConfiguration("DISABLE_DEFAULT_NAMESPACE", "false");
        
        // Compile the input file
        std::cout << "Compiling: " << inputFile << std::endl;
        
        if (context.compile(inputFile, outputDir)) {
            std::cout << "Compilation successful!" << std::endl;
            
            // Output results
            std::cout << "\nHTML Output:" << std::endl;
            std::cout << "------------" << std::endl;
            std::cout << context.getHtmlOutput() << std::endl;
            
            std::cout << "\nCSS Output:" << std::endl;
            std::cout << "-----------" << std::endl;
            std::cout << context.getCssOutput() << std::endl;
            
            std::cout << "\nJavaScript Output:" << std::endl;
            std::cout << "------------------" << std::endl;
            std::cout << context.getJsOutput() << std::endl;
            
        } else {
            std::cerr << "Compilation failed: " << context.getErrorMessage() << std::endl;
            return 1;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}