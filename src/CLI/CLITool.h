#ifndef CHTL_CLI_TOOL_H
#define CHTL_CLI_TOOL_H

#include "../Common.h"
#include <iostream>
#include <fstream>

namespace CHTL {
namespace CLI {

class CLITool {
public:
    struct Options {
        String inputFile;
        String outputFile;
        String outputDir = "./dist";
        bool watch = false;
        bool serve = false;
        int port = 3000;
        bool sourcemap = false;
        bool minify = false;
        bool verbose = false;
        String mode = "development"; // "development" or "production"
    };
    
    static int run(int argc, char* argv[]) {
        Options opts = parseArguments(argc, argv);
        
        if (opts.inputFile.empty()) {
            printHelp();
            return 1;
        }
        
        // Compile
        if (!compile(opts)) {
            return 1;
        }
        
        // Watch mode
        if (opts.watch) {
            watch(opts);
        }
        
        // Serve mode
        if (opts.serve) {
            serve(opts);
        }
        
        return 0;
    }
    
private:
    static Options parseArguments(int argc, char* argv[]) {
        Options opts;
        
        for (int i = 1; i < argc; i++) {
            String arg = argv[i];
            
            if (arg == "-i" || arg == "--input") {
                if (i + 1 < argc) opts.inputFile = argv[++i];
            }
            else if (arg == "-o" || arg == "--output") {
                if (i + 1 < argc) opts.outputFile = argv[++i];
            }
            else if (arg == "-d" || arg == "--output-dir") {
                if (i + 1 < argc) opts.outputDir = argv[++i];
            }
            else if (arg == "-w" || arg == "--watch") {
                opts.watch = true;
            }
            else if (arg == "-s" || arg == "--serve") {
                opts.serve = true;
            }
            else if (arg == "-p" || arg == "--port") {
                if (i + 1 < argc) opts.port = std::stoi(argv[++i]);
            }
            else if (arg == "--sourcemap") {
                opts.sourcemap = true;
            }
            else if (arg == "--minify") {
                opts.minify = true;
            }
            else if (arg == "-v" || arg == "--verbose") {
                opts.verbose = true;
            }
            else if (arg == "--mode") {
                if (i + 1 < argc) opts.mode = argv[++i];
            }
            else if (arg == "-h" || arg == "--help") {
                printHelp();
                exit(0);
            }
        }
        
        return opts;
    }
    
    static bool compile(const Options& opts) {
        if (opts.verbose) {
            std::cout << "Compiling " << opts.inputFile << "...\n";
        }
        
        // Read input
        std::ifstream input(opts.inputFile);
        if (!input.is_open()) {
            std::cerr << "Error: Cannot open file " << opts.inputFile << "\n";
            return false;
        }
        
        String content((std::istreambuf_iterator<char>(input)),
                       std::istreambuf_iterator<char>());
        
        // Compile would happen here
        
        if (opts.verbose) {
            std::cout << "Compilation complete!\n";
        }
        
        return true;
    }
    
    static void watch(const Options& opts) {
        std::cout << "Watching for changes...\n";
        // Watch implementation
    }
    
    static void serve(const Options& opts) {
        std::cout << "Starting dev server on port " << opts.port << "...\n";
        // Server implementation
    }
    
    static void printHelp() {
        std::cout << "CHTL Compiler v2.0.0\n\n";
        std::cout << "Usage: chtl [options]\n\n";
        std::cout << "Options:\n";
        std::cout << "  -i, --input <file>      Input CHTL file\n";
        std::cout << "  -o, --output <file>     Output file\n";
        std::cout << "  -d, --output-dir <dir>  Output directory (default: ./dist)\n";
        std::cout << "  -w, --watch             Watch mode\n";
        std::cout << "  -s, --serve             Start dev server\n";
        std::cout << "  -p, --port <port>       Server port (default: 3000)\n";
        std::cout << "  --sourcemap             Generate source maps\n";
        std::cout << "  --minify                Minify output\n";
        std::cout << "  --mode <mode>           Build mode (development|production)\n";
        std::cout << "  -v, --verbose           Verbose output\n";
        std::cout << "  -h, --help              Show help\n";
    }
};

} // namespace CLI
} // namespace CHTL

#endif // CHTL_CLI_TOOL_H
