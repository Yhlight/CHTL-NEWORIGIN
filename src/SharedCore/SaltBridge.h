#ifndef CHTL_SALT_BRIDGE_H
#define CHTL_SALT_BRIDGE_H

#include "../Common.h"
#include "../CHTL/CHTLNode/BaseNode.h"
#include "../CHTL-JS/CHTLJSLexer/CHTLJSToken.h"

namespace CHTL {
namespace Bridge {

// 选择器类型
enum class SelectorType {
    Tag,         // button
    Class,       // .box
    Id,          // #box
    Auto,        // box (自动推断)
    Descendant,  // .box button
    Index,       // button[0]
    Ampersand,   // &
};

// 选择器信息
struct SelectorInfo {
    SelectorType type;
    String value;
    int index = -1;
    
    SelectorInfo() : type(SelectorType::Auto) {}
    SelectorInfo(SelectorType t, const String& v, int i = -1)
        : type(t), value(v), index(i) {}
    
    String toString() const;
    String toJavaScript() const;
};

// 上下文信息
struct ContextInfo {
    String currentClass;      // 当前元素的class
    String currentId;         // 当前元素的id
    String currentTag;        // 当前元素的标签名
    HashMap<String, String> properties;  // 当前元素的属性
    
    bool hasClass() const { return !currentClass.empty(); }
    bool hasId() const { return !currentId.empty(); }
    bool hasTag() const { return !currentTag.empty(); }
};

// 盐桥 - CHTL和CHTL JS之间的通信桥梁
class SaltBridge {
public:
    static SaltBridge& getInstance();
    
    // 禁止拷贝和赋值
    SaltBridge(const SaltBridge&) = delete;
    SaltBridge& operator=(const SaltBridge&) = delete;
    
    // 上下文管理
    void pushContext(const ContextInfo& context);
    void popContext();
    ContextInfo getCurrentContext() const;
    bool hasContext() const { return !contextStack_.empty(); }
    
    // 选择器解析
    SelectorInfo parseSelector(const String& selector);
    String resolveSelectorReference(const String& selector);
    
    // 增强选择器转换
    String convertEnhancedSelector(const String& selector);
    
    // & 引用解析（在style中优先class，在script中优先id）
    String resolveAmpersand(bool preferClass = true);
    
    // 属性引用
    String resolvePropertyReference(const String& selector, const String& property);
    
    // CHTL请求CHTL JS处理
    String requestCHTLJS(const String& code);
    
    // CHTL JS请求CHTL信息
    Optional<String> requestCHTL(const String& query);
    
    // 注册元素（用于选择器查找）
    void registerElement(const String& tag, const String& id, const String& className);
    void registerElementWithProperties(const String& tag, const String& id, const String& className, 
                                       const HashMap<String, String>& properties);
    
    // 查找元素
    Optional<ContextInfo> findElement(const String& selector);
    
    // 获取元素的属性值
    Optional<String> getElementProperty(const String& selector, const String& property);
    
    // 清理注册表（测试用）
    void clearRegistry();
    
private:
    SaltBridge() = default;
    
    Vector<ContextInfo> contextStack_;
    
    // 元素注册表（用于选择器查找）
    struct ElementInfo {
        String tag;
        String id;
        String className;
        HashMap<String, String> properties;
    };
    Vector<ElementInfo> elementRegistry_;
};

// 增强选择器处理器
class EnhancedSelectorProcessor {
public:
    // 解析增强选择器
    static SelectorInfo parse(const String& selector);
    
    // 转换为JavaScript代码
    static String toJavaScript(const SelectorInfo& info);
    
private:
    static bool isClassSelector(const String& str);
    static bool isIdSelector(const String& str);
    static bool hasIndexAccess(const String& str);
    static int extractIndex(const String& str);
};

} // namespace Bridge
} // namespace CHTL

#endif // CHTL_SALT_BRIDGE_H
