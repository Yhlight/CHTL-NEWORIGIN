#pragma once

#include <string>
#include <memory>
#include <unordered_map>
#include <vector>

namespace CHTL {

class CHTLContext {
public:
    CHTLContext();
    ~CHTLContext();

    // Context management
    void setInputFile(const std::string& filepath);
    void setOutputDirectory(const std::string& dir);
    
    const std::string& getInputFile() const { return inputFile_; }
    const std::string& getOutputDirectory() const { return outputDirectory_; }
    
    // Configuration management
    void setConfiguration(const std::string& key, const std::string& value);
    std::string getConfiguration(const std::string& key) const;
    
    // Module management
    void addModule(const std::string& name, const std::string& path);
    std::string getModulePath(const std::string& name) const;
    
    // Error handling
    void addError(const std::string& message, int line = -1, int column = -1);
    void addWarning(const std::string& message, int line = -1, int column = -1);
    
    const std::vector<std::string>& getErrors() const { return errors_; }
    const std::vector<std::string>& getWarnings() const { return warnings_; }
    
    bool hasErrors() const { return !errors_.empty(); }
    void clearErrors();
    
    // Debug mode
    void setDebugMode(bool enabled) { debugMode_ = enabled; }
    bool isDebugMode() const { return debugMode_; }

private:
    std::string inputFile_;
    std::string outputDirectory_;
    
    std::unordered_map<std::string, std::string> configuration_;
    std::unordered_map<std::string, std::string> modules_;
    
    std::vector<std::string> errors_;
    std::vector<std::string> warnings_;
    
    bool debugMode_;
};

} // namespace CHTL