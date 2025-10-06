#include "CHTLJSParser.h"
#include <algorithm>
#include <cctype>
#include <iostream>

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
    int bracketDepth = 0;  // 新增：跟踪[]深度
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
        } else if (ch == '[') {
            bracketDepth++;  // 新增：跟踪[
            current += ch;
        } else if (ch == ']') {
            bracketDepth--;  // 新增：跟踪]
            current += ch;
        } else if ((ch == ',' || ch == ';') && parenDepth == 0 && braceDepth == 0 && bracketDepth == 0) {
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

// ============= 事件绑定操作符 &-> 实现 =============

Optional<EventBindOperator> CHTLJSParser::parseEventBindOperator(const String& code) {
    // 查找 &->
    size_t bindPos = code.find("&->");
    if (bindPos == String::npos) {
        return std::nullopt;
    }
    
    // 提取目标选择器（在 &-> 之前）
    // 向前跳过空白
    size_t targetEnd = bindPos;
    while (targetEnd > 0 && isWhitespace(code[targetEnd - 1])) {
        targetEnd--;
    }
    
    if (targetEnd == 0) {
        return std::nullopt;
    }
    
    // 找到目标起始位置
    size_t targetStart = targetEnd;
    
    // 如果是 {{...}}
    if (targetStart >= 2 && code[targetStart - 1] == '}' && code[targetStart - 2] == '}') {
        targetStart -= 2;  // 跳过 }}
        
        int depth = 1;
        while (targetStart > 0 && depth > 0) {
            if (targetStart >= 2 && code[targetStart - 1] == '}' && code[targetStart - 2] == '}') {
                depth++;
                targetStart -= 2;
            } else if (targetStart >= 2 && code[targetStart - 1] == '{' && code[targetStart - 2] == '{') {
                depth--;
                if (depth == 0) {
                    targetStart -= 2;
                    break;
                }
                targetStart -= 2;
            } else {
                targetStart--;
            }
        }
    } else {
        // 普通标识符
        while (targetStart > 0 && isIdentifierChar(code[targetStart - 1])) {
            targetStart--;
        }
    }
    
    String target = code.substr(targetStart, targetEnd - targetStart);
    target = trimWhitespace(target);
    
    if (target.empty()) {
        return std::nullopt;
    }
    
    // 跳过 &->
    size_t pos = bindPos + 3;
    while (pos < code.length() && isWhitespace(code[pos])) {
        pos++;
    }
    
    if (pos >= code.length()) {
        return std::nullopt;
    }
    
    // 检查是否是绑定块形式 &-> { ... }
    if (code[pos] == '{') {
        // 绑定块形式
        String blockContent = extractBlockContent(code, pos);
        if (blockContent.empty()) {
            return std::nullopt;
        }
        
        // 解析块内的事件绑定
        Vector<EventBinding> bindings = parseEventBindings(blockContent);
        
        // 创建EventBindOperator（绑定块形式）
        EventBindOperator op;
        op.target = target;
        op.isBlock = true;
        
        // 将所有绑定信息存储（暂时只取第一个作为示例）
        // 实际应该返回多个EventBindOperator或修改结构
        // 为了简化，我们将绑定块转换为多个单独的绑定
        if (!bindings.empty()) {
            op.eventNames.push_back(bindings[0].eventName);
            op.handler = bindings[0].handler;
        }
        
        return op;
    }
    
    // 单行形式: event: handler 或 event1, event2: handler
    // 查找冒号
    size_t colonPos = code.find(':', pos);
    if (colonPos == String::npos) {
        return std::nullopt;
    }
    
    // 提取事件名称部分
    String eventsStr = code.substr(pos, colonPos - pos);
    Vector<String> eventNames = parseEventNames(eventsStr);
    
    if (eventNames.empty()) {
        return std::nullopt;
    }
    
    // 提取处理器
    pos = colonPos + 1;
    while (pos < code.length() && isWhitespace(code[pos])) {
        pos++;
    }
    
    size_t endPos = pos;
    String handler = extractFunctionBody(code, pos, endPos);
    handler = trimWhitespace(handler);
    
    // 移除尾部的逗号和分号
    while (!handler.empty() && (handler.back() == ',' || handler.back() == ';')) {
        handler.pop_back();
        handler = trimWhitespace(handler);
    }
    
    if (handler.empty()) {
        return std::nullopt;
    }
    
    return EventBindOperator(target, eventNames, handler);
}

Optional<std::pair<size_t, size_t>> CHTLJSParser::findEventBindOperator(const String& code, size_t startPos) {
    size_t bindPos = code.find("&->", startPos);
    if (bindPos == String::npos) {
        return std::nullopt;
    }
    
    // 向前跳过空白找到目标结束位置
    size_t targetEnd = bindPos;
    while (targetEnd > startPos && isWhitespace(code[targetEnd - 1])) {
        targetEnd--;
    }
    
    if (targetEnd <= startPos) {
        return std::nullopt;
    }
    
    // 找到目标的起始位置
    size_t targetStart = targetEnd;
    
    // 如果是 {{...}}，找到完整的增强选择器
    if (targetStart >= 2 && code[targetStart - 1] == '}' && code[targetStart - 2] == '}') {
        targetStart -= 2;  // 跳过 }}
        
        int depth = 1;
        while (targetStart > startPos && depth > 0) {
            if (targetStart >= 2 && code[targetStart - 1] == '}' && code[targetStart - 2] == '}') {
                depth++;
                targetStart -= 2;
            } else if (targetStart >= 2 && code[targetStart - 1] == '{' && code[targetStart - 2] == '{') {
                depth--;
                if (depth == 0) {
                    targetStart -= 2;
                    break;
                }
                targetStart -= 2;
            } else {
                targetStart--;
            }
        }
    } else {
        // 普通标识符
        while (targetStart > startPos && isIdentifierChar(code[targetStart - 1])) {
            targetStart--;
        }
    }
    
    // 找到结束位置
    size_t pos = bindPos + 3;
    while (pos < code.length() && isWhitespace(code[pos])) {
        pos++;
    }
    
    size_t endPos;
    
    if (pos < code.length() && code[pos] == '{') {
        // 绑定块形式
        endPos = findMatchingBrace(code, pos);
        if (endPos == String::npos) {
            return std::nullopt;
        }
        endPos++; // 包含 }
    } else {
        // 单行形式，查找分号或逗号后的&->或行尾
        endPos = pos;
        int parenDepth = 0, braceDepth = 0;
        bool inString = false;
        char stringChar = '\0';
        
        while (endPos < code.length()) {
            char ch = code[endPos];
            
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
                } else if (ch == ';' && parenDepth == 0 && braceDepth == 0) {
                    endPos++;
                    break;
                } else if (ch == ',' && parenDepth == 0 && braceDepth == 0) {
                    // 检查下一个是否是 &->（链式绑定）
                    size_t nextPos = endPos + 1;
                    while (nextPos < code.length() && isWhitespace(code[nextPos])) {
                        nextPos++;
                    }
                    if (nextPos + 3 <= code.length() && code.substr(nextPos, 3) == "&->") {
                        // 链式绑定：继续查找，包含所有链式&->
                        endPos = nextPos + 3;  // 跳过 &->
                        continue;  // 继续查找更多的链式绑定
                    } else {
                        // 不是链式绑定，在逗号处结束
                        endPos++;
                        break;
                    }
                }
            } else {
                if (ch == stringChar && (endPos == 0 || code[endPos - 1] != '\\')) {
                    inString = false;
                }
            }
            
            endPos++;
        }
    }
    
    return std::make_pair(targetStart, endPos);
}

