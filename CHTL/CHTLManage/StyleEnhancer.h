#ifndef STYLE_ENHANCER_H
#define STYLE_ENHANCER_H

#include <string>
#include <map>
#include <vector>
#include <memory>
#include "CHTLNode/CHTLNode.h"

// 样式属性类型
enum class StylePropertyType {
    INLINE,         // 内联样式
    CLASS,          // 类选择器
    ID,             // ID选择器
    PSEUDO_CLASS,   // 伪类选择器
    PSEUDO_ELEMENT  // 伪元素选择器
};

// 样式属性
struct StyleProperty {
    std::string name;
    std::string value;
    StylePropertyType type;
    size_t line;
    size_t column;
    
    StyleProperty(const std::string& n, const std::string& v, StylePropertyType t, size_t l = 0, size_t c = 0)
        : name(n), value(v), type(t), line(l), column(c) {}
};

// 属性运算结果
struct PropertyOperationResult {
    bool success;
    std::string result;
    std::string error;
    
    PropertyOperationResult(bool s = false, const std::string& r = "", const std::string& e = "")
        : success(s), result(r), error(e) {}
};

class StyleEnhancer {
public:
    StyleEnhancer();
    ~StyleEnhancer() = default;
    
    // 处理局部样式块
    bool processStyleBlock(CHTLNode& element, const std::string& styleContent);
    
    // 自动化类名/ID添加
    bool addAutoClassId(CHTLNode& element, const std::vector<StyleProperty>& properties);
    
    // 上下文推导处理
    std::string processContextReference(const std::string& content, const CHTLNode& element);
    
    // 属性运算
    PropertyOperationResult evaluatePropertyOperation(const std::string& expression);
    
    // 引用属性处理
    std::string processPropertyReference(const std::string& reference, const CHTLNode& element);
    
    // 条件表达式处理
    std::string processConditionalExpression(const std::string& expression, const CHTLNode& element);
    
    // 生成全局样式
    std::string generateGlobalStyles() const;
    
    // 设置选项
    void setAutoAddClass(bool enabled) { autoAddClass_ = enabled; }
    void setAutoAddId(bool enabled) { autoAddId_ = enabled; }
    void setDebugMode(bool enabled) { debugMode_ = enabled; }
    
private:
    // 选项
    bool autoAddClass_;
    bool autoAddId_;
    bool debugMode_;
    
    // 全局样式存储
    std::vector<std::string> globalStyles_;
    
    // 属性运算
    PropertyOperationResult evaluateArithmeticExpression(const std::string& expression);
    PropertyOperationResult evaluateComparisonExpression(const std::string& expression);
    PropertyOperationResult evaluateLogicalExpression(const std::string& expression);
    
    // 单位处理
    std::string extractUnit(const std::string& value);
    std::string extractNumber(const std::string& value);
    bool isSameUnit(const std::string& unit1, const std::string& unit2);
    std::string convertPercentage(const std::string& value);
    
    // 字符串解耦
    std::vector<std::string> extractNumbers(const std::string& value);
    
    // 选择器处理
    std::string generateSelector(const std::string& selector, const CHTLNode& element);
    std::string resolveContextReference(const std::string& reference, const CHTLNode& element);
    
    // 辅助方法
    bool parseStyleContent(const std::string& content, std::vector<StyleProperty>& properties);
    void generateGlobalStylesFromProperties(const std::vector<StyleProperty>& properties, const CHTLNode& element);
    
    // 错误处理
    void reportError(const std::string& message, size_t line = 0, size_t column = 0);
    void reportWarning(const std::string& message, size_t line = 0, size_t column = 0);
    
    // 错误状态
    std::vector<std::string> errors_;
    std::vector<std::string> warnings_;
};

#endif // STYLE_ENHANCER_H