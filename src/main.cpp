#include <iostream>
#include <sstream>
#include "Common.h"
#include "CHTL/CHTLLexer/Lexer.h"
#include "CHTL/CHTLParser/CHTLParser.h"
#include "CHTL/CHTLGenerator/CHTLGenerator.h"
#include "Util/FileSystem/FileSystem.h"

using namespace CHTL;

void printUsage() {
    std::cout << "CHTL Compiler v2.6.1\n";
    std::cout << "Usage: chtl [options] <input-file>\n\n";
    std::cout << "Options:\n";
    std::cout << "  -h, --help          Show this help message\n";
    std::cout << "  -v, --version       Show version information\n";
    std::cout << "  -o <file>           Specify output file (default: <input>.html)\n\n";
    
    std::cout << "Output Options:\n";
    std::cout << "  --inline            Inline all CSS and JS into HTML\n";
    std::cout << "  --inline-css        Inline only CSS into HTML\n";
    std::cout << "  --inline-js         Inline only JS into HTML\n";
    std::cout << "  --default-struct    Generate HTML with default structure\n";
    std::cout << "  --separate          Output separate HTML, CSS, JS files (default)\n\n";
    
    std::cout << "Debug Options:\n";
    std::cout << "  --tokens            Print tokens and exit\n";
    std::cout << "  --ast               Print AST and exit\n\n";
    
    std::cout << "Examples:\n";
    std::cout << "  chtl input.chtl                  # Compile to input.html\n";
    std::cout << "  chtl input.chtl -o output.html   # Compile to output.html\n";
    std::cout << "  chtl input.chtl --inline         # Inline all resources\n";
    std::cout << "  chtl input.chtl --default-struct # With <!DOCTYPE html><html>...\n";
}

void printVersion() {
    std::cout << "CHTL Compiler v2.6.1-vscode\n";
    std::cout << "Built with C++17\n";
    std::cout << "License: MIT\n";
    std::cout << "Features: Conditional Rendering, CLI Tools, VSCode Extension\n";
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
        bool inlineAll = false;
        bool inlineCss = false;
        bool inlineJs = false;
        bool defaultStruct = false;
        bool separateFiles = false;
        
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
            } else if (arg == "--inline") {
                inlineAll = true;
            } else if (arg == "--inline-css") {
                inlineCss = true;
            } else if (arg == "--inline-js") {
                inlineJs = true;
            } else if (arg == "--default-struct") {
                defaultStruct = true;
            } else if (arg == "--separate") {
                separateFiles = true;
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
        
        // 语法分析
        CHTLParser parser(tokens);
        auto ast = parser.parse();
        
        if (parser.hasErrors()) {
            std::cerr << "Parse errors:\n";
            for (const auto& error : parser.getErrors()) {
                std::cerr << "  " << error << "\n";
            }
            return 1;
        }
        
        if (showAst) {
            std::cout << "AST:\n";
            std::cout << "----\n";
            std::cout << ast->toString() << "\n";
            return 0;
        }
        
        // 代码生成
        GeneratorConfig genConfig;
        genConfig.prettyPrint = true;
        genConfig.includeComments = false;
        
        // 根据CHTL.md规范，使用新的生成器
        CHTLGenerator generator(genConfig);
        String result = generator.generate(ast);
        
        String html = generator.getHtml();
        String css = generator.getCss();
        String js = generator.getJs();
        
        // 输出文件名
        String baseName = outputFile.empty() ? 
                          Util::FileSystem::getFileNameWithoutExtension(inputFile) : 
                          Util::FileSystem::getFileNameWithoutExtension(outputFile);
        
        if (outputFile.empty()) {
            outputFile = baseName + ".html";
        }
        
        // 处理内联选项（根据CHTL.md 2367-2374）
        if (inlineAll) {
            inlineCss = true;
            inlineJs = true;
        }
        
        // 构建最终HTML
        std::stringstream finalHtml;
        
        // 默认结构（根据CHTL.md 2362-2365）
        if (defaultStruct) {
            finalHtml << "<!DOCTYPE html>\n";
            finalHtml << "<html>\n";
            finalHtml << "<head>\n";
            finalHtml << "  <meta charset=\"UTF-8\">\n";
            finalHtml << "  <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n";
            finalHtml << "  <title>CHTL Page</title>\n";
            
            // CSS
            if (!css.empty() && inlineCss) {
                finalHtml << "  <style>\n" << css << "  </style>\n";
            } else if (!css.empty() && !separateFiles) {
                finalHtml << "  <link rel=\"stylesheet\" href=\"" << baseName << ".css\">\n";
            }
            
            finalHtml << "</head>\n";
            finalHtml << "<body>\n";
            finalHtml << html;
            
            // JS
            if (!js.empty() && inlineJs) {
                finalHtml << "  <script>\n" << js << "  </script>\n";
            } else if (!js.empty() && !separateFiles) {
                finalHtml << "  <script src=\"" << baseName << ".js\"></script>\n";
            }
            
            finalHtml << "</body>\n";
            finalHtml << "</html>\n";
        } else {
            // 无默认结构（根据CHTL.md 2362-2365：默认不提供结构）
            finalHtml << html;
            
            // 内联CSS
            if (!css.empty() && inlineCss) {
                finalHtml << "<style>\n" << css << "</style>\n";
            }
            
            // 内联JS
            if (!js.empty() && inlineJs) {
                finalHtml << "<script>\n" << js << "</script>\n";
            }
        }
        
        // 写入HTML文件
        if (!Util::FileSystem::writeFile(outputFile, finalHtml.str())) {
            std::cerr << "❌ Failed to write output file: " << outputFile << "\n";
            return 1;
        }
        
        std::cout << "✅ Compilation successful!\n";
        std::cout << "   Input:  " << inputFile << "\n";
        std::cout << "   Output: " << outputFile << "\n";
        
        // 分离文件输出（根据CHTL.md 2368：默认得到独立的HTML，CSS，JS文件）
        if (separateFiles || (!inlineCss && !inlineAll)) {
            if (!css.empty()) {
                String cssFile = baseName + ".css";
                if (Util::FileSystem::writeFile(cssFile, css)) {
                    std::cout << "   CSS:    " << cssFile << " (" << css.length() << " bytes)\n";
                } else {
                    std::cerr << "   Warning: Failed to write CSS file\n";
                }
            }
        }
        
        if (separateFiles || (!inlineJs && !inlineAll)) {
            if (!js.empty()) {
                String jsFile = baseName + ".js";
                if (Util::FileSystem::writeFile(jsFile, js)) {
                    std::cout << "   JS:     " << jsFile << " (" << js.length() << " bytes)\n";
                } else {
                    std::cerr << "   Warning: Failed to write JS file\n";
                }
            }
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
