#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <fstream>
#include <sstream>
#include "CHTL/CHTLLexer/CHTLLexer.h"
#include "CHTL/CHTLLexer/Token.h"
#include "CHTL/CHTLParser/CHTLParser.h"
#include "CHTL/CHTLGenerator/CHTLGenerator.h"
#include "Scanner/UnifiedScanner.h"
#include "CHTL/CHTLNode/BaseNode.h"

using namespace CHTL;

void printAST(std::shared_ptr<BaseNode> node, int depth = 0) {
    if (!node) return;
    
    std::string indent(depth * 2, ' ');
    std::cout << indent << "Node: " << static_cast<int>(node->getType()) 
              << " Value: '" << node->getValue() << "'" 
              << " Name: '" << node->getName() << "'" << std::endl;
    
    auto attributes = node->getAttributes();
    if (!attributes.empty()) {
        std::cout << indent << "  Attributes:" << std::endl;
        for (const auto& attr : attributes) {
            std::cout << indent << "    " << attr.first << " = " << attr.second << std::endl;
        }
    }
    
    auto children = node->getChildren();
    if (!children.empty()) {
        std::cout << indent << "  Children (" << children.size() << "):" << std::endl;
        for (const auto& child : children) {
            printAST(child, depth + 1);
        }
    }
}

int main() {
    std::string inputFile = "test_simple.chtl";
    
    // 读取输入文件
    std::ifstream file(inputFile);
    if (!file.is_open()) {
        std::cerr << "Error: Cannot open input file " << inputFile << std::endl;
        return 1;
    }
    
    std::string content((std::istreambuf_iterator<char>(file)),
                       std::istreambuf_iterator<char>());
    file.close();
    
    std::cout << "Input file size: " << content.length() << " bytes" << std::endl;
    
    // 使用统一扫描器扫描代码
    Scanner::UnifiedScanner scanner;
    Scanner::ScanResult scanResult = scanner.scan(content);
    
    if (!scanResult.success) {
        std::cerr << "Error: " << scanResult.errorMessage << std::endl;
        return 1;
    }
    
    std::cout << "Scan completed successfully" << std::endl;
    std::cout << "Fragments found: " << scanResult.fragments.size() << std::endl;
    
    // 使用CHTL词法分析器分析CHTL片段
    CHTLLexer lexer;
    std::vector<Token> allTokens;
    
    for (const auto& fragment : scanResult.fragments) {
        if (fragment.type == Scanner::FragmentType::CHTL) {
            std::vector<Token> tokens = lexer.tokenize(fragment.content);
            allTokens.insert(allTokens.end(), tokens.begin(), tokens.end());
        }
    }
    
    std::cout << "Tokens generated: " << allTokens.size() << std::endl;
    
    // 输出tokens
    for (size_t i = 0; i < allTokens.size(); ++i) {
        std::cout << "Token " << i << ": " << static_cast<int>(allTokens[i].type) 
                  << " '" << allTokens[i].value << "'" << std::endl;
    }
    
    // 使用CHTL解析器解析Token为AST
    CHTLParser parser;
    parser.setTokens(allTokens);
    auto ast = parser.parse();
    
    if (!ast) {
        std::cerr << "Error: Failed to parse CHTL code" << std::endl;
        const auto& errors = parser.getErrors();
        for (const auto& error : errors) {
            std::cerr << "Parse error: " << error << std::endl;
        }
        return 1;
    }
    
    std::cout << "AST generated successfully" << std::endl;
    printAST(ast);
    
    return 0;
}