#include <iostream>
#include "Common.h"
#include "CHTL/CHTLLexer/Lexer.h"
#include "Util/FileSystem/FileSystem.h"

using namespace CHTL;

void printUsage() {
    std::cout << "CHTL Compiler v1.0.0\n";
    std::cout << "Usage: chtl [options] <input-file>\n";
    std::cout << "Options:\n";
    std::cout << "  -h, --help        Show this help message\n";
    std::cout << "  -v, --version     Show version information\n";
    std::cout << "  -o <file>         Specify output file\n";
    std::cout << "  --tokens          Print tokens and exit\n";
    std::cout << "  --ast             Print AST and exit\n";
}

void printVersion() {
    std::cout << "CHTL Compiler v1.0.0\n";
    std::cout << "Built with C++17\n";
}

void printTokens(const Vector<Token>& tokens) {
    std::cout << "Tokens:\n";
    std::cout << "-------\n";
    for (const auto& token : tokens) {
        std::cout << token.toString() << "\n";
    }
}

int main(int argc, char** argv) {
    try {
        if (argc < 2) {
            printUsage();
            return 1;
        }
        
        String inputFile;
        String outputFile;
        bool showTokens = false;
        bool showAst = false;
        
        // 解析命令行参数
        for (int i = 1; i < argc; ++i) {
            String arg = argv[i];
            
            if (arg == "-h" || arg == "--help") {
                printUsage();
                return 0;
            } else if (arg == "-v" || arg == "--version") {
                printVersion();
                return 0;
            } else if (arg == "--tokens") {
                showTokens = true;
            } else if (arg == "--ast") {
                showAst = true;
            } else if (arg == "-o") {
                if (i + 1 < argc) {
                    outputFile = argv[++i];
                } else {
                    std::cerr << "Error: -o requires an argument\n";
                    return 1;
                }
            } else if (arg[0] != '-') {
                inputFile = arg;
            } else {
                std::cerr << "Unknown option: " << arg << "\n";
                return 1;
            }
        }
        
        if (inputFile.empty()) {
            std::cerr << "Error: No input file specified\n";
            printUsage();
            return 1;
        }
        
        // 读取源文件
        if (!Util::FileSystem::exists(inputFile)) {
            std::cerr << "Error: File not found: " << inputFile << "\n";
            return 1;
        }
        
        String source = Util::FileSystem::readFile(inputFile);
        
        // 词法分析
        Lexer lexer(source);
        Vector<Token> tokens = lexer.tokenize();
        
        if (showTokens) {
            printTokens(tokens);
            return 0;
        }
        
        // TODO: 语法分析
        // TODO: 代码生成
        
        std::cout << "Compilation successful!\n";
        std::cout << "Input: " << inputFile << "\n";
        if (!outputFile.empty()) {
            std::cout << "Output: " << outputFile << "\n";
        }
        
        return 0;
        
    } catch (const CompileError& e) {
        std::cerr << "Compile Error: " << e.what() << "\n";
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
}
