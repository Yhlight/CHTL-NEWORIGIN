/**
 * @file CHTLContext.h
 * @brief CHTL Compilation Context
 * 
 * Manages the compilation context including configuration,
 * state, and compilation options.
 */

#ifndef CHTL_CONTEXT_H
#define CHTL_CONTEXT_H

#include <string>
#include <memory>
#include <unordered_map>

namespace CHTL {

/**
 * @class CHTLContext
 * @brief Context for CHTL compilation process
 */
class CHTLContext {
public:
    CHTLContext();
    ~CHTLContext();

    // Compilation options
    void setDebugMode(bool debug) { debugMode_ = debug; }
    bool isDebugMode() const { return debugMode_; }

    void setInlineMode(bool inline_) { inlineMode_ = inline_; }
    bool isInlineMode() const { return inlineMode_; }

    void setInlineCss(bool inlineCss) { inlineCss_ = inlineCss; }
    bool isInlineCss() const { return inlineCss_; }

    void setInlineJs(bool inlineJs) { inlineJs_ = inlineJs; }
    bool isInlineJs() const { return inlineJs_; }

    void setDefaultStruct(bool defaultStruct) { defaultStruct_ = defaultStruct; }
    bool isDefaultStruct() const { return defaultStruct_; }

    // File paths
    void setInputFile(const std::string& file) { inputFile_ = file; }
    std::string getInputFile() const { return inputFile_; }

    void setOutputFile(const std::string& file) { outputFile_ = file; }
    std::string getOutputFile() const { return outputFile_; }

    // Configuration
    void setIndexInitialCount(int count) { indexInitialCount_ = count; }
    int getIndexInitialCount() const { return indexInitialCount_; }

    // Namespace management
    void enterNamespace(const std::string& name);
    void exitNamespace();
    std::string getCurrentNamespace() const;

    // Symbol table operations
    void registerSymbol(const std::string& name, const std::string& type);
    bool hasSymbol(const std::string& name) const;

private:
    bool debugMode_;
    bool inlineMode_;
    bool inlineCss_;
    bool inlineJs_;
    bool defaultStruct_;
    
    std::string inputFile_;
    std::string outputFile_;
    
    int indexInitialCount_;
    
    std::string currentNamespace_;
    std::unordered_map<std::string, std::string> symbolTable_;
};

} // namespace CHTL

#endif // CHTL_CONTEXT_H
