#include "CHTLJSContext.h"
#include <iostream>
#include <sstream>

CHTLJSContext::CHTLJSContext() 
    : debugMode_(false)
    , outputFormat_("javascript") {
    lexer_ = std::make_unique<CHTLJSLexer>();
    parser_ = std::make_unique<CHTLJSParser>();
    generator_ = std::make_unique<CHTLJSGenerator>();
}

bool CHTLJSContext::compile(const std::string& input, std::string& output) {
    std::cout << "开始编译CHTL JS代码..." << std::endl;
    
    try {
        // 1. 词法分析
        std::vector<CHTLJSToken> tokens;
        if (!tokenize(input, tokens)) {
            return false;
        }
        
        if (debugMode_) {
            std::cout << "词法分析完成，共 " << tokens.size() << " 个token" << std::endl;
        }
        
        // 2. 语法分析
        std::unique_ptr<CHTLJSNode> ast;
        if (!parse(tokens, ast)) {
            return false;
        }
        
        if (debugMode_) {
            std::cout << "语法分析完成，AST构建成功" << std::endl;
        }
        
        // 3. 代码生成
        if (!generate(ast, output)) {
            return false;
        }
        
        if (debugMode_) {
            std::cout << "代码生成完成" << std::endl;
        }
        
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "CHTL JS编译过程中发生错误: " << e.what() << std::endl;
        return false;
    }
}

bool CHTLJSContext::tokenize(const std::string& input, std::vector<CHTLJSToken>& tokens) {
    return lexer_->tokenize(input, tokens);
}

bool CHTLJSContext::parse(const std::vector<CHTLJSToken>& tokens, std::unique_ptr<CHTLJSNode>& ast) {
    return parser_->parse(tokens, ast);
}

bool CHTLJSContext::generate(const std::unique_ptr<CHTLJSNode>& ast, std::string& output) {
    return generator_->generate(ast, output);
}

void CHTLJSContext::reportError(const std::string& message, size_t line, size_t column) {
    std::stringstream ss;
    ss << "错误 [" << line << ":" << column << "]: " << message;
    errors_.push_back(ss.str());
    std::cerr << ss.str() << std::endl;
}

void CHTLJSContext::reportWarning(const std::string& message, size_t line, size_t column) {
    std::stringstream ss;
    ss << "警告 [" << line << ":" << column << "]: " << message;
    warnings_.push_back(ss.str());
    std::cout << ss.str() << std::endl;
}