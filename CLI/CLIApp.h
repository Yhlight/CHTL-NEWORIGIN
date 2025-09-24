#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>

namespace CHTL {
namespace CLI {

class CommandLineParser;
class CompilerMonitor;

class CLIApp {
public:
    CLIApp();
    ~CLIApp() = default;
    
    // Main execution
    int run(int argc, char* argv[]);
    
    // Configuration
    void setVersion(const std::string& version) { version_ = version; }
    void setDescription(const std::string& description) { description_ = description; }
    
    // Command handling
    void registerCommand(const std::string& name, std::function<int(const std::vector<std::string>&)> handler);
    
private:
    std::string version_;
    std::string description_;
    std::unique_ptr<CommandLineParser> parser_;
    std::unique_ptr<CompilerMonitor> monitor_;
    std::map<std::string, std::function<int(const std::vector<std::string>&)>> commands_;
    
    // Command implementations
    int compileCommand(const std::vector<std::string>& args);
    int helpCommand(const std::vector<std::string>& args);
    int versionCommand(const std::vector<std::string>& args);
    int validateCommand(const std::vector<std::string>& args);
    int formatCommand(const std::vector<std::string>& args);
    
    // Utility methods
    void printUsage() const;
    void printVersion() const;
    void printHelp(const std::string& command = "") const;
};

} // namespace CLI
} // namespace CHTL