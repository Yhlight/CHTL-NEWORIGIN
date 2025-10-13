#pragma once

#include "../Common.hpp"
#include "Lexer.hpp"
#include "Parser.hpp"
#include "Generator.hpp"
#include "../Util/FileSystem.hpp"

namespace CHTL {

// 编译器选项
struct CompilerOptions {
    bool inlineCSS = false;          // 是否内联CSS
    bool inlineJS = false;           // 是否内联JS
    bool defaultStruct = false;      // 是否生成默认HTML结构
    bool debugMode = false;          // 调试模式
    Path outputDir = ".";            // 输出目录
    String outputName = "output";    // 输出文件名
};

// 编译结果
struct CompileResult {
    bool success = false;
    String html;
    String css;
    String js;
    Vec<String> errors;
    Vec<String> warnings;
};

// CHTL编译器
class Compiler {
public:
    explicit Compiler(CompilerOptions options = CompilerOptions{})
        : options_(std::move(options)) {}
    
    // 编译文件
    CompileResult compileFile(const Path& inputPath) {
        CompileResult result;
        
        try {
            // 读取源文件
            auto sourceOpt = Util::FileSystem::readFile(inputPath);
            if (!sourceOpt) {
                result.errors.push_back("Failed to read file: " + inputPath.string());
                return result;
            }
            
            // 编译源代码
            result = compile(*sourceOpt);
            
            if (result.success) {
                // 写入输出文件
                if (!writeOutput(result)) {
                    result.errors.push_back("Failed to write output files");
                    result.success = false;
                }
            }
            
        } catch (const CompileError& e) {
            result.errors.push_back(
                "Compile error at " + std::to_string(e.line()) + ":" + 
                std::to_string(e.column()) + " - " + e.what()
            );
            result.success = false;
        } catch (const std::exception& e) {
            result.errors.push_back(String("Error: ") + e.what());
            result.success = false;
        }
        
        return result;
    }
    
    // 编译源代码
    CompileResult compile(const String& source) {
        CompileResult result;
        
        try {
            // 词法分析
            Lexer lexer(source);
            Vec<Token> tokens = lexer.tokenize();
            
            if (options_.debugMode) {
                Logger::debug("Lexer produced " + std::to_string(tokens.size()) + " tokens");
            }
            
            // 语法分析
            Parser parser(tokens);
            Ptr<Node> ast = parser.parse();
            
            if (options_.debugMode) {
                Logger::debug("Parser produced AST");
            }
            
            // 代码生成
            HTMLGenerator generator;
            generator.setInlineCSS(options_.inlineCSS);
            generator.setInlineJS(options_.inlineJS);
            
            String html = generator.generate(ast);
            
            // 如果需要默认结构
            if (options_.defaultStruct) {
                html = wrapWithDefaultStructure(html);
            }
            
            result.html = html;
            result.css = generator.getCSS();
            result.js = generator.getJS();
            result.success = true;
            
        } catch (const CompileError& e) {
            result.errors.push_back(
                "Compile error at " + std::to_string(e.line()) + ":" + 
                std::to_string(e.column()) + " - " + e.what()
            );
            result.success = false;
        } catch (const std::exception& e) {
            result.errors.push_back(String("Error: ") + e.what());
            result.success = false;
        }
        
        return result;
    }
    
    void setOptions(const CompilerOptions& options) {
        options_ = options;
    }
    
private:
    CompilerOptions options_;
    
    bool writeOutput(const CompileResult& result) {
        try {
            Path htmlPath = options_.outputDir / (options_.outputName + ".html");
            
            if (!Util::FileSystem::writeFile(htmlPath, result.html)) {
                return false;
            }
            
            // 如果不是内联模式，分别写入CSS和JS
            if (!options_.inlineCSS && !result.css.empty()) {
                Path cssPath = options_.outputDir / (options_.outputName + ".css");
                if (!Util::FileSystem::writeFile(cssPath, result.css)) {
                    return false;
                }
            }
            
            if (!options_.inlineJS && !result.js.empty()) {
                Path jsPath = options_.outputDir / (options_.outputName + ".js");
                if (!Util::FileSystem::writeFile(jsPath, result.js)) {
                    return false;
                }
            }
            
            return true;
            
        } catch (...) {
            return false;
        }
    }
    
    String wrapWithDefaultStructure(const String& content) const {
        String html = "<!DOCTYPE html>\n<html>\n<head>\n";
        html += "  <meta charset=\"UTF-8\">\n";
        html += "  <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n";
        html += "  <title>CHTL Generated</title>\n";
        
        if (!options_.inlineCSS) {
            html += "  <link rel=\"stylesheet\" href=\"" + options_.outputName + ".css\">\n";
        }
        
        html += "</head>\n<body>\n";
        html += content;
        html += "</body>\n";
        
        if (!options_.inlineJS) {
            html += "<script src=\"" + options_.outputName + ".js\"></script>\n";
        }
        
        html += "</html>\n";
        
        return html;
    }
};

} // namespace CHTL
