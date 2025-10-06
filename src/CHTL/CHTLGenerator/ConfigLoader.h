#ifndef CHTL_CONFIG_LOADER_H
#define CHTL_CONFIG_LOADER_H

#include "../../Common.h"
#include <unordered_map>
#include <fstream>

namespace CHTL {

struct CHTLConfig {
    // Compiler options
    bool prettyPrint = false;
    bool includeComments = false;
    bool generateSourceMap = false;
    bool minify = false;
    
    // Output options
    String outputDir = "./dist";
    String publicPath = "/";
    
    // Development options
    bool watch = false;
    bool hotReload = false;
    int port = 3000;
    
    // Advanced options
    bool enableCache = true;
    bool parallelCompile = false;
    int maxWorkers = 4;
    
    // Plugin paths
    Vector<String> plugins;
    
    // Custom origins
    HashMap<String, String> customOrigins;
};

class ConfigLoader {
public:
    static CHTLConfig load(const String& configPath = ".chtlrc") {
        CHTLConfig config;
        
        // Try to read config file
        std::ifstream file(configPath);
        if (!file.is_open()) {
            // Return default config
            return config;
        }
        
        // Simple JSON-like parsing (placeholder)
        // In production, use a proper JSON library
        String line;
        while (std::getline(file, line)) {
            parseLine(line, config);
        }
        
        return config;
    }
    
    static void save(const CHTLConfig& config, const String& configPath = ".chtlrc") {
        std::ofstream file(configPath);
        if (!file.is_open()) {
            return;
        }
        
        file << "{\n";
        file << "  \"prettyPrint\": " << (config.prettyPrint ? "true" : "false") << ",\n";
        file << "  \"includeComments\": " << (config.includeComments ? "true" : "false") << ",\n";
        file << "  \"generateSourceMap\": " << (config.generateSourceMap ? "true" : "false") << ",\n";
        file << "  \"minify\": " << (config.minify ? "true" : "false") << ",\n";
        file << "  \"outputDir\": \"" << config.outputDir << "\",\n";
        file << "  \"publicPath\": \"" << config.publicPath << "\",\n";
        file << "  \"watch\": " << (config.watch ? "true" : "false") << ",\n";
        file << "  \"hotReload\": " << (config.hotReload ? "true" : "false") << ",\n";
        file << "  \"port\": " << config.port << "\n";
        file << "}\n";
    }
    
private:
    static void parseLine(const String& line, CHTLConfig& config) {
        // Simple key-value parser (placeholder)
        size_t colonPos = line.find(':');
        if (colonPos == String::npos) return;
        
        String key = trim(line.substr(0, colonPos));
        String value = trim(line.substr(colonPos + 1));
        
        // Remove quotes and commas
        key = removeQuotes(key);
        value = removeQuotes(value);
        value = value.substr(0, value.find(','));
        
        // Parse values
        if (key == "prettyPrint") config.prettyPrint = (value == "true");
        else if (key == "includeComments") config.includeComments = (value == "true");
        else if (key == "generateSourceMap") config.generateSourceMap = (value == "true");
        else if (key == "minify") config.minify = (value == "true");
        else if (key == "outputDir") config.outputDir = value;
        else if (key == "publicPath") config.publicPath = value;
        else if (key == "watch") config.watch = (value == "true");
        else if (key == "hotReload") config.hotReload = (value == "true");
        else if (key == "port") config.port = std::stoi(value);
    }
    
    static String trim(const String& str) {
        size_t first = str.find_first_not_of(" \t\n\r");
        if (first == String::npos) return "";
        size_t last = str.find_last_not_of(" \t\n\r");
        return str.substr(first, last - first + 1);
    }
    
    static String removeQuotes(const String& str) {
        if (str.length() >= 2 && str[0] == '"' && str[str.length()-1] == '"') {
            return str.substr(1, str.length() - 2);
        }
        return str;
    }
};

} // namespace CHTL

#endif // CHTL_CONFIG_LOADER_H
