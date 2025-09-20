#include "TemplateManager.h"
#include <iostream>
#include <sstream>
#include <set>

TemplateManager::TemplateManager() {
}

bool TemplateManager::registerTemplate(std::unique_ptr<TemplateNode> template_) {
    if (!template_) {
        reportError("模板为空");
        return false;
    }
    
    std::string name = template_->getTemplateName();
    if (name.empty()) {
        reportError("模板名称为空");
        return false;
    }
    
    if (templates_.find(name) != templates_.end()) {
        reportWarning("模板 " + name + " 已存在，将被覆盖");
    }
    
    templates_[name] = std::move(template_);
    return true;
}

TemplateNode* TemplateManager::findTemplate(const std::string& name) const {
    auto it = templates_.find(name);
    return (it != templates_.end()) ? it->second.get() : nullptr;
}

bool TemplateManager::useTemplate(const std::string& name, std::map<std::string, std::string>& properties) const {
    TemplateNode* template_ = findTemplate(name);
    if (!template_) {
        reportError("模板 " + name + " 不存在");
        return false;
    }
    
    // 简化的模板使用实现
    // 这里可以根据模板类型进行不同的处理
    return true;
}

bool TemplateManager::resolveInheritance() {
    // 简化的继承处理
    return true;
}

bool TemplateManager::validateTemplates() const {
    bool valid = true;
    
    // 验证模板名称唯一性
    std::set<std::string> allNames;
    
    for (const auto& pair : templates_) {
        if (allNames.find(pair.first) != allNames.end()) {
            reportError("模板名称重复: " + pair.first);
            valid = false;
        }
        allNames.insert(pair.first);
    }
    
    return valid;
}

void TemplateManager::printTemplates() const {
    std::cout << "=== 模板管理器状态 ===" << std::endl;
    std::cout << "模板数量: " << templates_.size() << std::endl;
    
    for (const auto& pair : templates_) {
        std::cout << "模板: " << pair.first << " (" << pair.second->getTemplateType() << ")" << std::endl;
    }
}

std::vector<std::string> TemplateManager::getTemplateNames() const {
    std::vector<std::string> names;
    
    for (const auto& pair : templates_) {
        names.push_back(pair.first);
    }
    
    return names;
}

void TemplateManager::reportError(const std::string& message) const {
    std::cerr << "模板管理器错误: " << message << std::endl;
}

void TemplateManager::reportWarning(const std::string& message) const {
    std::cout << "模板管理器警告: " << message << std::endl;
}