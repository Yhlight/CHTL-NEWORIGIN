#include "CHTLJSGenerator.h"
#include <sstream>
#include <regex>

namespace CHTL {
namespace JS {

CHTLJSGenerator::CHTLJSGenerator(const JSGeneratorConfig& config)
    : config_(config), bridge_(Bridge::SaltBridge::getInstance()) {}

String CHTLJSGenerator::generate(const String& chtljsCode) {
    String code = chtljsCode;
    
    // 处理增强选择器 {{...}}
    std::regex selectorRegex(R"(\{\{([^}]+)\}\})");
    std::smatch match;
    String result = code;
    
    size_t offset = 0;
    std::string::const_iterator searchStart(code.cbegin());
    
    while (std::regex_search(searchStart, code.cend(), match, selectorRegex)) {
        String selector = match[1].str();
        String jsCode = processEnhancedSelector(selector);
        
        size_t pos = match.position() + offset;
        result.replace(pos, match[0].length(), jsCode);
        offset += jsCode.length() - match[0].length();
        
        searchStart = match.suffix().first;
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
