#include "CHTLJSParser.h"
#include <algorithm>
#include <cctype>

namespace CHTL {
namespace JS {

CHTLJSParser::CHTLJSParser(const String& source, const JSParserConfig& config)
    : config_(config), source_(source) {}

Optional<ListenBlock> CHTLJSParser::parseListenBlock(const String& code) {
    // 查找 "->Listen" 或 ".Listen"
    size_t listenPos = code.find("->Listen");
    if (listenPos == String::npos) {
        listenPos = code.find(".Listen");
    }
    
    if (listenPos == String::npos) {
        return std::nullopt;
    }
    
    // 提取目标选择器
    String target = extractTarget(code, listenPos);
    if (target.empty()) {
        return std::nullopt;
    }
    
    // 查找 '{' 
    size_t bracePos = code.find('{', listenPos);
    if (bracePos == String::npos) {
        return std::nullopt;
    }
    
    // 提取块内容
    String blockContent = extractBlockContent(code, bracePos);
    if (blockContent.empty()) {
        return std::nullopt;
    }
    
    // 解析事件绑定
    Vector<EventBinding> bindings = parseEventBindings(blockContent);
    
    // 创建ListenBlock
    ListenBlock block(target);
    block.bindings = std::move(bindings);
    block.unordered = config_.allowUnorderedKeyValues;
    
    return block;
}

Vector<EventBinding> CHTLJSParser::parseEventBindings(const String& bindingsCode) {
    Vector<EventBinding> bindings;
    
    // 分割绑定（考虑嵌套）
    Vector<String> bindingStrings = splitBindings(bindingsCode);
    
    for (const auto& bindingStr : bindingStrings) {
        auto binding = parseEventBinding(bindingStr);
        if (binding.has_value()) {
            bindings.push_back(binding.value());
        }
    }
    
    return bindings;
}

Optional<EventBinding> CHTLJSParser::parseEventBinding(const String& bindingCode) {
    String code = trimWhitespace(bindingCode);
    if (code.empty()) {
        return std::nullopt;
    }
    
    // 查找冒号
    size_t colonPos = code.find(':');
    if (colonPos == String::npos) {
        return std::nullopt;
    }
    
    // 提取事件名
    String eventName = code.substr(0, colonPos);
    eventName = trimWhitespace(eventName);
    
    // 检查是否是可选的（以?结尾）
    bool isOptional = false;
    if (!eventName.empty() && eventName.back() == '?') {
        isOptional = true;
        eventName.pop_back();
        eventName = trimWhitespace(eventName);
    }
    
    // 提取处理器
    String handler = code.substr(colonPos + 1);
    handler = trimWhitespace(handler);
    
    // 移除末尾的逗号或分号
    while (!handler.empty() && (handler.back() == ',' || handler.back() == ';')) {
        handler.pop_back();
        handler = trimWhitespace(handler);
    }
    
    if (eventName.empty() || handler.empty()) {
        return std::nullopt;
    }
    
    return EventBinding(eventName, handler, isOptional);
}

Optional<std::pair<size_t, size_t>> CHTLJSParser::findListenBlock(const String& code, size_t startPos) {
    size_t listenPos = code.find("Listen", startPos);
    if (listenPos == String::npos) {
        return std::nullopt;
    }
    
    // 确保Listen前面是-> 或 .
    if (listenPos >= 2) {
        String prefix = code.substr(listenPos - 2, 2);
        if (prefix != "->" && code[listenPos - 1] != '.') {
            // 继续查找下一个
            return findListenBlock(code, listenPos + 1);
        }
    } else if (listenPos >= 1 && code[listenPos - 1] != '.') {
        return findListenBlock(code, listenPos + 1);
    }
    
    // 查找 '{'
    size_t bracePos = code.find('{', listenPos);
    if (bracePos == String::npos) {
        return std::nullopt;
    }
    
    // 查找匹配的 '}'
    size_t endBracePos = findMatchingBrace(code, bracePos);
    if (endBracePos == String::npos) {
        return std::nullopt;
    }
    
    // 找到目标的起始位置
    size_t targetStart = listenPos;
    while (targetStart > startPos && !isWhitespace(code[targetStart - 1])) {
        targetStart--;
        // 处理 {{...}}
        if (code[targetStart] == '}' && targetStart >= 1 && code[targetStart - 1] == '}') {
            // 向前查找 {{
            while (targetStart > startPos) {
                targetStart--;
                if (targetStart >= 1 && code[targetStart] == '{' && code[targetStart - 1] == '{') {
                    targetStart--;
                    break;
                }
            }
        }
    }
    
    return std::make_pair(targetStart, endBracePos + 1);
}

String CHTLJSParser::extractTarget(const String& code, size_t listenPos) const {
    // 向前查找目标
    size_t pos = listenPos;
    
    // 跳过 "->" 或 "."
    while (pos > 0 && (code[pos] == '-' || code[pos] == '>' || code[pos] == '.' || isWhitespace(code[pos]))) {
        pos--;
    }
    
    if (pos == 0) {
        return "";
    }
    
    // 向前提取标识符或 {{...}}
    size_t endPos = pos + 1;
    size_t startPos = pos;
    
    // 如果是 }}，查找对应的 {{
    if (pos >= 1 && code[pos] == '}' && code[pos - 1] == '}') {
        endPos = pos + 1;
        pos -= 2;  // 跳过 }}
        
        int depth = 1;
        while (pos > 0 && depth > 0) {
            if (code[pos] == '}' && pos >= 1 && code[pos - 1] == '}') {
                depth++;
                pos -= 2;
            } else if (code[pos] == '{' && pos >= 1 && code[pos - 1] == '{') {
                depth--;
                if (depth == 0) {
                    startPos = pos - 1;
                    break;
                }
                pos -= 2;
            } else {
                pos--;
            }
        }
    } else {
        // 普通标识符
        while (startPos > 0 && isIdentifierChar(code[startPos - 1])) {
            startPos--;
        }
    }
    
    return code.substr(startPos, endPos - startPos);
}

String CHTLJSParser::extractBlockContent(const String& code, size_t startPos) const {
    if (startPos >= code.length() || code[startPos] != '{') {
        return "";
    }
    
    size_t endPos = findMatchingBrace(code, startPos);
    if (endPos == String::npos) {
        return "";
    }
    
    // 返回 { 和 } 之间的内容
    if (endPos <= startPos + 1) {
        return "";
    }
    
    return code.substr(startPos + 1, endPos - startPos - 1);
}

size_t CHTLJSParser::findMatchingBrace(const String& code, size_t leftBracePos) const {
    if (leftBracePos >= code.length() || code[leftBracePos] != '{') {
        return String::npos;
    }
    
    int depth = 1;
    size_t pos = leftBracePos + 1;
    bool inString = false;
    char stringChar = '\0';
    bool escaped = false;
    
    while (pos < code.length() && depth > 0) {
        char ch = code[pos];
        
        if (escaped) {
            escaped = false;
            pos++;
            continue;
        }
        
        if (ch == '\\') {
            escaped = true;
            pos++;
            continue;
        }
        
        if (inString) {
            if (ch == stringChar) {
                inString = false;
            }
        } else {
            if (ch == '"' || ch == '\'' || ch == '`') {
                inString = true;
                stringChar = ch;
            } else if (ch == '{') {
                depth++;
            } else if (ch == '}') {
                depth--;
                if (depth == 0) {
                    return pos;
                }
            }
        }
        
        pos++;
    }
    
    return String::npos;
}

Vector<String> CHTLJSParser::splitBindings(const String& bindingsCode) const {
    Vector<String> bindings;
    String current;
    
    int parenDepth = 0;
    int braceDepth = 0;
    bool inString = false;
    char stringChar = '\0';
    bool escaped = false;
    
    for (size_t i = 0; i < bindingsCode.length(); i++) {
        char ch = bindingsCode[i];
        
        if (escaped) {
            current += ch;
            escaped = false;
            continue;
        }
        
        if (ch == '\\') {
            current += ch;
            escaped = true;
            continue;
        }
        
        if (inString) {
            current += ch;
            if (ch == stringChar) {
                inString = false;
            }
            continue;
        }
        
        if (ch == '"' || ch == '\'' || ch == '`') {
            current += ch;
            inString = true;
            stringChar = ch;
            continue;
        }
        
        if (ch == '(') {
            parenDepth++;
            current += ch;
        } else if (ch == ')') {
            parenDepth--;
            current += ch;
        } else if (ch == '{') {
            braceDepth++;
            current += ch;
        } else if (ch == '}') {
            braceDepth--;
            current += ch;
        } else if ((ch == ',' || ch == ';') && parenDepth == 0 && braceDepth == 0) {
            // 分隔符 - 同时处理逗号和分号
            String trimmed = trimWhitespace(current);
            if (!trimmed.empty()) {
                bindings.push_back(trimmed);
            }
            current.clear();
        } else if (ch != ';' || parenDepth > 0 || braceDepth > 0) {
            // 只在非顶层或非分号时添加字符
            current += ch;
        }
        // 顶层的分号被忽略（已作为分隔符处理）
    }
    
    // 添加最后一个
    String trimmed = trimWhitespace(current);
    if (!trimmed.empty()) {
        bindings.push_back(trimmed);
    }
    
    return bindings;
}

String CHTLJSParser::extractFunctionBody(const String& code, size_t startPos, size_t& endPos) const {
    // 简化实现：查找完整的函数表达式
    size_t pos = startPos;
    
    // 跳过空白
    while (pos < code.length() && isWhitespace(code[pos])) {
        pos++;
    }
    
    if (pos >= code.length()) {
        endPos = pos;
        return "";
    }
    
    // 处理不同类型的函数
    // 1. 箭头函数: () => {}
    // 2. function关键字: function() {}
    // 3. 函数引用: handlerFunction
    
    size_t start = pos;
    int parenDepth = 0;
    int braceDepth = 0;
    bool inString = false;
    char stringChar = '\0';
    
    while (pos < code.length()) {
        char ch = code[pos];
        
        if (!inString) {
            if (ch == '"' || ch == '\'' || ch == '`') {
                inString = true;
                stringChar = ch;
            } else if (ch == '(') {
                parenDepth++;
            } else if (ch == ')') {
                parenDepth--;
            } else if (ch == '{') {
                braceDepth++;
            } else if (ch == '}') {
                braceDepth--;
                if (braceDepth < 0) {
                    break;  // 超出当前作用域
                }
            } else if ((ch == ',' || ch == ';') && parenDepth == 0 && braceDepth == 0) {
                break;  // 函数结束
            }
        } else {
            if (ch == stringChar && (pos == 0 || code[pos - 1] != '\\')) {
                inString = false;
            }
        }
        
        pos++;
    }
    
    endPos = pos;
    return code.substr(start, pos - start);
}

String CHTLJSParser::trimWhitespace(const String& str) const {
    if (str.empty()) {
        return str;
    }
    
    size_t start = 0;
    while (start < str.length() && isWhitespace(str[start])) {
        start++;
    }
    
    if (start >= str.length()) {
        return "";
    }
    
    size_t end = str.length() - 1;
    while (end > start && isWhitespace(str[end])) {
        end--;
    }
    
    return str.substr(start, end - start + 1);
}

bool CHTLJSParser::isWhitespace(char ch) const {
    return ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r';
}

bool CHTLJSParser::isIdentifierChar(char ch) const {
    return std::isalnum(static_cast<unsigned char>(ch)) || ch == '_' || ch == '$';
}

} // namespace JS
} // namespace CHTL
