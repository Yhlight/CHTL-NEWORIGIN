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
    
    // 处理增强选择器 {{...}}
    // 使用简单的字符串查找替换，避免regex复杂性
    size_t pos = 0;
    while ((pos = result.find("{{", pos)) != String::npos) {
        size_t endPos = result.find("}}", pos);
        if (endPos == String::npos) {
            break;  // 没有找到结束标记
        }
        
        // 提取选择器内容
        String selector = result.substr(pos + 2, endPos - pos - 2);
        
        // 转换为JavaScript
        String jsCode = processEnhancedSelector(selector);
        
        // 替换
        result.replace(pos, endPos - pos + 2, jsCode);
        pos += jsCode.length();
    }
    
    // 处理 -> 操作符（CHTL JS语法糖，转换为.）
    // 注意：必须在处理&之前，避免&->被误处理
    pos = 0;
    while ((pos = result.find("->", pos)) != String::npos) {
        result.replace(pos, 2, ".");
        pos += 1;
    }
    
    // 处理 & 引用
    pos = 0;
    while ((pos = result.find("&", pos)) != String::npos) {
        // 检查是否是 && 
        if (pos + 1 < result.length() && result[pos + 1] == '&') {
            pos += 2;  // 跳过逻辑运算符
            continue;
        }
        
        // 单独的 & 需要替换为上下文引用
        String contextRef = bridge_.resolveAmpersand(false);  // script中优先id
        if (!contextRef.empty()) {
            // 将选择器转换为JavaScript代码
            String jsRef = bridge_.convertEnhancedSelector(contextRef);
            result.replace(pos, 1, jsRef);
            pos += jsRef.length();
        } else {
            // 如果没有上下文，使用this
            result.replace(pos, 1, "this");
            pos += 4;
        }
    }
    
    // 如果配置要求，用IIFE包装
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

} // namespace JS
} // namespace CHTL
