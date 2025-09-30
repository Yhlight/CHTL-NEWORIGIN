#include "CHTL/CHTLNode/BaseNode.h"
#include <sstream>
#include <algorithm>
#include <stdexcept>

namespace CHTL {

// BaseNode implementation
BaseNode::BaseNode(NodeType type, const std::string& name)
    : type_(type), name_(name), parent_(nullptr), line_(0), column_(0) {
}

void BaseNode::addChild(std::unique_ptr<BaseNode> child) {
    if (child) {
        child->setParent(this);
        children_.push_back(std::move(child));
    }
}

void BaseNode::removeChild(size_t index) {
    if (index < children_.size()) {
        children_.erase(children_.begin() + index);
    }
}

void BaseNode::clearChildren() {
    children_.clear();
}

BaseNode* BaseNode::getChild(size_t index) const {
    if (index < children_.size()) {
        return children_[index].get();
    }
    return nullptr;
}

void BaseNode::setProperty(const std::string& key, const std::any& value) {
    properties_[key] = value;
}

std::any BaseNode::getProperty(const std::string& key) const {
    auto it = properties_.find(key);
    return it != properties_.end() ? it->second : std::any{};
}

bool BaseNode::hasProperty(const std::string& key) const {
    return properties_.find(key) != properties_.end();
}

void BaseNode::removeProperty(const std::string& key) {
    properties_.erase(key);
}

BaseNode* BaseNode::findChild(const std::string& name) const {
    for (const auto& child : children_) {
        if (child->getName() == name) {
            return child.get();
        }
    }
    return nullptr;
}

std::vector<BaseNode*> BaseNode::findChildren(const std::string& name) const {
    std::vector<BaseNode*> result;
    for (const auto& child : children_) {
        if (child->getName() == name) {
            result.push_back(child.get());
        }
    }
    return result;
}

BaseNode* BaseNode::findDescendant(const std::string& name) const {
    for (const auto& child : children_) {
        if (child->getName() == name) {
            return child.get();
        }
        BaseNode* descendant = child->findDescendant(name);
        if (descendant) {
            return descendant;
        }
    }
    return nullptr;
}

std::vector<BaseNode*> BaseNode::findDescendants(const std::string& name) const {
    std::vector<BaseNode*> result;
    for (const auto& child : children_) {
        if (child->getName() == name) {
            result.push_back(child.get());
        }
        auto childDescendants = child->findDescendants(name);
        result.insert(result.end(), childDescendants.begin(), childDescendants.end());
    }
    return result;
}

std::string BaseNode::toString() const {
    std::ostringstream oss;
    oss << "BaseNode(" << static_cast<int>(type_) << ", \"" << name_ << "\")";
    return oss.str();
}

std::string BaseNode::toDebugString() const {
    std::ostringstream oss;
    oss << "BaseNode {\n";
    oss << "  type: " << static_cast<int>(type_) << ",\n";
    oss << "  name: \"" << name_ << "\",\n";
    oss << "  line: " << line_ << ",\n";
    oss << "  column: " << column_ << ",\n";
    oss << "  children: " << children_.size() << ",\n";
    oss << "  properties: " << properties_.size() << "\n";
    oss << "}";
    return oss.str();
}

void BaseNode::validateChildren() const {
    // Default implementation - can be overridden by derived classes
}

void BaseNode::validateProperties() const {
    // Default implementation - can be overridden by derived classes
}

bool BaseNode::isValid() const {
    // Default implementation - can be overridden by derived classes
    return true;
}

std::string BaseNode::getValidationError() const {
    // Default implementation - can be overridden by derived classes
    return "";
}

std::string BaseNode::generateHTML() const {
    // Default implementation - can be overridden by derived classes
    return "";
}

std::string BaseNode::generateCSS() const {
    // Default implementation - can be overridden by derived classes
    return "";
}

std::string BaseNode::generateJS() const {
    // Default implementation - can be overridden by derived classes
    return "";
}

std::unique_ptr<BaseNode> BaseNode::clone() const {
    // Default implementation - can be overridden by derived classes
    auto cloned = std::make_unique<BaseNode>(type_, name_);
    
    // Copy properties
    for (const auto& prop : getProperties()) {
        cloned->setProperty(prop.first, prop.second);
    }
    
    // Clone children
    for (const auto& child : getChildren()) {
        cloned->addChild(child->clone());
    }
    
    return cloned;
}

// RootNode implementation
RootNode::RootNode() : BaseNode(NodeType::ROOT, "root") {
}

bool RootNode::isValid() const {
    // Root node is always valid
    return true;
}

std::string RootNode::getValidationError() const {
    return "";
}

std::string RootNode::generateHTML() const {
    std::ostringstream oss;
    
    // Generate DOCTYPE
    std::string doctype = getDocumentType();
    if (!doctype.empty()) {
        oss << "<!DOCTYPE " << doctype << ">\n";
    }
    
    // Generate HTML structure
    for (const auto& child : getChildren()) {
        oss << child->generateHTML();
    }
    
    return oss.str();
}

std::string RootNode::generateCSS() const {
    std::ostringstream oss;
    
    for (const auto& child : getChildren()) {
        std::string css = child->generateCSS();
        if (!css.empty()) {
            oss << css << "\n";
        }
    }
    
    return oss.str();
}

std::string RootNode::generateJS() const {
    std::ostringstream oss;
    
    for (const auto& child : getChildren()) {
        std::string js = child->generateJS();
        if (!js.empty()) {
            oss << js << "\n";
        }
    }
    
    return oss.str();
}

std::unique_ptr<BaseNode> RootNode::clone() const {
    auto cloned = std::make_unique<RootNode>();
    
    // Copy properties
    for (const auto& prop : getProperties()) {
        cloned->setProperty(prop.first, prop.second);
    }
    
    // Clone children
    for (const auto& child : getChildren()) {
        cloned->addChild(child->clone());
    }
    
    return cloned;
}

std::string RootNode::getDocumentType() const {
    auto doctype = getProperty("doctype");
    if (doctype.has_value()) {
        try {
            return std::any_cast<std::string>(doctype);
        } catch (const std::bad_any_cast&) {
            return "";
        }
    }
    return "";
}

std::string RootNode::getTitle() const {
    auto title = getProperty("title");
    if (title.has_value()) {
        try {
            return std::any_cast<std::string>(title);
        } catch (const std::bad_any_cast&) {
            return "";
        }
    }
    return "";
}

std::string RootNode::getLanguage() const {
    auto lang = getProperty("language");
    if (lang.has_value()) {
        try {
            return std::any_cast<std::string>(lang);
        } catch (const std::bad_any_cast&) {
            return "";
        }
    }
    return "";
}

// NodeUtils implementation
BaseNode* NodeUtils::findNodeByType(BaseNode* root, BaseNode::NodeType type) {
    if (!root) return nullptr;
    
    if (root->getType() == type) {
        return root;
    }
    
    for (const auto& child : root->getChildren()) {
        BaseNode* found = findNodeByType(child.get(), type);
        if (found) {
            return found;
        }
    }
    
    return nullptr;
}

std::vector<BaseNode*> NodeUtils::findAllNodesByType(BaseNode* root, BaseNode::NodeType type) {
    std::vector<BaseNode*> result;
    
    if (!root) return result;
    
    if (root->getType() == type) {
        result.push_back(root);
    }
    
    for (const auto& child : root->getChildren()) {
        auto childResults = findAllNodesByType(child.get(), type);
        result.insert(result.end(), childResults.begin(), childResults.end());
    }
    
    return result;
}

BaseNode* NodeUtils::findNodeByProperty(BaseNode* root, const std::string& key, const std::any& value) {
    if (!root) return nullptr;
    
    if (root->hasProperty(key)) {
        auto rootValue = root->getProperty(key);
        if (rootValue.type() == value.type()) {
            try {
                if (rootValue.type() == typeid(std::string)) {
                    if (std::any_cast<std::string>(rootValue) == std::any_cast<std::string>(value)) {
                        return root;
                    }
                }
                // Add more type comparisons as needed
            } catch (const std::bad_any_cast&) {
                // Type mismatch, continue searching
            }
        }
    }
    
    for (const auto& child : root->getChildren()) {
        BaseNode* found = findNodeByProperty(child.get(), key, value);
        if (found) {
            return found;
        }
    }
    
    return nullptr;
}

std::vector<BaseNode*> NodeUtils::findAllNodesByProperty(BaseNode* root, const std::string& key, const std::any& value) {
    std::vector<BaseNode*> result;
    
    if (!root) return result;
    
    if (root->hasProperty(key)) {
        auto rootValue = root->getProperty(key);
        if (rootValue.type() == value.type()) {
            try {
                if (rootValue.type() == typeid(std::string)) {
                    if (std::any_cast<std::string>(rootValue) == std::any_cast<std::string>(value)) {
                        result.push_back(root);
                    }
                }
                // Add more type comparisons as needed
            } catch (const std::bad_any_cast&) {
                // Type mismatch, continue searching
            }
        }
    }
    
    for (const auto& child : root->getChildren()) {
        auto childResults = findAllNodesByProperty(child.get(), key, value);
        result.insert(result.end(), childResults.begin(), childResults.end());
    }
    
    return result;
}

void NodeUtils::removeNode(BaseNode* node) {
    if (!node || !node->getParent()) return;
    
    BaseNode* parent = node->getParent();
    for (size_t i = 0; i < parent->getChildCount(); ++i) {
        if (parent->getChild(i) == node) {
            parent->removeChild(i);
            break;
        }
    }
}

void NodeUtils::replaceNode(BaseNode* oldNode, std::unique_ptr<BaseNode> newNode) {
    if (!oldNode || !oldNode->getParent() || !newNode) return;
    
    BaseNode* parent = oldNode->getParent();
    for (size_t i = 0; i < parent->getChildCount(); ++i) {
        if (parent->getChild(i) == oldNode) {
            newNode->setParent(parent);
            parent->removeChild(i);
            parent->addChild(std::move(newNode));
            break;
        }
    }
}

void NodeUtils::insertNodeAfter(BaseNode* target, std::unique_ptr<BaseNode> newNode) {
    if (!target || !target->getParent() || !newNode) return;
    
    BaseNode* parent = target->getParent();
    newNode->setParent(parent);
    
    for (size_t i = 0; i < parent->getChildCount(); ++i) {
        if (parent->getChild(i) == target) {
            // Insert after the target
            parent->addChild(std::move(newNode));
            break;
        }
    }
}

void NodeUtils::insertNodeBefore(BaseNode* target, std::unique_ptr<BaseNode> newNode) {
    if (!target || !target->getParent() || !newNode) return;
    
    BaseNode* parent = target->getParent();
    newNode->setParent(parent);
    
    for (size_t i = 0; i < parent->getChildCount(); ++i) {
        if (parent->getChild(i) == target) {
            // Insert before the target
            parent->removeChild(i);
            parent->addChild(std::move(newNode));
            parent->addChild(std::make_unique<BaseNode>(target->getType(), target->getName()));
            break;
        }
    }
}

bool NodeUtils::validateNodeHierarchy(BaseNode* root) {
    if (!root) return false;
    
    // Check if node is valid
    if (!root->isValid()) {
        return false;
    }
    
    // Recursively validate children
    for (const auto& child : root->getChildren()) {
        if (!validateNodeHierarchy(child.get())) {
            return false;
        }
    }
    
    return true;
}

std::vector<std::string> NodeUtils::getValidationErrors(BaseNode* root) {
    std::vector<std::string> errors;
    
    if (!root) {
        errors.push_back("Root node is null");
        return errors;
    }
    
    // Check if node is valid
    if (!root->isValid()) {
        errors.push_back(root->getValidationError());
    }
    
    // Recursively check children
    for (const auto& child : root->getChildren()) {
        auto childErrors = getValidationErrors(child.get());
        errors.insert(errors.end(), childErrors.begin(), childErrors.end());
    }
    
    return errors;
}

bool NodeUtils::areNodesEqual(const BaseNode* node1, const BaseNode* node2) {
    if (!node1 || !node2) return node1 == node2;
    
    if (node1->getType() != node2->getType()) return false;
    if (node1->getName() != node2->getName()) return false;
    if (node1->getChildCount() != node2->getChildCount()) return false;
    
    // Compare children
    for (size_t i = 0; i < node1->getChildCount(); ++i) {
        if (!areNodesEqual(node1->getChild(i), node2->getChild(i))) {
            return false;
        }
    }
    
    return true;
}

bool NodeUtils::areNodeTreesEqual(const BaseNode* root1, const BaseNode* root2) {
    return areNodesEqual(root1, root2);
}

} // namespace CHTL