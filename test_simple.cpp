#include <iostream>
#include <string>
#include "CHTL/CHTLLexer/CHTLLexer.h"
#include "CHTL/CHTLLexer/Token.h"
#include "Scanner/UnifiedScanner.h"

using namespace CHTL;

int main() {
    std::cout << "CHTL 简单测试" << std::endl;
    std::cout << "=============" << std::endl;
    
    // 测试词法分析器
    std::string testCode = R"(
html
{
    head
    {
        title
        {
            text
            {
                CHTL测试页面
            }
        }
    }
}
)";
    
    std::cout << "测试代码:" << std::endl;
    std::cout << testCode << std::endl;
    
    // 创建词法分析器
    CHTLLexer lexer;
    lexer.setSkipWhitespace(true);
    lexer.setSkipComments(false);
    
    // 进行词法分析
    std::vector<Token> tokens = lexer.tokenize(testCode);
    
    std::cout << "\n词法分析结果:" << std::endl;
    std::cout << "Token数量: " << tokens.size() << std::endl;
    
    for (size_t i = 0; i < tokens.size() && i < 10; ++i) {
        const Token& token = tokens[i];
        std::cout << "Token " << i << ": " << token.toString() << std::endl;
    }
    
    if (tokens.size() > 10) {
        std::cout << "... 还有 " << (tokens.size() - 10) << " 个Token" << std::endl;
    }
    
    // 测试统一扫描器
    std::cout << "\n测试统一扫描器:" << std::endl;
    Scanner::UnifiedScanner scanner;
    Scanner::ScanResult result = scanner.scan(testCode);
    
    if (result.success) {
        std::cout << "扫描成功!" << std::endl;
        std::cout << "代码片段数量: " << result.fragments.size() << std::endl;
        std::cout << "占位符数量: " << result.placeholders.size() << std::endl;
        
        for (size_t i = 0; i < result.fragments.size(); ++i) {
            const auto& fragment = result.fragments[i];
            std::cout << "片段 " << i << ": 类型=" << static_cast<int>(fragment.type) 
                      << ", 长度=" << fragment.content.length() << std::endl;
        }
    } else {
        std::cout << "扫描失败: " << result.errorMessage << std::endl;
    }
    
    // 测试节点系统
    std::cout << "\n测试节点系统:" << std::endl;
    auto rootNode = NodeFactory::createElement("html");
    auto headNode = NodeFactory::createElement("head");
    auto titleNode = NodeFactory::createElement("title");
    auto textNode = NodeFactory::createText("CHTL测试页面");
    
    rootNode->addChild(headNode);
    headNode->addChild(titleNode);
    titleNode->addChild(textNode);
    
    std::cout << "根节点: " << rootNode->toString() << std::endl;
    std::cout << "子节点数量: " << rootNode->getChildCount() << std::endl;
    std::cout << "节点深度: " << rootNode->getDepth() << std::endl;
    std::cout << "总节点数: " << rootNode->getNodeCount() << std::endl;
    
    std::cout << "\n测试完成!" << std::endl;
    
    return 0;
}