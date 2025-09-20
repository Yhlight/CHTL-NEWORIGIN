#ifndef CHTL_JS_GENERATOR_H
#define CHTL_JS_GENERATOR_H

#include <string>
#include <memory>
#include "CHTLJSNode/CHTLJSNode.h"

class CHTLJSGenerator {
public:
    CHTLJSGenerator();
    ~CHTLJSGenerator() = default;
    
    // 生成JavaScript代码
    bool generate(const std::unique_ptr<CHTLJSNode>& ast, std::string& output);
    
    // 设置生成选项
    void setDebugMode(bool enabled) { debugMode_ = enabled; }
    void setIndentSize(int size) { indentSize_ = size; }
    void setMinify(bool enabled) { minify_ = enabled; }
    
private:
    // 生成选项
    bool debugMode_;
    int indentSize_;
    bool minify_;
    
    // 生成状态
    std::string output_;
    int currentIndent_;
    
    // 主要生成方法
    void generateNode(const CHTLJSNode& node);
    void generateListen(const ListenNode& node);
    void generateAnimate(const AnimateNode& node);
    void generateRouter(const RouterNode& node);
    void generateSelector(const SelectorNode& node);
    void generateEventBinding(const EventBindingNode& node);
    void generateAnimation(const AnimationNode& node);
    void generateRoute(const RouteNode& node);
    void generateVirtualObject(const VirtualObjectNode& node);
    
    // 辅助方法
    void addIndent();
    void addLine();
    void addText(const std::string& text);
    std::string escapeJS(const std::string& text);
    std::string generateSelectorJS(const std::string& selector);
    std::string generateEventJS(const std::string& eventType, const std::string& handler);
    std::string generateAnimationJS(const AnimateNode& node);
    std::string generateRouterJS(const RouterNode& node);
    
    // 错误处理
    void reportError(const std::string& message);
    
    // 生成状态
    std::vector<std::string> errors_;
};

#endif // CHTL_JS_GENERATOR_H