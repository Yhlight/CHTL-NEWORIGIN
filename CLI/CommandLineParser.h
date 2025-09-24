#pragma once

#include <string>
#include <vector>
#include <map>

namespace CHTL {
namespace CLI {

struct CommandLineOptions {
    bool inlineOutput = false;
    bool inlineCss = false;
    bool inlineJs = false;
    bool defaultStruct = false;
    bool verbose = false;
    bool debug = false;
    bool watch = false;
    bool inPlace = false;
    
    std::string outputDir = "./build";
    std::string indentSize = "2";
    std::vector<std::string> inputFiles;
    std::vector<std::string> includePaths;
    std::vector<std::string> excludePaths;
};

class CommandLineParser {
public:
    CommandLineParser();
    ~CommandLineParser() = default;
    
    // Main parsing method
    std::vector<std::string> parse(int argc, char* argv[]);
    
    // Option parsing
    CommandLineOptions parseOptions(const std::vector<std::string>& args);
    
    // Validation
    bool validateOptions(const CommandLineOptions& options) const;
    std::vector<std::string> getValidationErrors(const CommandLineOptions& options) const;
    
    // Help generation
    std::string generateHelp(const std::string& command = "") const;
    std::string generateUsage() const;
    
private:
    std::map<std::string, std::string> optionDescriptions_;
    std::map<std::string, std::string> optionAliases_;
    
    void initializeOptionDescriptions();
    void initializeOptionAliases();
    
    bool isOption(const std::string& arg) const;
    std::string getOptionName(const std::string& arg) const;
    std::string getOptionValue(const std::string& arg) const;
    
    void parseOption(const std::string& option, CommandLineOptions& options) const;
    void parseShortOption(const std::string& option, CommandLineOptions& options) const;
    void parseLongOption(const std::string& option, CommandLineOptions& options) const;
};

} // namespace CLI
} // namespace CHTL