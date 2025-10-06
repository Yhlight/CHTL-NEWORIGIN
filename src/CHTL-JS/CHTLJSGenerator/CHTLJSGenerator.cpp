#include "CHTLJSGenerator.h"
#include <sstream>
#include <iostream>

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
    String result;
    size_t lastPos = 0;
    size_t searchPos = 0;
    
    // 创建解析器
    CHTLJSParser parser(code);
    
    // 查找所有Listen块
    while (true) {
        auto blockPos = parser.findListenBlock(code, searchPos);
        if (!blockPos.has_value()) {
            // 没有更多Listen块，添加剩余内容
            result += code.substr(lastPos);
            break;
        }
        
        size_t startPos = blockPos->first;
        size_t endPos = blockPos->second;
        
        // 添加Listen块之前的内容
        result += code.substr(lastPos, startPos - lastPos);
        
        // 提取并解析Listen块
        String blockCode = code.substr(startPos, endPos - startPos);
        auto listenBlock = parser.parseListenBlock(blockCode);
        
        if (listenBlock.has_value()) {
            // 生成addEventListener代码
            String target = listenBlock->target;
            
            // 如果target是{{...}}，转换它
            if (target.length() >= 4 && target.substr(0, 2) == "{{" && 
                target.substr(target.length() - 2) == "}}") {
                String selector = target.substr(2, target.length() - 4);
                target = bridge_.convertEnhancedSelector(selector);
            }
            
            // 生成每个事件绑定
            std::stringstream ss;
            for (size_t i = 0; i < listenBlock->bindings.size(); i++) {
                const auto& binding = listenBlock->bindings[i];
                
                ss << target << ".addEventListener('" 
                   << binding.eventName << "', " 
                   << binding.handler << ")";
                
                // 添加分号和换行（除了最后一个）
                if (i < listenBlock->bindings.size() - 1) {
                    ss << ";\n    ";
                }
            }
            
            result += ss.str();
        } else {
            // 解析失败，保留原始代码
            result += blockCode;
        }
        
        lastPos = endPos;
        searchPos = endPos;
    }
    
    return result;
}

} // namespace JS
} // namespace CHTL
