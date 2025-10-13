#include <iostream>
#include <string>
#include "../include/CHTL/Compiler.hpp"

using namespace CHTL;

void printUsage(const char* programName) {
    std::cout << "CHTL Compiler v1.0.0\n\n";
    std::cout << "Usage: " << programName << " [options] <input.chtl>\n\n";
    std::cout << "Options:\n";
    std::cout << "  --inline         生成内联的HTML文件（包含CSS和JS）\n";
    std::cout << "  --inline-css     内联CSS\n";
    std::cout << "  --inline-js      内联JS\n";
    std::cout << "  --default-struct 生成带有默认HTML结构的文件\n";
    std::cout << "  --debug          启用调试模式\n";
    std::cout << "  -o <dir>         指定输出目录（默认：当前目录）\n";
    std::cout << "  --output <name>  指定输出文件名（默认：output）\n";
    std::cout << "  --help           显示此帮助信息\n";
    std::cout << "\n";
    std::cout << "示例:\n";
    std::cout << "  " << programName << " example.chtl\n";
    std::cout << "  " << programName << " --inline --default-struct example.chtl\n";
    std::cout << "  " << programName << " -o build example.chtl\n";
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printUsage(argv[0]);
        return 1;
    }
    
    CompilerOptions options;
    String inputFile;
    
    // 解析命令行参数
    for (int i = 1; i < argc; ++i) {
        String arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            printUsage(argv[0]);
            return 0;
        }
        else if (arg == "--inline") {
            options.inlineCSS = true;
            options.inlineJS = true;
        }
        else if (arg == "--inline-css") {
            options.inlineCSS = true;
        }
        else if (arg == "--inline-js") {
            options.inlineJS = true;
        }
        else if (arg == "--default-struct") {
            options.defaultStruct = true;
        }
        else if (arg == "--debug") {
            options.debugMode = true;
        }
        else if (arg == "-o" && i + 1 < argc) {
            options.outputDir = argv[++i];
        }
        else if (arg == "--output" && i + 1 < argc) {
            options.outputName = argv[++i];
        }
        else if (!arg.empty() && arg[0] != '-') {
            inputFile = arg;
        }
        else {
            std::cerr << "未知选项: " << arg << "\n";
            printUsage(argv[0]);
            return 1;
        }
    }
    
    if (inputFile.empty()) {
        std::cerr << "错误: 未指定输入文件\n";
        printUsage(argv[0]);
        return 1;
    }
    
    // 编译
    Compiler compiler(options);
    
    std::cout << "正在编译: " << inputFile << "\n";
    
    CompileResult result = compiler.compileFile(inputFile);
    
    // 输出结果
    if (result.success) {
        std::cout << "编译成功!\n";
        std::cout << "输出文件: " << options.outputDir.string() << "/" 
                  << options.outputName << ".html\n";
        
        if (!result.warnings.empty()) {
            std::cout << "\n警告:\n";
            for (const auto& warning : result.warnings) {
                std::cout << "  " << warning << "\n";
            }
        }
    } else {
        std::cerr << "编译失败:\n";
        for (const auto& error : result.errors) {
            std::cerr << "  " << error << "\n";
        }
        return 1;
    }
    
    return 0;
}
