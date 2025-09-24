#include "CLIApp.h"
#include "CommandLineParser.h"
#include "CompilerMonitor.h"
#include <iostream>
#include <functional>

namespace CHTL {
namespace CLI {

CLIApp::CLIApp() 
    : version_("1.0.0")
    , description_("CHTL - C++ HyperText Language Compiler") {
    parser_ = std::make_unique<CommandLineParser>();
    monitor_ = std::make_unique<CompilerMonitor>();
    
    // Register default commands
    registerCommand("compile", [this](const std::vector<std::string>& args) { return compileCommand(args); });
    registerCommand("help", [this](const std::vector<std::string>& args) { return helpCommand(args); });
    registerCommand("version", [this](const std::vector<std::string>& args) { return versionCommand(args); });
    registerCommand("validate", [this](const std::vector<std::string>& args) { return validateCommand(args); });
    registerCommand("format", [this](const std::vector<std::string>& args) { return formatCommand(args); });
}

int CLIApp::run(int argc, char* argv[]) {
    try {
        auto args = parser_->parse(argc, argv);
        
        if (args.empty()) {
            printUsage();
            return 1;
        }
        
        std::string command = args[0];
        std::vector<std::string> commandArgs(args.begin() + 1, args.end());
        
        auto it = commands_.find(command);
        if (it != commands_.end()) {
            return it->second(commandArgs);
        } else {
            std::cerr << "Unknown command: " << command << std::endl;
            printUsage();
            return 1;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}

void CLIApp::registerCommand(const std::string& name, std::function<int(const std::vector<std::string>&)> handler) {
    commands_[name] = handler;
}

int CLIApp::compileCommand(const std::vector<std::string>& args) {
    std::cout << "Compiling CHTL files..." << std::endl;
    
    // TODO: Implement actual compilation
    // This would use the CHTL compiler, CHTL JS compiler, Scanner, etc.
    
    for (const auto& arg : args) {
        std::cout << "Processing: " << arg << std::endl;
    }
    
    return 0;
}

int CLIApp::helpCommand(const std::vector<std::string>& args) {
    if (args.empty()) {
        printUsage();
    } else {
        printHelp(args[0]);
    }
    return 0;
}

int CLIApp::versionCommand(const std::vector<std::string>& args) {
    printVersion();
    return 0;
}

int CLIApp::validateCommand(const std::vector<std::string>& args) {
    std::cout << "Validating CHTL files..." << std::endl;
    
    // TODO: Implement validation
    for (const auto& arg : args) {
        std::cout << "Validating: " << arg << std::endl;
    }
    
    return 0;
}

int CLIApp::formatCommand(const std::vector<std::string>& args) {
    std::cout << "Formatting CHTL files..." << std::endl;
    
    // TODO: Implement formatting
    for (const auto& arg : args) {
        std::cout << "Formatting: " << arg << std::endl;
    }
    
    return 0;
}

void CLIApp::printUsage() const {
    std::cout << description_ << " v" << version_ << std::endl;
    std::cout << std::endl;
    std::cout << "Usage: chtl <command> [options] [files...]" << std::endl;
    std::cout << std::endl;
    std::cout << "Commands:" << std::endl;
    std::cout << "  compile    Compile CHTL files to HTML/CSS/JS" << std::endl;
    std::cout << "  validate   Validate CHTL syntax" << std::endl;
    std::cout << "  format     Format CHTL files" << std::endl;
    std::cout << "  help       Show help information" << std::endl;
    std::cout << "  version    Show version information" << std::endl;
    std::cout << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --inline          Generate inline HTML file" << std::endl;
    std::cout << "  --inline-css      Inline CSS in HTML" << std::endl;
    std::cout << "  --inline-js       Inline JavaScript in HTML" << std::endl;
    std::cout << "  --default-struct  Add default HTML structure" << std::endl;
    std::cout << "  --output, -o      Output directory" << std::endl;
    std::cout << "  --verbose, -v     Verbose output" << std::endl;
    std::cout << "  --debug           Enable debug mode" << std::endl;
    std::cout << std::endl;
}

void CLIApp::printVersion() const {
    std::cout << description_ << " version " << version_ << std::endl;
    std::cout << "Built with C++20 and CMake" << std::endl;
}

void CLIApp::printHelp(const std::string& command) const {
    if (command == "compile") {
        std::cout << "chtl compile - Compile CHTL files to HTML/CSS/JS" << std::endl;
        std::cout << std::endl;
        std::cout << "Usage: chtl compile [options] <files...>" << std::endl;
        std::cout << std::endl;
        std::cout << "Options:" << std::endl;
        std::cout << "  --output, -o <dir>     Output directory (default: ./build)" << std::endl;
        std::cout << "  --inline               Generate inline HTML file" << std::endl;
        std::cout << "  --default-struct       Add default HTML structure" << std::endl;
        std::cout << "  --watch, -w            Watch for file changes" << std::endl;
        std::cout << "  --verbose, -v          Verbose output" << std::endl;
        std::cout << std::endl;
        std::cout << "Examples:" << std::endl;
        std::cout << "  chtl compile main.chtl" << std::endl;
        std::cout << "  chtl compile --inline --output dist main.chtl" << std::endl;
    } else if (command == "validate") {
        std::cout << "chtl validate - Validate CHTL syntax" << std::endl;
        std::cout << std::endl;
        std::cout << "Usage: chtl validate <files...>" << std::endl;
        std::cout << std::endl;
        std::cout << "This command checks CHTL files for syntax errors and warnings." << std::endl;
    } else if (command == "format") {
        std::cout << "chtl format - Format CHTL files" << std::endl;
        std::cout << std::endl;
        std::cout << "Usage: chtl format [options] <files...>" << std::endl;
        std::cout << std::endl;
        std::cout << "Options:" << std::endl;
        std::cout << "  --in-place, -i         Modify files in place" << std::endl;
        std::cout << "  --indent <size>        Set indentation size (default: 2)" << std::endl;
        std::cout << std::endl;
    } else {
        std::cout << "Unknown command: " << command << std::endl;
        std::cout << "Use 'chtl help' to see available commands." << std::endl;
    }
}

} // namespace CLI
} // namespace CHTL