Vector<String> CHTLJSParser::parseEventNames(const String& namesCode) {
    Vector<String> names;
    String current;
    
    for (size_t i = 0; i < namesCode.length(); i++) {
        char ch = namesCode[i];
        
        if (ch == ',') {
            String trimmed = trimWhitespace(current);
            if (!trimmed.empty()) {
                names.push_back(trimmed);
            }
            current.clear();
        } else {
            current += ch;
        }
    }
    
    // 添加最后一个
    String trimmed = trimWhitespace(current);
    if (!trimmed.empty()) {
        names.push_back(trimmed);
    }
    
    return names;
}

bool CHTLJSParser::isEventBindBlock(const String& code, size_t bindPos) {
    size_t pos = bindPos + 3; // 跳过 &->
    while (pos < code.length() && isWhitespace(code[pos])) {
        pos++;
    }
    return pos < code.length() && code[pos] == '{';
}

Vector<EventBindOperator> CHTLJSParser::parseChainBinding(const String& code) {
    Vector<EventBindOperator> operators;
    
    // 先找到第一个&->来获取目标
    size_t firstBindPos = code.find("&->");
    if (firstBindPos == String::npos) {
        return operators;
    }
    
    // 提取共同的目标
    size_t targetEnd = firstBindPos;
    while (targetEnd > 0 && isWhitespace(code[targetEnd - 1])) {
        targetEnd--;
    }
    
    size_t targetStart = 0;
    if (targetEnd >= 2 && code[targetEnd - 1] == '}' && code[targetEnd - 2] == '}') {
        targetStart = targetEnd - 2;
        int depth = 1;
        while (targetStart > 0 && depth > 0) {
            if (targetStart >= 2 && code[targetStart - 1] == '}' && code[targetStart - 2] == '}') {
                depth++;
                targetStart -= 2;
            } else if (targetStart >= 2 && code[targetStart - 1] == '{' && code[targetStart - 2] == '{') {
                depth--;
                if (depth == 0) {
                    targetStart -= 2;
                    break;
                }
                targetStart -= 2;
            } else {
                targetStart--;
            }
        }
    } else {
        while (targetStart < targetEnd && isWhitespace(code[targetStart])) {
            targetStart++;
        }
        while (targetStart < targetEnd && !isWhitespace(code[targetStart]) && 
               code[targetStart] != '&') {
            targetEnd++;
            if (targetEnd < code.length() && isIdentifierChar(code[targetEnd])) {
                continue;
            }
            break;
        }
    }
    
    String sharedTarget = code.substr(targetStart, targetEnd - targetStart);
    sharedTarget = trimWhitespace(sharedTarget);
    
    // 分割每个&->绑定
    size_t pos = 0;
    while ((pos = code.find("&->", pos)) != String::npos) {
        // 跳过&->
        size_t bindStart = pos + 3;
        while (bindStart < code.length() && isWhitespace(code[bindStart])) {
            bindStart++;
        }
        
        // 找到这个绑定的结束位置（逗号或分号）
        size_t bindEnd = bindStart;
        int parenDepth = 0, braceDepth = 0;
        
        while (bindEnd < code.length()) {
            char ch = code[bindEnd];
            if (ch == '(') parenDepth++;
            else if (ch == ')') parenDepth--;
            else if (ch == '{') braceDepth++;
            else if (ch == '}') braceDepth--;
            else if ((ch == ',' || ch == ';') && parenDepth == 0 && braceDepth == 0) {
                break;
            }
            bindEnd++;
        }
        
        // 提取并解析这个绑定
        String bindCode = code.substr(bindStart, bindEnd - bindStart);
        auto binding = parseEventBinding(bindCode);
        
        if (binding.has_value()) {
            EventBindOperator op(sharedTarget, {binding->eventName}, binding->handler);
            operators.push_back(op);
        }
        
        pos = bindEnd + 1;
    }
    
    return operators;
}

