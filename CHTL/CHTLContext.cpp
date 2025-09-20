#include "CHTLContext.h"
#include <iostream>
#include <sstream>

CHTLContext::CHTLContext() 
    : debugMode_(false)
    , outputFormat_("html") {
    lexer_ = std::make_unique<CHTLLexer>();
    parser_ = std::make_unique<CHTLParser>();
    generator_ = std::make_unique<CHTLGenerator>();
}

bool CHTLContext::compile(const std::string& input, std::string& output) {
    std::cout << "开始编译CHTL代码..." << std::endl;
    
    try {
        // 1. 词法分析
        std::vector<Token> tokens;
        if (!tokenize(input, tokens)) {
            return false;
        }
        
        if (debugMode_) {
            std::cout << "词法分析完成，共 " << tokens.size() << " 个token" << std::endl;
        }
        
        // 2. 语法分析
        std::unique_ptr<CHTLNode> ast;
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
        std::cerr << "CHTL编译过程中发生错误: " << e.what() << std::endl;
        return false;
    }
}

bool CHTLContext::tokenize(const std::string& input, std::vector<Token>& tokens) {
    return lexer_->tokenize(input, tokens);
}

bool CHTLContext::parse(const std::vector<Token>& tokens, std::unique_ptr<CHTLNode>& ast) {
    return parser_->parse(tokens, ast);
}

bool CHTLContext::generate(const std::unique_ptr<CHTLNode>& ast, std::string& output) {
    return generator_->generate(ast, output);
}

void CHTLContext::reportError(const std::string& message, size_t line, size_t column) {
    std::stringstream ss;
    ss << "错误 [" << line << ":" << column << "]: " << message;
    errors_.push_back(ss.str());
    std::cerr << ss.str() << std::endl;
}

void CHTLContext::reportWarning(const std::string& message, size_t line, size_t column) {
    std::stringstream ss;
    ss << "警告 [" << line << ":" << column << "]: " << message;
    warnings_.push_back(ss.str());
    std::cout << ss.str() << std::endl;
}