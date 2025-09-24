#include "CommandLineParser.h"
#include "../../Util/StringUtil/StringUtil.h"
#include <algorithm>
#include <sstream>

namespace CHTL {
namespace CLI {

CommandLineParser::CommandLineParser() {
    initializeOptionDescriptions();
    initializeOptionAliases();
}

void CommandLineParser::initializeOptionDescriptions() {
    optionDescriptions_["--inline"] = "Generate inline HTML file";
    optionDescriptions_["--inline-css"] = "Inline CSS in HTML";
    optionDescriptions_["--inline-js"] = "Inline JavaScript in HTML";
    optionDescriptions_["--default-struct"] = "Add default HTML structure";
    optionDescriptions_["--output"] = "Output directory";
    optionDescriptions_["--verbose"] = "Verbose output";
    optionDescriptions_["--debug"] = "Enable debug mode";
    optionDescriptions_["--watch"] = "Watch for file changes";
    optionDescriptions_["--in-place"] = "Modify files in place";
    optionDescriptions_["--indent"] = "Set indentation size";
    optionDescriptions_["--include"] = "Include path";
    optionDescriptions_["--exclude"] = "Exclude path";
    optionDescriptions_["--help"] = "Show help information";
    optionDescriptions_["--version"] = "Show version information";
}

void CommandLineParser::initializeOptionAliases() {
    optionAliases_["-o"] = "--output";
    optionAliases_["-v"] = "--verbose";
    optionAliases_["-w"] = "--watch";
    optionAliases_["-i"] = "--in-place";
    optionAliases_["-h"] = "--help";
}

std::vector<std::string> CommandLineParser::parse(int argc, char* argv[]) {
    std::vector<std::string> args;
    
    for (int i = 1; i < argc; ++i) { // Skip program name
        args.push_back(argv[i]);
    }
    
    return args;
}

CommandLineOptions CommandLineParser::parseOptions(const std::vector<std::string>& args) {
    CommandLineOptions options;
    
    for (size_t i = 0; i < args.size(); ++i) {
        const std::string& arg = args[i];
        
        if (isOption(arg)) {
            parseOption(arg, options);
        } else {
            options.inputFiles.push_back(arg);
        }
    }
    
    return options;
}

bool CommandLineParser::validateOptions(const CommandLineOptions& options) const {
    return getValidationErrors(options).empty();
}

std::vector<std::string> CommandLineParser::getValidationErrors(const CommandLineOptions& options) const {
    std::vector<std::string> errors;
    
    if (options.inputFiles.empty() && !options.watch) {
        errors.push_back("No input files specified");
    }
    
    if (options.inlineOutput && options.inlineCss) {
        errors.push_back("Cannot use both --inline and --inline-css");
    }
    
    if (options.inlineOutput && options.inlineJs) {
        errors.push_back("Cannot use both --inline and --inline-js");
    }
    
    // Validate file extensions
    for (const auto& file : options.inputFiles) {
        if (!Util::StringUtil::endsWith(file, ".chtl")) {
            errors.push_back("Invalid file extension: " + file + " (expected .chtl)");
        }
    }
    
    return errors;
}

std::string CommandLineParser::generateHelp(const std::string& command) const {
    std::ostringstream oss;
    
    if (command.empty()) {
        oss << generateUsage() << std::endl;
        oss << std::endl;
        oss << "Global options:" << std::endl;
        
        for (const auto& [option, description] : optionDescriptions_) {
            oss << "  " << option;
            if (optionAliases_.find(option.substr(2)) != optionAliases_.end()) {
                oss << ", -" << option.substr(2);
            }
            oss << "    " << description << std::endl;
        }
    } else {
        oss << "Help for command: " << command << std::endl;
        // Command-specific help would be generated here
    }
    
    return oss.str();
}

std::string CommandLineParser::generateUsage() const {
    return "Usage: chtl <command> [options] [files...]";
}

bool CommandLineParser::isOption(const std::string& arg) const {
    return arg.length() > 1 && arg[0] == '-';
}

std::string CommandLineParser::getOptionName(const std::string& arg) const {
    if (arg.length() > 2 && arg.substr(0, 2) == "--") {
        return arg.substr(2);
    } else if (arg.length() > 1 && arg[0] == '-') {
        return arg.substr(1);
    }
    return "";
}

std::string CommandLineParser::getOptionValue(const std::string& arg) const {
    size_t equalPos = arg.find('=');
    if (equalPos != std::string::npos) {
        return arg.substr(equalPos + 1);
    }
    return "";
}

void CommandLineParser::parseOption(const std::string& option, CommandLineOptions& options) const {
    if (option.length() > 2 && option.substr(0, 2) == "--") {
        parseLongOption(option, options);
    } else if (option.length() > 1 && option[0] == '-') {
        parseShortOption(option, options);
    }
}

void CommandLineParser::parseShortOption(const std::string& option, CommandLineOptions& options) const {
    std::string name = option.substr(1);
    
    // Handle aliases
    auto aliasIt = optionAliases_.find(name);
    if (aliasIt != optionAliases_.end()) {
        parseLongOption(aliasIt->second, options);
        return;
    }
    
    // Handle short options
    switch (name[0]) {
        case 'o':
            // Output directory - would need to get value from next argument
            break;
        case 'v':
            options.verbose = true;
            break;
        case 'w':
            options.watch = true;
            break;
        case 'i':
            options.inPlace = true;
            break;
        case 'h':
            // Help - would be handled by main application
            break;
        default:
            // Unknown option
            break;
    }
}

void CommandLineParser::parseLongOption(const std::string& option, CommandLineOptions& options) const {
    std::string name = option.substr(2);
    std::string value = getOptionValue(option);
    
    if (name == "inline") {
        options.inlineOutput = true;
    } else if (name == "inline-css") {
        options.inlineCss = true;
    } else if (name == "inline-js") {
        options.inlineJs = true;
    } else if (name == "default-struct") {
        options.defaultStruct = true;
    } else if (name == "verbose") {
        options.verbose = true;
    } else if (name == "debug") {
        options.debug = true;
    } else if (name == "watch") {
        options.watch = true;
    } else if (name == "in-place") {
        options.inPlace = true;
    } else if (name == "output") {
        if (!value.empty()) {
            options.outputDir = value;
        }
    } else if (name == "indent") {
        if (!value.empty()) {
            options.indentSize = value;
        }
    } else if (name == "include") {
        if (!value.empty()) {
            options.includePaths.push_back(value);
        }
    } else if (name == "exclude") {
        if (!value.empty()) {
            options.excludePaths.push_back(value);
        }
    }
}

} // namespace CLI
} // namespace CHTL