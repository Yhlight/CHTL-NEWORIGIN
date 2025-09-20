#ifndef TEMPLATE_MANAGER_H
#define TEMPLATE_MANAGER_H

#include <string>
#include <map>
#include <memory>
#include <vector>
#include "CHTLNode/CHTLNode.h"

class TemplateManager {
public:
    TemplateManager();
    ~TemplateManager() = default;
    
    // 模板注册
    bool registerTemplate(std::unique_ptr<TemplateNode> template_);
    
    // 模板查找
    TemplateNode* findTemplate(const std::string& name) const;
    
    // 模板使用
    bool useTemplate(const std::string& name, std::map<std::string, std::string>& properties) const;
    
    // 模板继承处理
    bool resolveInheritance();
    
    // 模板验证
    bool validateTemplates() const;
    
    // 调试信息
    void printTemplates() const;
    std::vector<std::string> getTemplateNames() const;
    
private:
    // 模板存储
    std::map<std::string, std::unique_ptr<TemplateNode>> templates_;
    
    // 错误处理
    void reportError(const std::string& message) const;
    void reportWarning(const std::string& message) const;
    
    // 错误状态
    mutable std::vector<std::string> errors_;
    mutable std::vector<std::string> warnings_;
};

#endif // TEMPLATE_MANAGER_H