// ============= Delegate事件委托实现 =============

Optional<DelegateBlock> CHTLJSParser::parseDelegateBlock(const String& code) {
    // 查找 "->Delegate" 或 ".Delegate"
    size_t delegatePos = code.find("->Delegate");
    if (delegatePos == String::npos) {
        delegatePos = code.find(".Delegate");
    }
    
    if (delegatePos == String::npos) {
        return std::nullopt;
    }
    
    // 提取父元素选择器
    String parent = extractTarget(code, delegatePos);
    if (parent.empty()) {
        return std::nullopt;
    }
    
    // 查找 '{'
    size_t bracePos = code.find('{', delegatePos);
    if (bracePos == String::npos) {
        return std::nullopt;
    }
    
    // 提取块内容
    String blockContent = extractBlockContent(code, bracePos);
    if (blockContent.empty()) {
        return std::nullopt;
    }
    
    // 创建DelegateBlock
    DelegateBlock block(parent);
    
    // 解析块内容
    // 首先查找target键
    size_t targetPos = blockContent.find("target");
    if (targetPos != String::npos) {
        // 查找target后的冒号
        size_t colonPos = blockContent.find(':', targetPos);
        if (colonPos != String::npos) {
            size_t valueStart = colonPos + 1;
            while (valueStart < blockContent.length() && isWhitespace(blockContent[valueStart])) {
                valueStart++;
            }
            
            // 检查是否是数组 [...]
            if (valueStart < blockContent.length() && blockContent[valueStart] == '[') {
                // 数组形式的targets
                size_t arrayEnd = blockContent.find(']', valueStart);
                if (arrayEnd != String::npos) {
                    String arrayContent = blockContent.substr(valueStart + 1, arrayEnd - valueStart - 1);
                    
                    // 分割targets（简单实现：按逗号分割）
                    String current;
                    for (size_t i = 0; i < arrayContent.length(); i++) {
                        char ch = arrayContent[i];
                        if (ch == ',') {
                            String target = trimWhitespace(current);
                            if (!target.empty()) {
                                block.targets.push_back(target);
                            }
                            current.clear();
                        } else {
                            current += ch;
                        }
                    }
                    // 添加最后一个
                    String target = trimWhitespace(current);
                    if (!target.empty()) {
                        block.targets.push_back(target);
                    }
                }
            } else {
                // 单个target
                size_t valueEnd = valueStart;
                while (valueEnd < blockContent.length() && blockContent[valueEnd] != ',' && 
                       blockContent[valueEnd] != ';' && blockContent[valueEnd] != '\n') {
                    valueEnd++;
                }
                String target = blockContent.substr(valueStart, valueEnd - valueStart);
                target = trimWhitespace(target);
                if (!target.empty()) {
                    block.targets.push_back(target);
                }
            }
        }
    }
    
    // 解析事件绑定（除了target键外的其他键值对）
    // 简化实现：将整个块内容当作事件绑定解析，然后过滤掉target
    auto allBindings = parseEventBindings(blockContent);
    for (const auto& binding : allBindings) {
        if (binding.eventName != "target") {
            block.eventBindings.push_back(binding);
        }
    }
    
    return block;
}

