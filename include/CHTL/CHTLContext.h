#pragma once

#include <string>
#include <memory>
#include <unordered_map>
#include <vector>
#include <fstream>
#include "CHTLState.h"
#include "CHTLStrategy.h"

namespace CHTL {

/**
 * @brief Main context class that coordinates state machine and strategy pattern
 * 
 * This is the central orchestrator that manages the entire CHTL compilation process
 * using state machine for parsing flow and strategy pattern for different parsing approaches
 */
class CHTLContext {
public:
    CHTLContext();
    ~CHTLContext() = default;
    
    /**
     * @brief Initialize CHTL context
     */
    void initialize();
    
    /**
     * @brief Compile CHTL file to HTML/CSS/JS
     * @param inputFile Input CHTL file path
     * @param outputDir Output directory for generated files
     * @return True if successful, false if error
     */
    bool compile(const std::string& inputFile, const std::string& outputDir = ".");
    
    /**
     * @brief Compile CHTL string to HTML/CSS/JS
     * @param inputString Input CHTL string
     * @param outputDir Output directory for generated files
     * @return True if successful, false if error
     */
    bool compileString(const std::string& inputString, const std::string& outputDir = ".");
    
    /**
     * @brief Get compilation result
     * @return Compilation result
     */
    std::string getResult() const;
    
    /**
     * @brief Get HTML output
     * @return HTML output
     */
    std::string getHtmlOutput() const;
    
    /**
     * @brief Get CSS output
     * @return CSS output
     */
    std::string getCssOutput() const;
    
    /**
     * @brief Get JavaScript output
     * @return JavaScript output
     */
    std::string getJsOutput() const;
    
    /**
     * @brief Check if compilation was successful
     * @return True if successful, false if error
     */
    bool isSuccessful() const;
    
    /**
     * @brief Get error message
     * @return Error message
     */
    std::string getErrorMessage() const;
    
    /**
     * @brief Reset context state
     */
    void reset();
    
    /**
     * @brief Set configuration options
     * @param key Configuration key
     * @param value Configuration value
     */
    void setConfiguration(const std::string& key, const std::string& value);
    
    /**
     * @brief Get configuration value
     * @param key Configuration key
     * @return Configuration value
     */
    std::string getConfiguration(const std::string& key) const;

private:
    // State machine and strategy pattern components
    std::unique_ptr<CHTLStateManager> stateManager_;
    std::unique_ptr<CHTLStrategyManager> strategyManager_;
    
    // Compilation state
    bool isSuccessful_;
    std::string errorMessage_;
    std::string htmlOutput_;
    std::string cssOutput_;
    std::string jsOutput_;
    
    // Configuration
    std::unordered_map<std::string, std::string> configuration_;
    
    // Internal methods
    bool processFile(const std::string& filePath);
    bool processTokens(const std::vector<std::string>& tokens);
    void generateOutput();
    void handleError(const std::string& error);
};

} // namespace CHTL