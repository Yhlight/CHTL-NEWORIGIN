#include "../../../include/ASTNode.h"

namespace CHTL {

void ASTNode::addChild(std::shared_ptr<ASTNode> child) {
    if (child) {
        child->setDepth(depth + 1);
        children.push_back(child);
    }
}

void ASTNode::removeChild(std::shared_ptr<ASTNode> child) {
    children.erase(
        std::remove(children.begin(), children.end(), child),
        children.end()
    );
}

void ASTNode::insertChild(int index, std::shared_ptr<ASTNode> child) {
    if (child && index >= 0 && index <= static_cast<int>(children.size())) {
        child->setDepth(depth + 1);
        children.insert(children.begin() + index, child);
    }
}

std::shared_ptr<ASTNode> ASTNode::getChild(int index) const {
    if (index >= 0 && index < static_cast<int>(children.size())) {
        return children[index];
    }
    return nullptr;
}

size_t ASTNode::getChildCount() const {
    return children.size();
}

bool ASTNode::hasChildren() const {
    return !children.empty();
}

void ASTNode::addAttribute(const String& attr) {
    if (std::find(attributes.begin(), attributes.end(), attr) == attributes.end()) {
        attributes.push_back(attr);
    }
}

void ASTNode::removeAttribute(const String& attr) {
    attributes.erase(
        std::remove(attributes.begin(), attributes.end(), attr),
        attributes.end()
    );
}

bool ASTNode::hasAttribute(const String& attr) const {
    return std::find(attributes.begin(), attributes.end(), attr) != attributes.end();
}

void ASTNode::setMetadata(const String& key, const String& value) {
    metadata[key] = value;
}

String ASTNode::getMetadata(const String& key) const {
    auto it = metadata.find(key);
    return (it != metadata.end()) ? it->second : "";
}

bool ASTNode::hasMetadata(const String& key) const {
    return metadata.find(key) != metadata.end();
}

bool ASTNode::isLeaf() const {
    return children.empty();
}

bool ASTNode::isContainer() const {
    return !children.empty();
}

String ASTNode::toString() const {
    String result = getTypeString();
    if (!value.empty()) {
        result += "(\"" + value + "\")";
    }
    if (!attributes.empty()) {
        result += "[" + std::to_string(attributes.size()) + " attrs]";
    }
    if (!children.empty()) {
        result += "{" + std::to_string(children.size()) + " children}";
    }
    return result;
}

String ASTNode::getTypeString() const {
    static const std::unordered_map<NodeType, String> typeMap = {
        {NodeType::ELEMENT, "ELEMENT"},
        {NodeType::TEXT, "TEXT"},
        {NodeType::COMMENT, "COMMENT"},
        {NodeType::ATTRIBUTE, "ATTRIBUTE"},
        {NodeType::TEMPLATE_STYLE, "TEMPLATE_STYLE"},
        {NodeType::TEMPLATE_ELEMENT, "TEMPLATE_ELEMENT"},
        {NodeType::TEMPLATE_VAR, "TEMPLATE_VAR"},
        {NodeType::CUSTOM_STYLE, "CUSTOM_STYLE"},
        {NodeType::CUSTOM_ELEMENT, "CUSTOM_ELEMENT"},
        {NodeType::CUSTOM_VAR, "CUSTOM_VAR"},
        {NodeType::ORIGIN_HTML, "ORIGIN_HTML"},
        {NodeType::ORIGIN_STYLE, "ORIGIN_STYLE"},
        {NodeType::ORIGIN_JAVASCRIPT, "ORIGIN_JAVASCRIPT"},
        {NodeType::IMPORT_HTML, "IMPORT_HTML"},
        {NodeType::IMPORT_STYLE, "IMPORT_STYLE"},
        {NodeType::IMPORT_JAVASCRIPT, "IMPORT_JAVASCRIPT"},
        {NodeType::IMPORT_CHTL, "IMPORT_CHTL"},
        {NodeType::IMPORT_CJMOD, "IMPORT_CJMOD"},
        {NodeType::CONFIGURATION_BLOCK, "CONFIGURATION_BLOCK"},
        {NodeType::NAME_BLOCK, "NAME_BLOCK"},
        {NodeType::NAMESPACE_BLOCK, "NAMESPACE_BLOCK"},
        {NodeType::STYLE_BLOCK, "STYLE_BLOCK"},
        {NodeType::STYLE_PROPERTY, "STYLE_PROPERTY"},
        {NodeType::SCRIPT_BLOCK, "SCRIPT_BLOCK"},
        {NodeType::DELETE_OPERATOR, "DELETE_OPERATOR"},
        {NodeType::INSERT_OPERATOR, "INSERT_OPERATOR"},
        {NodeType::INHERIT_OPERATOR, "INHERIT_OPERATOR"}
    };
    
    auto it = typeMap.find(type);
    return (it != typeMap.end()) ? it->second : "UNKNOWN";
}

std::shared_ptr<ASTNode> ASTNode::findFirst(NodeType type) const {
    for (const auto& child : children) {
        if (child->getType() == type) {
            return child;
        }
        auto result = child->findFirst(type);
        if (result) {
            return result;
        }
    }
    return nullptr;
}

ASTNodeVector ASTNode::findAll(NodeType type) const {
    ASTNodeVector result;
    for (const auto& child : children) {
        if (child->getType() == type) {
            result.push_back(child);
        }
        auto childResults = child->findAll(type);
        result.insert(result.end(), childResults.begin(), childResults.end());
    }
    return result;
}

std::shared_ptr<ASTNode> ASTNode::findFirstWithValue(const String& value) const {
    for (const auto& child : children) {
        if (child->getValue() == value) {
            return child;
        }
        auto result = child->findFirstWithValue(value);
        if (result) {
            return result;
        }
    }
    return nullptr;
}

ASTNodeVector ASTNode::findAllWithValue(const String& value) const {
    ASTNodeVector result;
    for (const auto& child : children) {
        if (child->getValue() == value) {
            result.push_back(child);
        }
        auto childResults = child->findAllWithValue(value);
        result.insert(result.end(), childResults.begin(), childResults.end());
    }
    return result;
}

bool ASTNode::isValid() const {
    // Basic validation logic
    // Basic validation - no UNKNOWN type in current enum
    return true;
    
    // Validate children recursively
    for (const auto& child : children) {
        if (!child->isValid()) {
            return false;
        }
    }
    
    return true;
}

StringVector ASTNode::validate() const {
    StringVector errors;
    
    // Basic validation - no UNKNOWN type in current enum
    
    // Validate children recursively
    for (const auto& child : children) {
        auto childErrors = child->validate();
        errors.insert(errors.end(), childErrors.begin(), childErrors.end());
    }
    
    return errors;
}

// ElementNode implementation
String ElementNode::getAttribute(const String& key) const {
    auto it = attributes.find(key);
    return (it != attributes.end()) ? it->second : "";
}

bool ElementNode::hasAttribute(const String& key) const {
    return attributes.find(key) != attributes.end();
}

void ElementNode::removeAttribute(const String& key) {
    attributes.erase(key);
}

// StyleNode implementation
void StyleNode::setProperty(const String& key, const String& value) {
    properties[key] = value;
}

String StyleNode::getProperty(const String& key) const {
    auto it = properties.find(key);
    return (it != properties.end()) ? it->second : "";
}

bool StyleNode::hasProperty(const String& key) const {
    return properties.find(key) != properties.end();
}

void StyleNode::removeProperty(const String& key) {
    properties.erase(key);
}

void StyleNode::addSelector(const String& selector) {
    if (std::find(selectors.begin(), selectors.end(), selector) == selectors.end()) {
        selectors.push_back(selector);
    }
}

void StyleNode::removeSelector(const String& selector) {
    selectors.erase(
        std::remove(selectors.begin(), selectors.end(), selector),
        selectors.end()
    );
}

bool StyleNode::hasSelector(const String& selector) const {
    return std::find(selectors.begin(), selectors.end(), selector) != selectors.end();
}

// TemplateNode implementation
void TemplateNode::setParameter(const String& key, const String& value) {
    parameters[key] = value;
}

String TemplateNode::getParameter(const String& key) const {
    auto it = parameters.find(key);
    return (it != parameters.end()) ? it->second : "";
}

bool TemplateNode::hasParameter(const String& key) const {
    return parameters.find(key) != parameters.end();
}

} // namespace CHTL