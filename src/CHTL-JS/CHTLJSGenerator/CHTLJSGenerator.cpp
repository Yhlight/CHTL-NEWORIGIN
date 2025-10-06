#include "CHTLJSGenerator.h"
#include <sstream>
#include <iostream>
#include <regex>

namespace CHTL {
namespace JS {

CHTLJSGenerator::CHTLJSGenerator(const JSGeneratorConfig& config)
    : config_(config), bridge_(Bridge::SaltBridge::getInstance()) {}

String CHTLJSGenerator::generate(const String& chtljsCode) {
    String result = chtljsCode;
    
    // 第一步：处理Listen语法 (必须在处理增强选择器之前)
    result = processListenBlocks(result);
    
    // 第二步：处理增强选择器 {{...}}
    size_t pos = 0;
    while ((pos = result.find("{{", pos)) != String::npos) {
        size_t endPos = result.find("}}", pos);
        if (endPos == String::npos) {
            break;
        }
        
        String selector = result.substr(pos + 2, endPos - pos - 2);
        String jsCode = processEnhancedSelector(selector);
        result.replace(pos, endPos - pos + 2, jsCode);
        pos += jsCode.length();
    }
    
    // 第三步：处理 -> 操作符
    pos = 0;
    while ((pos = result.find("->", pos)) != String::npos) {
        result.replace(pos, 2, ".");
        pos += 1;
    }
    
    // 第四步：处理 & 引用
    pos = 0;
    while ((pos = result.find("&", pos)) != String::npos) {
        if (pos + 1 < result.length() && result[pos + 1] == '&') {
            pos += 2;
            continue;
        }
        
        String contextRef = bridge_.resolveAmpersand(false);
        if (!contextRef.empty()) {
            String jsRef = bridge_.convertEnhancedSelector(contextRef);
            result.replace(pos, 1, jsRef);
            pos += jsRef.length();
        } else {
            result.replace(pos, 1, "this");
            pos += 4;
        }
    }
    
    // 第五步：如果配置要求，用IIFE包装
    if (config_.wrapIIFE) {
        result = wrapWithIIFE(result);
    }
    
    return result;
}

String CHTLJSGenerator::processEnhancedSelector(const String& selector) {
    return bridge_.convertEnhancedSelector(selector);
}

String CHTLJSGenerator::processListen(const String& target, const String& events) {
    std::stringstream ss;
    
    // 简化实现：生成addEventListener调用
    ss << target << ".addEventListener(" << events << ")";
    
    return ss.str();
}

String CHTLJSGenerator::processEventBind(const String& target, const String& event, const String& handler) {
    std::stringstream ss;
    
    ss << target << ".addEventListener('" << event << "', " << handler << ")";
    
    return ss.str();
}

String CHTLJSGenerator::processDelegate(const String& parent, const String& /* config */) {
    std::stringstream ss;
    
    // 事件委托实现
    ss << "// Event delegation for " << parent << "\n";
    ss << "// TODO: Implement full delegation\n";
    
    return ss.str();
}

String CHTLJSGenerator::processAnimate(const String& /* config */) {
    std::stringstream ss;
    
    // 动画实现
    ss << "// Animation\n";
    ss << "// TODO: Implement animation with requestAnimationFrame\n";
    
    return ss.str();
}

String CHTLJSGenerator::processRouter(const String& /* config */) {
    std::stringstream ss;
    
    // 路由实现
    ss << "// Router\n";
    ss << "// TODO: Implement SPA router\n";
    
    return ss.str();
}

String CHTLJSGenerator::wrapWithIIFE(const String& code) {
    std::stringstream ss;
    
    if (config_.useStrict) {
        ss << "(function() {\n";
        ss << "  'use strict';\n";
        ss << "  " << code << "\n";
        ss << "})();\n";
    } else {
        ss << "(function() {\n";
        ss << "  " << code << "\n";
        ss << "})();\n";
    }
    
    return ss.str();
}

String CHTLJSGenerator::processListenBlocks(const String& code) {
    String result = code;
    
    // 使用regex查找并替换Listen块
    // 模式: {{...}}->Listen { ... } 或 identifier->Listen { ... }
    std::regex listenPattern(R"(((?:\{\{[^}]+\}\}|\w+))\s*->\s*Listen\s*\{([^}]*(?:\{[^}]*\}[^}]*)*)\})");
    
    std::string input = code;
    std::smatch match;
    std::string output;
    
    size_t lastPos = 0;
    auto searchStart = input.cbegin();
    
    while (std::regex_search(searchStart, input.cend(), match, listenPattern)) {
        // 添加匹配前的内容
        output.append(input, lastPos, match.position());
        
        String target = match[1].str();
        String eventsBlock = match[2].str();
        
        // 如果target是{{...}}，转换它
        if (target.length() >= 4 && target.substr(0, 2) == "{{" && target.substr(target.length() - 2) == "}}") {
            String selector = target.substr(2, target.length() - 4);
            target = bridge_.convertEnhancedSelector(selector);
        }
        
        // 简单解析事件绑定 - 使用字符串处理而不是复杂regex
        std::stringstream replacement;
        size_t eventPos = 0;
        
        while (eventPos < eventsBlock.length()) {
            // 跳过空白
            while (eventPos < eventsBlock.length() && std::isspace(eventsBlock[eventPos])) {
                eventPos++;
            }
            if (eventPos >= eventsBlock.length()) break;
            
            // 查找事件名 (identifier:)
            size_t colonPos = eventsBlock.find(':', eventPos);
            if (colonPos == String::npos) break;
            
            String eventName = eventsBlock.substr(eventPos, colonPos - eventPos);
            // 去除空白
            eventName.erase(eventName.find_last_not_of(" \t\n\r") + 1);
            
            // 查找handler - 从:到逗号或块结束
            size_t handlerStart = colonPos + 1;
            while (handlerStart < eventsBlock.length() && std::isspace(eventsBlock[handlerStart])) {
                handlerStart++;
            }
            
            size_t handlerEnd = handlerStart;
            int parenDepth = 0, braceDepth = 0;
            
            while (handlerEnd < eventsBlock.length()) {
                char ch = eventsBlock[handlerEnd];
                if (ch == '(') parenDepth++;
                else if (ch == ')') parenDepth--;
                else if (ch == '{') braceDepth++;
                else if (ch == '}') braceDepth--;
                else if (ch == ',' && parenDepth == 0 && braceDepth == 0) {
                    break;  // 找到分隔符
                }
                handlerEnd++;
            }
            
            String handler = eventsBlock.substr(handlerStart, handlerEnd - handlerStart);
            // 去除尾部空白和分号
            size_t trimEnd = handler.find_last_not_of(" \t\n\r,;");
            if (trimEnd != String::npos) {
                handler = handler.substr(0, trimEnd + 1);
            }
            
            replacement << target << ".addEventListener('" << eventName << "', " << handler << ");\n    ";
            
            eventPos = handlerEnd + 1;
        }
        
        output.append(replacement.str());
        
        lastPos = match.position() + match.length();
        searchStart = match.suffix().first;
    }
    
    // 添加剩余内容
    output.append(input, lastPos, std::string::npos);
    
    return output;
}

} // namespace JS
} // namespace CHTL
