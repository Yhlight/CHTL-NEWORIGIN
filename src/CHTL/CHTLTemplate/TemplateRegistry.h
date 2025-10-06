#ifndef CHTL_TEMPLATE_REGISTRY_H
#define CHTL_TEMPLATE_REGISTRY_H

#include "../../Common.h"
#include "../CHTLNode/BaseNode.h"

namespace CHTL {

// 模板定义结构
struct TemplateDefinition {
    String name;
    TemplateNode::TemplateType type;
    Vector<SharedPtr<BaseNode>> children;  // 模板内容
    HashMap<String, String> variables;     // @Var变量
    
    TemplateDefinition() = default;
    TemplateDefinition(const String& n, TemplateNode::TemplateType t)
        : name(n), type(t) {}
};

// 模板注册表（单例）
class TemplateRegistry {
public:
    static TemplateRegistry& getInstance() {
        static TemplateRegistry instance;
        return instance;
    }
    
    // 注册模板
    void registerTemplate(const String& name, TemplateNode::TemplateType type,
                         const Vector<SharedPtr<BaseNode>>& children);
    
    // 注册变量组
    void registerVarTemplate(const String& name, const HashMap<String, String>& variables);
    
    // 查找模板
    Optional<TemplateDefinition> findTemplate(const String& name, TemplateNode::TemplateType type);
    
    // 查找变量组
    Optional<HashMap<String, String>> findVarTemplate(const String& name);
    
    // 检查模板是否存在
    bool hasTemplate(const String& name, TemplateNode::TemplateType type);
    
    // 清空注册表（用于测试）
    void clear();
    
    // 获取所有模板
    const HashMap<String, TemplateDefinition>& getAllTemplates() const {
        return templates_;
    }
    
private:
    TemplateRegistry() = default;
    ~TemplateRegistry() = default;
    
    // 禁止拷贝
    TemplateRegistry(const TemplateRegistry&) = delete;
    TemplateRegistry& operator=(const TemplateRegistry&) = delete;
    
    // 生成模板键名
    String makeKey(const String& name, TemplateNode::TemplateType type) const;
    
    HashMap<String, TemplateDefinition> templates_;
    HashMap<String, HashMap<String, String>> varTemplates_;
};

} // namespace CHTL

#endif // CHTL_TEMPLATE_REGISTRY_H
