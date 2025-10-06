#ifndef CHTLJS_PARSER_H
#define CHTLJS_PARSER_H

#include "../../Common.h"
#include "../CHTLJSLexer/CHTLJSToken.h"
#include "../CHTLJSLexer/CHTLJSLexer.h"

namespace CHTL {
namespace JS {

// 事件绑定结构
struct EventBinding {
    String eventName;
    String handler;
    bool isOptional = false;
    
    EventBinding() = default;
    EventBinding(const String& event, const String& h, bool opt = false)
        : eventName(event), handler(h), isOptional(opt) {}
};

// Listen块结构
struct ListenBlock {
    String target;  // 目标选择器 (可能包含{{...}})
    Vector<EventBinding> bindings;
    bool unordered = true;  // 支持无序键值对
    
    ListenBlock() = default;
    explicit ListenBlock(const String& t) : target(t) {}
};

// CHTL JS Parser配置
struct JSParserConfig {
    bool allowUnorderedKeyValues = true;
    bool allowOptionalKeyValues = true;
    bool allowUnquotedLiterals = true;
};

// CHTL JS Parser
class CHTLJSParser {
public:
    explicit CHTLJSParser(const String& source, const JSParserConfig& config = JSParserConfig());
    
    // 解析Listen块
    // 输入: "{{box}}->Listen { click: fn, hover: fn2 }"
    // 返回: ListenBlock结构
    Optional<ListenBlock> parseListenBlock(const String& code);
    
    // 解析事件绑定列表
    // 输入: "click: fn, mouseenter: fn2, mouseleave?: fn3"
    // 返回: Vector<EventBinding>
    Vector<EventBinding> parseEventBindings(const String& bindingsCode);
    
    // 解析单个事件绑定
    // 输入: "click: () => {}"
    // 返回: EventBinding
    Optional<EventBinding> parseEventBinding(const String& bindingCode);
    
    // 查找Listen块的起始位置
    // 返回: {开始位置, 结束位置} 或 nullopt
    Optional<std::pair<size_t, size_t>> findListenBlock(const String& code, size_t startPos = 0);
    
private:
    JSParserConfig config_;
    String source_;
    
    // 辅助方法
    String trimWhitespace(const String& str) const;
    bool isWhitespace(char ch) const;
    bool isIdentifierChar(char ch) const;
    
    // 提取目标选择器
    // 输入: "{{box}}->Listen" 返回 "{{box}}"
    String extractTarget(const String& code, size_t listenPos) const;
    
    // 提取块内容
    // 输入: "{ ... }" 返回 "..."
    String extractBlockContent(const String& code, size_t startPos) const;
    
    // 查找匹配的右大括号
    size_t findMatchingBrace(const String& code, size_t leftBracePos) const;
    
    // 分割事件绑定（考虑嵌套）
    Vector<String> splitBindings(const String& bindingsCode) const;
    
    // 提取函数体（处理嵌套括号和大括号）
    String extractFunctionBody(const String& code, size_t startPos, size_t& endPos) const;
};

} // namespace JS
} // namespace CHTL

#endif // CHTLJS_PARSER_H
