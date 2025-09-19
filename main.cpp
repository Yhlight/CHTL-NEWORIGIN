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

int main(int argc, char* argv[]) {
    std::cout << "CHTL Compiler v1.0.0" << std::endl;
    std::cout << "====================" << std::endl;
    
    if (argc < 2) {
        std::cout << "Usage: chtl_compiler <input_file> [options]" << std::endl;
        std::cout << "Options:" << std::endl;
        std::cout << "  --help, -h     Show this help message" << std::endl;
        std::cout << "  --version, -v  Show version information" << std::endl;
        std::cout << "  --output, -o   Specify output file" << std::endl;
        std::cout << "  --inline       Generate inline HTML" << std::endl;
        std::cout << "  --debug        Enable debug mode" << std::endl;
        return 1;
    }
    
    std::string inputFile = argv[1];
    std::string outputFile;
    bool inlineMode = false;
    bool debugMode = false;
    
    // 解析命令行参数
    for (int i = 2; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--help" || arg == "-h") {
            std::cout << "CHTL Compiler - A C++ based hypertext language compiler" << std::endl;
            std::cout << "Usage: chtl_compiler <input_file> [options]" << std::endl;
            std::cout << "Options:" << std::endl;
            std::cout << "  --help, -h     Show this help message" << std::endl;
            std::cout << "  --version, -v  Show version information" << std::endl;
            std::cout << "  --output, -o   Specify output file" << std::endl;
            std::cout << "  --inline       Generate inline HTML" << std::endl;
            std::cout << "  --debug        Enable debug mode" << std::endl;
            return 0;
        } else if (arg == "--version" || arg == "-v") {
            std::cout << "CHTL Compiler v1.0.0" << std::endl;
            std::cout << "Built with C++17" << std::endl;
            return 0;
        } else if (arg == "--output" || arg == "-o") {
            if (i + 1 < argc) {
                outputFile = argv[++i];
            } else {
                std::cerr << "Error: --output requires a filename" << std::endl;
                return 1;
            }
        } else if (arg == "--inline") {
            inlineMode = true;
        } else if (arg == "--debug") {
            debugMode = true;
        } else {
            std::cerr << "Error: Unknown option " << arg << std::endl;
            return 1;
        }
    }
    
    try {
        // 读取输入文件
        std::ifstream file(inputFile);
        if (!file.is_open()) {
            std::cerr << "Error: Cannot open input file " << inputFile << std::endl;
            return 1;
        }
        
        std::string content((std::istreambuf_iterator<char>(file)),
                           std::istreambuf_iterator<char>());
        file.close();
        
        if (debugMode) {
            std::cout << "Input file size: " << content.length() << " bytes" << std::endl;
        }
        
        // 使用统一扫描器扫描代码
        Scanner::UnifiedScanner scanner;
        Scanner::ScanResult scanResult = scanner.scan(content);
        
        if (!scanResult.success) {
            std::cerr << "Error: " << scanResult.errorMessage << std::endl;
            return 1;
        }
        
        if (debugMode) {
            std::cout << "Scan completed successfully" << std::endl;
            std::cout << "Fragments found: " << scanResult.fragments.size() << std::endl;
            std::cout << "Placeholders created: " << scanResult.placeholders.size() << std::endl;
        }
        
        // 使用CHTL词法分析器分析CHTL片段
        CHTLLexer lexer;
        std::vector<Token> allTokens;
        
        for (const auto& fragment : scanResult.fragments) {
            if (fragment.type == Scanner::FragmentType::CHTL) {
                std::vector<Token> tokens = lexer.tokenize(fragment.content);
                allTokens.insert(allTokens.end(), tokens.begin(), tokens.end());
            }
        }
        
        if (debugMode) {
            std::cout << "Tokens generated: " << allTokens.size() << std::endl;
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
        
        if (debugMode) {
            std::cout << "AST generated successfully" << std::endl;
        }
        
        // 使用CHTL生成器生成HTML
        CHTLGenerator generator;
        generator.setInlineCSS(inlineMode);
        generator.setInlineJS(inlineMode);
        generator.setDefaultStructure(true);
        
        std::string htmlOutput = generator.generatePage(ast);
        
        // 生成输出文件名
        if (outputFile.empty()) {
            size_t dotPos = inputFile.find_last_of('.');
            if (dotPos != std::string::npos) {
                outputFile = inputFile.substr(0, dotPos) + ".html";
            } else {
                outputFile = inputFile + ".html";
            }
        }
        
        // 写入输出文件
        std::ofstream outFile(outputFile);
        if (!outFile.is_open()) {
            std::cerr << "Error: Cannot create output file " << outputFile << std::endl;
            return 1;
        }
        
        outFile << htmlOutput;
        outFile.close();
        
        std::cout << "Compilation completed successfully!" << std::endl;
        std::cout << "Output written to: " << outputFile << std::endl;
        
        if (debugMode) {
            std::cout << "Output size: " << htmlOutput.length() << " bytes" << std::endl;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