Optional<std::pair<size_t, size_t>> CHTLJSParser::findDelegateBlock(const String& code, size_t startPos) {
    size_t delegatePos = code.find("Delegate", startPos);
    if (delegatePos == String::npos) {
        return std::nullopt;
    }
    
    // 确保Delegate前面是-> 或 .
    if (delegatePos >= 2) {
        String prefix = code.substr(delegatePos - 2, 2);
        if (prefix != "->" && code[delegatePos - 1] != '.') {
            // 继续查找下一个
            return findDelegateBlock(code, delegatePos + 1);
        }
    } else if (delegatePos >= 1 && code[delegatePos - 1] != '.') {
        return findDelegateBlock(code, delegatePos + 1);
    }
    
    // 查找 '{'
    size_t bracePos = code.find('{', delegatePos);
    if (bracePos == String::npos) {
        return std::nullopt;
    }
    
    // 查找匹配的 '}'
    size_t endBracePos = findMatchingBrace(code, bracePos);
    if (endBracePos == String::npos) {
        return std::nullopt;
    }
    
    // 找到目标的起始位置
    size_t targetStart = delegatePos;
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

// ============= Animate动画系统实现 =============

Optional<AnimateBlock> CHTLJSParser::parseAnimateBlock(const String& code) {
    // 查找 "Animate"
    size_t animatePos = code.find("Animate");
    if (animatePos == String::npos) {
        return std::nullopt;
    }
    
    // 查找 '{'
    size_t bracePos = code.find('{', animatePos);
    if (bracePos == String::npos) {
        return std::nullopt;
    }
    
    // 提取块内容
    String blockContent = extractBlockContent(code, bracePos);
    if (blockContent.empty()) {
        return std::nullopt;
    }
    
    AnimateBlock block;
    
    // 解析各个键值对
    auto bindings = parseEventBindings(blockContent);
    
    for (const auto& binding : bindings) {
        String key = binding.eventName;
        String value = binding.handler;
        
        if (key == "target") {
            // 处理target（可能是数组）
            value = trimWhitespace(value);
            if (!value.empty() && value[0] == '[') {
                // 数组形式
                size_t end = value.find(']');
                if (end != String::npos) {
                    String arrayContent = value.substr(1, end - 1);
                    String current;
                    for (size_t i = 0; i < arrayContent.length(); i++) {
                        char ch = arrayContent[i];
                        if (ch == ',') {
                            String target = trimWhitespace(current);
                            if (!target.empty()) {
                                block.targets.push_back(target);
                            }
                            current.clear();
                        } else {
                            current += ch;
                        }
                    }
                    String target = trimWhitespace(current);
                    if (!target.empty()) {
                        block.targets.push_back(target);
                    }
                }
            } else {
                block.targets.push_back(value);
            }
        } else if (key == "duration") {
            block.duration = std::stoi(value);
        } else if (key == "easing") {
            block.easing = value;
        } else if (key == "loop") {
            block.loop = std::stoi(value);
        } else if (key == "direction") {
            block.direction = value;
        } else if (key == "delay") {
            block.delay = std::stoi(value);
        } else if (key == "callback") {
            block.callback = value;
        } else if (key == "begin") {
            block.begin = parseCssProperties(value);
        } else if (key == "end") {
            block.end = parseCssProperties(value);
        } else if (key == "when") {
            // 解析关键帧数组
            value = trimWhitespace(value);
            // TODO: 修复splitBindings来正确处理嵌套数组
            // 当前问题：when: [{ at: 0.25 }, { at: 0.5 }] 在第一个逗号处被错误分割
            if (value.length() >= 2 && value[0] == '[' && value[value.length() - 1] == ']') {
                String arrayContent = value.substr(1, value.length() - 2);
                // 简化：按}分割关键帧
                size_t pos = 0;
                while (pos < arrayContent.length()) {
                    size_t start = arrayContent.find('{', pos);
                    if (start == String::npos) break;
                    
                    size_t end = findMatchingBrace(arrayContent, start);
                    if (end == String::npos) break;
                    
                    String frameContent = arrayContent.substr(start + 1, end - start - 1);
                    // std::cerr << "  frameContent: '" << frameContent << "'\n";
                    auto frameProps = parseEventBindings(frameContent);
                    // std::cerr << "  frameProps.size(): " << frameProps.size() << "\n";
                    
                    KeyFrame frame;
                    for (const auto& prop : frameProps) {
                        if (prop.eventName == "at") {
                            frame.at = std::stod(prop.handler);
                        } else {
                            frame.properties[prop.eventName] = prop.handler;
                        }
                    }
                    
                    block.when.push_back(frame);
                    pos = end + 1;
                }
            }
        }
    }
    
    return block;
}

Optional<std::pair<size_t, size_t>> CHTLJSParser::findAnimateBlock(const String& code, size_t startPos) {
    size_t animatePos = code.find("Animate", startPos);
    if (animatePos == String::npos) {
        return std::nullopt;
    }
    
    // 查找 '{'
    size_t bracePos = code.find('{', animatePos);
    if (bracePos == String::npos) {
        return std::nullopt;
    }
    
    // 查找匹配的 '}'
    size_t endBracePos = findMatchingBrace(code, bracePos);
    if (endBracePos == String::npos) {
        return std::nullopt;
    }
    
    return std::make_pair(animatePos, endBracePos + 1);
}

HashMap<String, String> CHTLJSParser::parseCssProperties(const String& code) {
    HashMap<String, String> properties;
    
    String content = trimWhitespace(code);
    
    // 如果被{}包裹，去掉
    if (!content.empty() && content[0] == '{' && content[content.length() - 1] == '}') {
        content = content.substr(1, content.length() - 2);
    }
    
    // 解析CSS属性
    auto bindings = parseEventBindings(content);
    for (const auto& binding : bindings) {
        properties[binding.eventName] = binding.handler;
    }
    
    return properties;
}

// ============= Router路由系统实现 =============

Optional<RouterBlock> CHTLJSParser::parseRouterBlock(const String& code) {
    // 查找 "Router"
    size_t routerPos = code.find("Router");
    if (routerPos == String::npos) {
        return std::nullopt;
    }
    
    // 查找 '{'
    size_t bracePos = code.find('{', routerPos);
    if (bracePos == String::npos) {
        return std::nullopt;
    }
    
    // 提取块内容
    String blockContent = extractBlockContent(code, bracePos);
    if (blockContent.empty()) {
        return std::nullopt;
    }
    
    RouterBlock block;
    
    // 解析各个键值对
    auto bindings = parseEventBindings(blockContent);
    
    Vector<String> urls;
    Vector<String> pages;
    
    for (const auto& binding : bindings) {
        String key = binding.eventName;
        String value = trimWhitespace(binding.handler);
        
        if (key == "url") {
            // 解析URL（可能是多个）
            if (value.find(',') != String::npos) {
                // 多个URL，按逗号分割
                Vector<String> urlList;
                String current;
                int depth = 0;
                for (char ch : value) {
                    if (ch == '{' || ch == '(' || ch == '[') depth++;
                    else if (ch == '}' || ch == ')' || ch == ']') depth--;
                    else if (ch == ',' && depth == 0) {
                        String url = trimWhitespace(current);
                        if (!url.empty()) {
                            // 移除引号
                            if ((url.front() == '"' && url.back() == '"') ||
                                (url.front() == '\'' && url.back() == '\'')) {
                                url = url.substr(1, url.length() - 2);
                            }
                            urls.push_back(url);
                        }
                        current.clear();
                        continue;
                    }
                    current += ch;
                }
                String url = trimWhitespace(current);
                if (!url.empty()) {
                    if ((url.front() == '"' && url.back() == '"') ||
                        (url.front() == '\'' && url.back() == '\'')) {
                        url = url.substr(1, url.length() - 2);
                    }
                    urls.push_back(url);
                }
            } else {
                // 单个URL
                if ((value.front() == '"' && value.back() == '"') ||
                    (value.front() == '\'' && value.back() == '\'')) {
                    value = value.substr(1, value.length() - 2);
                }
                urls.push_back(value);
            }
        } else if (key == "page") {
            // 解析page（可能是多个）
            if (value.find(',') != String::npos) {
                // 多个page，按逗号分割
                String current;
                int depth = 0;
                for (char ch : value) {
                    if (ch == '{' || ch == '(' || ch == '[') depth++;
                    else if (ch == '}' || ch == ')' || ch == ']') depth--;
                    else if (ch == ',' && depth == 0) {
                        String page = trimWhitespace(current);
                        if (!page.empty()) {
                            pages.push_back(page);
                        }
                        current.clear();
                        continue;
                    }
                    current += ch;
                }
                String page = trimWhitespace(current);
                if (!page.empty()) {
                    pages.push_back(page);
                }
            } else {
                pages.push_back(value);
            }
        } else if (key == "mode") {
            block.mode = value;
            // 移除引号
            if ((block.mode.front() == '"' && block.mode.back() == '"') ||
                (block.mode.front() == '\'' && block.mode.back() == '\'')) {
                block.mode = block.mode.substr(1, block.mode.length() - 2);
            }
        } else if (key == "root") {
            // TODO: 处理root
            block.rootPath = value;
        }
    }
    
    // 组合URL和page
    for (size_t i = 0; i < urls.size() && i < pages.size(); i++) {
        block.routes.push_back(RouteRule(urls[i], pages[i]));
    }
    
    return block;
}

Optional<std::pair<size_t, size_t>> CHTLJSParser::findRouterBlock(const String& code, size_t startPos) {
    size_t routerPos = code.find("Router", startPos);
    if (routerPos == String::npos) {
        return std::nullopt;
    }
    
    // 查找 '{'
    size_t bracePos = code.find('{', routerPos);
    if (bracePos == String::npos) {
        return std::nullopt;
    }
    
    // 查找匹配的 '}'
    size_t endBracePos = findMatchingBrace(code, bracePos);
    if (endBracePos == String::npos) {
        return std::nullopt;
    }
    
    return std::make_pair(routerPos, endBracePos + 1);
}

// ============= Vir虚对象实现 =============

Optional<VirObject> CHTLJSParser::parseVirDeclaration(const String& code) {
    size_t virPos = code.find("Vir");
    if (virPos == String::npos) {
        return std::nullopt;
    }
    
    // 查找对象名称（Vir之后，=之前）
    size_t equalPos = code.find('=', virPos);
    if (equalPos == String::npos) {
        return std::nullopt;
    }
    
    String nameSection = code.substr(virPos + 3, equalPos - virPos - 3);
    String name = trimWhitespace(nameSection);
    
    VirObject vir(name);
    
    // 查找源类型（Listen、Animate等）
    size_t afterEqual = equalPos + 1;
    while (afterEqual < code.length() && isWhitespace(code[afterEqual])) {
        afterEqual++;
    }
    
    // 检测源类型
    if (code.substr(afterEqual, 6) == "Listen") {
        vir.sourceType = "Listen";
        // 解析Listen块获取成员
        auto listenBlock = parseListenBlock(code.substr(afterEqual));
        if (listenBlock.has_value()) {
            for (const auto& binding : listenBlock->bindings) {
                vir.members[binding.eventName] = binding.handler;
            }
        }
    } else if (code.substr(afterEqual, 7) == "Animate") {
        vir.sourceType = "Animate";
        // Vir Animate不常用，暂不解析成员
    }
    
    return vir;
}

Optional<std::pair<size_t, size_t>> CHTLJSParser::findVirDeclaration(const String& code, size_t startPos) {
    size_t virPos = code.find("Vir", startPos);
    if (virPos == String::npos) {
        return std::nullopt;
    }
    
    // 查找=
    size_t equalPos = code.find('=', virPos);
    if (equalPos == String::npos) {
        return std::nullopt;
    }
    
    // 查找源类型块的结束
    size_t afterEqual = equalPos + 1;
    while (afterEqual < code.length() && isWhitespace(code[afterEqual])) {
        afterEqual++;
    }
    
    // 查找{
    size_t bracePos = code.find('{', afterEqual);
    if (bracePos == String::npos) {
        return std::nullopt;
    }
    
    // 查找匹配的}
    size_t endBracePos = findMatchingBrace(code, bracePos);
    if (endBracePos == String::npos) {
        return std::nullopt;
    }
    
    // 查找分号
    size_t semiPos = code.find(';', endBracePos);
    if (semiPos != String::npos && semiPos < endBracePos + 20) {
        return std::make_pair(virPos, semiPos + 1);
    }
    
    return std::make_pair(virPos, endBracePos + 1);
}

} // namespace JS
} // namespace CHTL
