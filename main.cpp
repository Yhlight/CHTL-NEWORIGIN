#include <iostream>
#include <string>
#include <vector>
#include "CompilerDispatcher/CompilerDispatcher.h"
#include "CHTL/CHTLContext.h"
#include "CHTL JS/CHTLJSContext.h"

int main(int argc, char* argv[]) {
    std::cout << "CHTL Compiler v1.0.0" << std::endl;
    std::cout << "基于C++实现的超文本语言编译器" << std::endl;
    
    if (argc < 2) {
        std::cout << "用法: chtl <输入文件> [选项]" << std::endl;
        std::cout << "选项:" << std::endl;
        std::cout << "  --output <目录>    指定输出目录" << std::endl;
        std::cout << "  --inline          生成内联HTML文件" << std::endl;
        std::cout << "  --inline-css      内联CSS" << std::endl;
        std::cout << "  --inline-js       内联JavaScript" << std::endl;
        std::cout << "  --default-struct  生成默认HTML结构" << std::endl;
        return 1;
    }
    
    std::string inputFile = argv[1];
    std::string outputDir = ".";
    bool inlineMode = false;
    bool inlineCSS = false;
    bool inlineJS = false;
    bool defaultStruct = false;
    
    // 解析命令行参数
    for (int i = 2; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "--output" && i + 1 < argc) {
            outputDir = argv[++i];
        } else if (arg == "--inline") {
            inlineMode = true;
        } else if (arg == "--inline-css") {
            inlineCSS = true;
        } else if (arg == "--inline-js") {
            inlineJS = true;
        } else if (arg == "--default-struct") {
            defaultStruct = true;
        }
    }
    
    try {
        // 创建编译器调度器
        CompilerDispatcher dispatcher;
        
        // 设置编译选项
        dispatcher.setOutputDirectory(outputDir);
        dispatcher.setInlineMode(inlineMode);
        dispatcher.setInlineCSS(inlineCSS);
        dispatcher.setInlineJS(inlineJS);
        dispatcher.setDefaultStruct(defaultStruct);
        
        // 执行编译
        bool success = dispatcher.compile(inputFile);
        
        if (success) {
            std::cout << "编译成功！输出目录: " << outputDir << std::endl;
            return 0;
        } else {
            std::cerr << "编译失败！" << std::endl;
            return 1;
        }
    } catch (const std::exception& e) {
        std::cerr << "编译错误: " << e.what() << std::endl;
        return 1;
    }
}