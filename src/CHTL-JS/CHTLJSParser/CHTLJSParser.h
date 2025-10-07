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

// 事件绑定操作符结构
struct EventBindOperator {
    String target;  // 目标选择器
    Vector<String> eventNames;  // 事件名称列表（支持多事件）
    String handler;  // 处理器
    bool isBlock = false;  // 是否是绑定块形式
    
    EventBindOperator() = default;
    EventBindOperator(const String& t, const Vector<String>& events, const String& h)
        : target(t), eventNames(events), handler(h) {}
};

// Delegate块结构
struct DelegateBlock {
    String parent;  // 父元素选择器
    Vector<String> targets;  // 要委托的目标选择器（可能是数组）
    Vector<EventBinding> eventBindings;  // 事件绑定列表
    
    DelegateBlock() = default;
    explicit DelegateBlock(const String& p) : parent(p) {}
};

// 动画关键帧结构
struct KeyFrame {
    double at;  // 时间点（0.0-1.0）
    HashMap<String, String> properties;  // CSS属性
    
    KeyFrame() : at(0.0) {}
    KeyFrame(double t) : at(t) {}
};

// Animate块结构
struct AnimateBlock {
    Vector<String> targets;  // 目标元素（可以是数组）
    int duration = 1000;  // 持续时间（ms）
    String easing = "linear";  // 缓动函数
    HashMap<String, String> begin;  // 起始状态
    Vector<KeyFrame> when;  // 中间关键帧
    HashMap<String, String> end;  // 结束状态
    int loop = 1;  // 循环次数（-1为无限循环）
    String direction = "normal";  // 播放方向
    int delay = 0;  // 延迟（ms）
    String callback;  // 完成回调
    
    AnimateBlock() = default;
};

// 路由规则结构
struct RouteRule {
    String url;      // URL路径
    String page;     // 页面选择器
    
    RouteRule() = default;
    RouteRule(const String& u, const String& p) : url(u), page(p) {}
};

// Router块结构
struct RouterBlock {
    Vector<RouteRule> routes;    // 路由规则列表
    String rootPath;             // 根路径
    String rootContainer;        // 根容器选择器
    String mode = "hash";        // 路由模式：hash或history
    
    RouterBlock() = default;
};

// Vir虚对象结构
struct VirObject {
    String name;                           // 虚对象名称
    String type;                           // 类型（Listen, Delegate, Animate, Router等）
    String sourceType;                     // 源类型（Listen, Animate等）
    HashMap<String, String> members;       // 成员映射
    bool hasVirtualObject = false;         // 是否有虚对象
    Vector<String> virtualKeys;            // 虚对象的键列表
    
    VirObject() = default;
    explicit VirObject(const String& n) : name(n) {}
};

// ScriptLoader块结构
struct ScriptLoaderBlock {
    Vector<String> scripts;                // 要加载的脚本列表
    String onload;                         // 加载完成回调
    String onerror;                        // 加载错误回调
    bool async = true;                     // 是否异步加载
    
    ScriptLoaderBlock() = default;
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
    
    // 解析事件绑定操作符 &->
    // 输入: "{{box}} &-> click: fn" 或 "{{box}} &-> click, hover: fn" 或 "{{box}} &-> { ... }"
    // 返回: EventBindOperator结构
    Optional<EventBindOperator> parseEventBindOperator(const String& code);
    
    // 解析链式绑定（包含多个&->）
    // 输入: "{{box}} &-> click: fn1, &-> hover: fn2, &-> leave: fn3;"
    // 返回: Vector<EventBindOperator>
    Vector<EventBindOperator> parseChainBinding(const String& code);
    
    // 查找事件绑定操作符的位置
    // 返回: {开始位置, 结束位置} 或 nullopt
    Optional<std::pair<size_t, size_t>> findEventBindOperator(const String& code, size_t startPos = 0);
    
    // 解析多个事件名称（逗号分隔）
    // 输入: "click, mouseenter, mouseleave"
    // 返回: Vector<String>
    Vector<String> parseEventNames(const String& namesCode);
    
    // 检查是否是事件绑定块形式 &-> { ... }
    bool isEventBindBlock(const String& code, size_t bindPos);
    
    // 解析Delegate块
    // 输入: "{{parent}}->Delegate { target: {{child}}, click: fn }"
    // 返回: DelegateBlock结构
    Optional<DelegateBlock> parseDelegateBlock(const String& code);
    
    // 查找Delegate块的位置
    Optional<std::pair<size_t, size_t>> findDelegateBlock(const String& code, size_t startPos = 0);
    
    // 解析Animate块
    // 输入: "Animate { target: {{box}}, duration: 1000, ... }"
    // 返回: AnimateBlock结构
    Optional<AnimateBlock> parseAnimateBlock(const String& code);
    
    // 查找Animate块的位置
    Optional<std::pair<size_t, size_t>> findAnimateBlock(const String& code, size_t startPos = 0);
    
    // 解析CSS属性块（用于begin/end/when）
    HashMap<String, String> parseCssProperties(const String& code);
    
    // 解析Router块
    // 输入: "Router { url: '/home', page: {{box}} }"
    // 返回: RouterBlock结构
    Optional<RouterBlock> parseRouterBlock(const String& code);
    
    // 查找Router块的位置
    Optional<std::pair<size_t, size_t>> findRouterBlock(const String& code, size_t startPos = 0);
    
    // 解析Vir虚对象声明
    // 输入: "Vir test = Listen { click: fn }"
    // 返回: VirObject结构
    Optional<VirObject> parseVirDeclaration(const String& code);
    
    // 查找Vir声明的位置
    Optional<std::pair<size_t, size_t>> findVirDeclaration(const String& code, size_t startPos = 0);
    
    // 解析ScriptLoader块
    // 输入: "ScriptLoader { src: 'lib.js', onload: callback }"
    // 返回: ScriptLoaderBlock结构
    Optional<ScriptLoaderBlock> parseScriptLoaderBlock(const String& code);
    
    // 查找ScriptLoader块的位置
    Optional<std::pair<size_t, size_t>> findScriptLoaderBlock(const String& code, size_t startPos = 0);
    
    // 辅助方法（public for generator use）
    String trimWhitespace(const String& str) const;
    String extractBlockContent(const String& code, size_t startPos) const;
    String extractTarget(const String& code, size_t pos) const;
    
private:
    JSParserConfig config_;
    String source_;
    
    // 辅助方法
    bool isWhitespace(char ch) const;
    bool isIdentifierChar(char ch) const;
    
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
