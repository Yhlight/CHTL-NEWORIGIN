#include <iostream>
#include <fstream>
#include <string>
#include "../CHTL/CHTLContext.h"
#include "../CHTL JS/CHTLJSContext.h"
#include "../Scanner/UnifiedScanner.h"
#include "../CodeMerger/CodeMerger.h"

void testCHTLLexer() {
    std::cout << "=== 测试CHTL词法分析器 ===" << std::endl;
    
    CHTLLexer lexer;
    lexer.setDebugMode(true);
    
    std::string input = R"(
html
{
    head
    {
        title: "测试页面";
    }
    
    body
    {
        div
        {
            text: "Hello, CHTL!";
        }
    }
}
)";
    
    std::vector<Token> tokens;
    if (lexer.tokenize(input, tokens)) {
        std::cout << "词法分析成功，共 " << tokens.size() << " 个token" << std::endl;
        
        for (const auto& token : tokens) {
            std::cout << "Token: " << static_cast<int>(token.type) 
                      << " = \"" << token.value << "\" [" 
                      << token.line << ":" << token.column << "]" << std::endl;
        }
    } else {
        std::cout << "词法分析失败" << std::endl;
    }
}

void testCHTLParser() {
    std::cout << "\n=== 测试CHTL语法分析器 ===" << std::endl;
    
    CHTLLexer lexer;
    CHTLParser parser;
    parser.setDebugMode(true);
    
    std::string input = R"(
html
{
    head
    {
        title: "测试页面";
    }
    
    body
    {
        div
        {
            text: "Hello, CHTL!";
        }
    }
}
)";
    
    std::vector<Token> tokens;
    if (lexer.tokenize(input, tokens)) {
        std::unique_ptr<CHTLNode> ast;
        if (parser.parse(tokens, ast)) {
            std::cout << "语法分析成功" << std::endl;
            std::cout << "AST: " << ast->toDebugString() << std::endl;
        } else {
            std::cout << "语法分析失败" << std::endl;
        }
    }
}

void testCHTLGenerator() {
    std::cout << "\n=== 测试CHTL代码生成器 ===" << std::endl;
    
    CHTLLexer lexer;
    CHTLParser parser;
    CHTLGenerator generator;
    generator.setDebugMode(true);
    
    std::string input = R"(
html
{
    head
    {
        title: "测试页面";
    }
    
    body
    {
        div
        {
            text: "Hello, CHTL!";
        }
    }
}
)";
    
    std::vector<Token> tokens;
    if (lexer.tokenize(input, tokens)) {
        std::unique_ptr<CHTLNode> ast;
        if (parser.parse(tokens, ast)) {
            std::string output;
            if (generator.generate(ast, output)) {
                std::cout << "代码生成成功" << std::endl;
                std::cout << "生成的HTML:" << std::endl;
                std::cout << output << std::endl;
            } else {
                std::cout << "代码生成失败" << std::endl;
            }
        }
    }
}

void testCHTLJS() {
    std::cout << "\n=== 测试CHTL JS ===" << std::endl;
    
    CHTLJSLexer lexer;
    CHTLJSParser parser;
    CHTLJSGenerator generator;
    
    std::string input = R"(
Listen
{
    click: function() {
        console.log('Button clicked!');
    }
}

Animate
{
    target: {{button}};
    duration: 1000;
    easing: ease-in-out;
}
)";
    
    std::vector<CHTLJSToken> tokens;
    if (lexer.tokenize(input, tokens)) {
        std::unique_ptr<CHTLJSNode> ast;
        if (parser.parse(tokens, ast)) {
            std::string output;
            if (generator.generate(ast, output)) {
                std::cout << "CHTL JS代码生成成功" << std::endl;
                std::cout << "生成的JavaScript:" << std::endl;
                std::cout << output << std::endl;
            }
        }
    }
}

void testUnifiedScanner() {
    std::cout << "\n=== 测试统一扫描器 ===" << std::endl;
    
    UnifiedScanner scanner;
    
    std::string input = R"(
html
{
    head
    {
        title: "测试页面";
    }
    
    body
    {
        div
        {
            text: "Hello, CHTL!";
        }
        
        style
        {
            .container
            {
                width: 100%;
                padding: 20px;
            }
        }
        
        script
        {
            {{div}}->addEventListener('click', function() {
                console.log('Div clicked!');
            });
        }
    }
}
)";
    
    CodeSegments segments;
    if (scanner.scan(input, segments)) {
        std::cout << "代码扫描成功" << std::endl;
        std::cout << "CHTL片段数量: " << segments.chtlSegments.size() << std::endl;
        std::cout << "CHTL JS片段数量: " << segments.chtlJSSegments.size() << std::endl;
        std::cout << "CSS片段数量: " << segments.cssSegments.size() << std::endl;
        std::cout << "JS片段数量: " << segments.jsSegments.size() << std::endl;
    } else {
        std::cout << "代码扫描失败" << std::endl;
    }
}

int main() {
    std::cout << "CHTL编译器测试程序" << std::endl;
    std::cout << "==================" << std::endl;
    
    testCHTLLexer();
    testCHTLParser();
    testCHTLGenerator();
    testCHTLJS();
    testUnifiedScanner();
    
    std::cout << "\n测试完成！" << std::endl;
    return 0;
}