#include "CHTL/CHTLContext.h"
#include "CHTL/CHTLLexer.h"
#include "CHTL/CHTLParser.h"
#include "CHTL/CHTLGenerator.h"
#include <fstream>
#include <sstream>
#include <stdexcept>

namespace CHTL {

CHTLContext::CHTLContext() 
    : stateManager_(std::make_unique<CHTLStateManager>())
    , strategyManager_(std::make_unique<CHTLStrategyManager>())
    , isSuccessful_(false) {
}

void CHTLContext::initialize() {
    stateManager_->initialize();
    strategyManager_->initialize();
    isSuccessful_ = false;
    errorMessage_.clear();
    htmlOutput_.clear();
    cssOutput_.clear();
    jsOutput_.clear();
    configuration_.clear();
}

bool CHTLContext::compile(const std::string& inputFile, const std::string& outputDir) {
    try {
        initialize();
        
        // Process the input file
        if (!processFile(inputFile)) {
            return false;
        }
        
        // Generate output
        generateOutput();
        
        isSuccessful_ = true;
        return true;
        
    } catch (const std::exception& e) {
        handleError("Compilation error: " + std::string(e.what()));
        return false;
    }
}

bool CHTLContext::compileString(const std::string& inputString, const std::string& outputDir) {
    try {
        initialize();
        
        // Create lexer and parser
        CHTLLexer lexer;
        lexer.initialize();
        
        CHTLParser parser;
        parser.initialize();
        
        // Tokenize input
        auto tokens = lexer.tokenize(inputString);
        if (tokens.empty()) {
            handleError("No tokens generated from input");
            return false;
        }
        
        // Parse tokens into AST
        auto astRoot = parser.parse(tokens);
        if (!parser.isSuccessful()) {
            handleError("Parsing failed: " + parser.getErrorMessage());
            return false;
        }
        
        // Generate output
        CHTLGenerator generator;
        generator.initialize();
        generator.setOutputOptions(configuration_);
        
        if (!generator.generate(astRoot)) {
            handleError("Generation failed: " + generator.getErrorMessage());
            return false;
        }
        
        // Store outputs
        htmlOutput_ = generator.getHtmlOutput();
        cssOutput_ = generator.getCssOutput();
        jsOutput_ = generator.getJsOutput();
        
        isSuccessful_ = true;
        return true;
        
    } catch (const std::exception& e) {
        handleError("Compilation error: " + std::string(e.what()));
        return false;
    }
}

std::string CHTLContext::getResult() const {
    return htmlOutput_ + "\n" + cssOutput_ + "\n" + jsOutput_;
}

std::string CHTLContext::getHtmlOutput() const {
    return htmlOutput_;
}

std::string CHTLContext::getCssOutput() const {
    return cssOutput_;
}

std::string CHTLContext::getJsOutput() const {
    return jsOutput_;
}

bool CHTLContext::isSuccessful() const {
    return isSuccessful_;
}

std::string CHTLContext::getErrorMessage() const {
    return errorMessage_;
}

void CHTLContext::reset() {
    initialize();
}

void CHTLContext::setConfiguration(const std::string& key, const std::string& value) {
    configuration_[key] = value;
}

std::string CHTLContext::getConfiguration(const std::string& key) const {
    auto it = configuration_.find(key);
    if (it != configuration_.end()) {
        return it->second;
    }
    return "";
}

bool CHTLContext::processFile(const std::string& filePath) {
    try {
        std::ifstream file(filePath);
        if (!file.is_open()) {
            handleError("Cannot open file: " + filePath);
            return false;
        }
        
        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string content = buffer.str();
        
        return compileString(content);
        
    } catch (const std::exception& e) {
        handleError("File processing error: " + std::string(e.what()));
        return false;
    }
}

bool CHTLContext::processTokens(const std::vector<std::string>& tokens) {
    for (const auto& token : tokens) {
        if (!stateManager_->processToken(token)) {
            handleError("Token processing failed: " + stateManager_->getErrorMessage());
            return false;
        }
        
        if (!strategyManager_->process(token)) {
            handleError("Strategy processing failed for token: " + token);
            return false;
        }
    }
    
    return true;
}

void CHTLContext::generateOutput() {
    // This method would coordinate the generation process
    // For now, it's a placeholder
}

void CHTLContext::handleError(const std::string& error) {
    errorMessage_ = error;
    isSuccessful_ = false;
}

} // namespace CHTL