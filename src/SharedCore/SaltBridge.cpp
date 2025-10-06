#include "SaltBridge.h"
#include "../Util/StringUtil/StringUtil.h"
#include <sstream>

namespace CHTL {
namespace Bridge {

// SelectorInfo实现
String SelectorInfo::toString() const {
    std::stringstream ss;
    switch (type) {
        case SelectorType::Tag: ss << "Tag(" << value << ")"; break;
        case SelectorType::Class: ss << "Class(." << value << ")"; break;
        case SelectorType::Id: ss << "Id(#" << value << ")"; break;
        case SelectorType::Auto: ss << "Auto(" << value << ")"; break;
        case SelectorType::Descendant: ss << "Descendant(" << value << ")"; break;
        case SelectorType::Index: ss << "Index(" << value << "[" << index << "])"; break;
        case SelectorType::Ampersand: ss << "Ampersand(&)"; break;
    }
    return ss.str();
}

String SelectorInfo::toJavaScript() const {
    return EnhancedSelectorProcessor::toJavaScript(*this);
}

// SaltBridge实现
SaltBridge& SaltBridge::getInstance() {
    static SaltBridge instance;
    return instance;
}

void SaltBridge::pushContext(const ContextInfo& context) {
    contextStack_.push_back(context);
}

void SaltBridge::popContext() {
    if (!contextStack_.empty()) {
        contextStack_.pop_back();
    }
}

ContextInfo SaltBridge::getCurrentContext() const {
    if (!contextStack_.empty()) {
        return contextStack_.back();
    }
    return ContextInfo();
}

SelectorInfo SaltBridge::parseSelector(const String& selector) {
    return EnhancedSelectorProcessor::parse(selector);
}

String SaltBridge::resolveSelectorReference(const String& selector) {
    return convertEnhancedSelector(selector);
}

String SaltBridge::convertEnhancedSelector(const String& selector) {
    SelectorInfo info = parseSelector(selector);
    return info.toJavaScript();
}

String SaltBridge::resolveAmpersand(bool preferClass) {
    if (!hasContext()) {
        return "";
    }
    
    ContextInfo ctx = getCurrentContext();
    
    if (preferClass && ctx.hasClass()) {
        return "." + ctx.currentClass;
    } else if (ctx.hasId()) {
        return "#" + ctx.currentId;
    } else if (ctx.hasClass()) {
        return "." + ctx.currentClass;
    } else if (ctx.hasTag()) {
        return ctx.currentTag;
    }
    
    return "";
}

String SaltBridge::resolvePropertyReference(const String& selector, const String& property) {
    // 查找元素并返回属性值
    auto element = findElement(selector);
    if (element.has_value()) {
        auto it = element->properties.find(property);
        if (it != element->properties.end()) {
            return it->second;
        }
    }
    
    // 返回占位符
    return "/* " + selector + "." + property + " */";
}

String SaltBridge::requestCHTLJS(const String& code) {
    // CHTL请求CHTL JS处理代码
    // 这里会调用CHTL JS编译器
    return code;  // 暂时返回原样
}

Optional<String> SaltBridge::requestCHTL(const String& query) {
    // CHTL JS请求CHTL信息
    if (query == "context.class") {
        if (hasContext() && getCurrentContext().hasClass()) {
            return getCurrentContext().currentClass;
        }
    } else if (query == "context.id") {
        if (hasContext() && getCurrentContext().hasId()) {
            return getCurrentContext().currentId;
        }
    }
    
    return std::nullopt;
}

void SaltBridge::registerElement(const String& tag, const String& id, const String& className) {
    ElementInfo info;
    info.tag = tag;
    info.id = id;
    info.className = className;
    elementRegistry_.push_back(info);
}

void SaltBridge::registerElementWithProperties(const String& tag, const String& id, const String& className,
                                               const HashMap<String, String>& properties) {
    ElementInfo info;
    info.tag = tag;
    info.id = id;
    info.className = className;
    info.properties = properties;
    elementRegistry_.push_back(info);
}

Optional<String> SaltBridge::getElementProperty(const String& selector, const String& property) {
    // 查找元素
    auto elemInfo = findElement(selector);
    if (elemInfo.has_value()) {
        auto it = elemInfo->properties.find(property);
        if (it != elemInfo->properties.end()) {
            return it->second;
        }
    }
    return std::nullopt;
}

void SaltBridge::clearRegistry() {
    elementRegistry_.clear();
    contextStack_.clear();
}

Optional<ContextInfo> SaltBridge::findElement(const String& selector) {
    // 根据选择器类型查找元素
    // 自动推断：优先tag -> id -> class
    
    for (const auto& elem : elementRegistry_) {
        // .class 选择器
        if (!selector.empty() && selector[0] == '.') {
            if (selector.substr(1) == elem.className) {
                ContextInfo ctx;
                ctx.currentTag = elem.tag;
                ctx.currentId = elem.id;
                ctx.currentClass = elem.className;
                ctx.properties = elem.properties;
                return ctx;
            }
        }
        // #id 选择器  
        else if (!selector.empty() && selector[0] == '#') {
            if (selector.substr(1) == elem.id) {
                ContextInfo ctx;
                ctx.currentTag = elem.tag;
                ctx.currentId = elem.id;
                ctx.currentClass = elem.className;
                ctx.properties = elem.properties;
                return ctx;
            }
        }
        // 自动推断：tag -> id -> class
        else {
            if (selector == elem.tag ||
                selector == elem.id ||
                selector == elem.className) {
                ContextInfo ctx;
                ctx.currentTag = elem.tag;
                ctx.currentId = elem.id;
                ctx.currentClass = elem.className;
                ctx.properties = elem.properties;
                return ctx;
            }
        }
    }
    
    return std::nullopt;
}

// EnhancedSelectorProcessor实现
SelectorInfo EnhancedSelectorProcessor::parse(const String& selector) {
    using namespace Util;
    
    String trimmed = StringUtil::trim(selector);
    
    // & 引用
    if (trimmed == "&") {
        return SelectorInfo(SelectorType::Ampersand, "&");
    }
    
    // 检查是否有索引访问
    if (hasIndexAccess(trimmed)) {
        int index = extractIndex(trimmed);
        String base = trimmed.substr(0, trimmed.find('['));
        base = StringUtil::trim(base);
        
        if (isClassSelector(base)) {
            return SelectorInfo(SelectorType::Index, base.substr(1), index);
        } else if (isIdSelector(base)) {
            return SelectorInfo(SelectorType::Index, base.substr(1), index);
        } else {
            return SelectorInfo(SelectorType::Index, base, index);
        }
    }
    
    // 类选择器
    if (isClassSelector(trimmed)) {
        return SelectorInfo(SelectorType::Class, trimmed.substr(1));
    }
    
    // ID选择器
    if (isIdSelector(trimmed)) {
        return SelectorInfo(SelectorType::Id, trimmed.substr(1));
    }
    
    // 后代选择器
    if (StringUtil::contains(trimmed, " ")) {
        return SelectorInfo(SelectorType::Descendant, trimmed);
    }
    
    // 标签选择器或自动推断
    return SelectorInfo(SelectorType::Auto, trimmed);
}

String EnhancedSelectorProcessor::toJavaScript(const SelectorInfo& info) {
    std::stringstream ss;
    
    switch (info.type) {
        case SelectorType::Class:
            if (info.index >= 0) {
                ss << "document.querySelectorAll('." << info.value << "')[" << info.index << "]";
            } else {
                ss << "document.querySelector('." << info.value << "')";
            }
            break;
            
        case SelectorType::Id:
            ss << "document.getElementById('" << info.value << "')";
            break;
            
        case SelectorType::Tag:
            if (info.index >= 0) {
                ss << "document.getElementsByTagName('" << info.value << "')[" << info.index << "]";
            } else {
                ss << "document.querySelector('" << info.value << "')";
            }
            break;
            
        case SelectorType::Auto:
            // 自动推断：优先class（最常用）
            ss << "document.querySelector('." << info.value << "')";
            break;
            
        case SelectorType::Descendant:
            ss << "document.querySelector('" << info.value << "')";
            break;
            
        case SelectorType::Index:
            // 检查是否以.或#开头
            if (!info.value.empty() && info.value[0] == '.') {
                ss << "document.querySelectorAll('" << info.value << "')[" << info.index << "]";
            } else if (!info.value.empty() && info.value[0] == '#') {
                ss << "document.querySelectorAll('" << info.value << "')[" << info.index << "]";
            } else {
                ss << "document.querySelectorAll('." << info.value << "')[" << info.index << "]";
            }
            break;
            
        case SelectorType::Ampersand: {
            // & 从SaltBridge获取当前上下文
            SaltBridge& bridge = SaltBridge::getInstance();
            if (bridge.hasContext()) {
                // 从上下文获取选择器，然后递归转换
                String contextSelector = bridge.resolveAmpersand(false);  // script中优先id
                if (!contextSelector.empty()) {
                    // 解析上下文选择器并转换为JavaScript
                    SelectorInfo contextInfo = parse(contextSelector);
                    return toJavaScript(contextInfo);
                }
            }
            // 如果没有上下文，使用this
            ss << "this";
            break;
        }
    }
    
    return ss.str();
}

bool EnhancedSelectorProcessor::isClassSelector(const String& str) {
    return !str.empty() && str[0] == '.';
}

bool EnhancedSelectorProcessor::isIdSelector(const String& str) {
    return !str.empty() && str[0] == '#';
}

bool EnhancedSelectorProcessor::hasIndexAccess(const String& str) {
    return str.find('[') != String::npos && str.find(']') != String::npos;
}

int EnhancedSelectorProcessor::extractIndex(const String& str) {
    size_t start = str.find('[');
    size_t end = str.find(']');
    
    if (start != String::npos && end != String::npos && end > start + 1) {
        String indexStr = str.substr(start + 1, end - start - 1);
        try {
            return std::stoi(indexStr);
        } catch (...) {
            return -1;
        }
    }
    
    return -1;
}

} // namespace Bridge
} // namespace CHTL
