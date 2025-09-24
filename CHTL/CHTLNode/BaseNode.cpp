#include "BaseNode.h"
#include "../../Util/StringUtil/StringUtil.h"
#include <sstream>
#include <algorithm>

namespace CHTL {

// BaseNode implementation
BaseNode::BaseNode(NodeType type) 
    : type_(type), parent_(nullptr) {
}

std::string BaseNode::getTypeName() const {
    switch (type_) {
        case NodeType::BASE: return "BaseNode";
        case NodeType::ELEMENT: return "ElementNode";
        case NodeType::TEXT: return "TextNode";
        case NodeType::COMMENT: return "CommentNode";
        case NodeType::TEMPLATE: return "TemplateNode";
        case NodeType::CUSTOM: return "CustomNode";
        case NodeType::STYLE: return "StyleNode";
        case NodeType::SCRIPT: return "ScriptNode";
        case NodeType::ORIGIN: return "OriginNode";
        case NodeType::IMPORT: return "ImportNode";
        case NodeType::CONFIG: return "ConfigNode";
        case NodeType::NAMESPACE: return "NamespaceNode";
        case NodeType::OPERATOR: return "OperatorNode";
        case NodeType::BEHAVIOR: return "BehaviorNode";
        case NodeType::CONDITIONAL: return "ConditionalNode";
        case NodeType::ATTRIBUTE: return "AttributeNode";
        case NodeType::EXPRESSION: return "ExpressionNode";
        case NodeType::LITERAL: return "LiteralNode";
        case NodeType::SELECTOR: return "SelectorNode";
        case NodeType::FUNCTION_CALL: return "FunctionCallNode";
        case NodeType::TEMPLATE_CALL: return "TemplateCallNode";
        case NodeType::CUSTOM_CALL: return "CustomCallNode";
        case NodeType::VAR_CALL: return "VarCallNode";
        case NodeType::ORIGIN_CALL: return "OriginCallNode";
        case NodeType::IMPORT_CALL: return "ImportCallNode";
        case NodeType::NAMESPACE_CALL: return "NamespaceCallNode";
        case NodeType::CONFIG_CALL: return "ConfigCallNode";
        default: return "UnknownNode";
    }
}

void BaseNode::setPosition(size_t line, size_t column, size_t length) {
    position_ = NodePosition(line, column, length);
}

void BaseNode::addChild(std::unique_ptr<BaseNode> child) {
    if (child) {
        child->setParent(this);
        onChildAdded(child.get());
        children_.push_back(std::move(child));
    }
}

void BaseNode::removeChild(BaseNode* child) {
    auto it = std::find_if(children_.begin(), children_.end(),
        [child](const std::unique_ptr<BaseNode>& node) {
            return node.get() == child;
        });
    
    if (it != children_.end()) {
        onChildRemoved(child);
        children_.erase(it);
    }
}

void BaseNode::clearChildren() {
    for (auto& child : children_) {
        onChildRemoved(child.get());
    }
    children_.clear();
}

BaseNode* BaseNode::getNextSibling() const {
    if (!parent_) return nullptr;
    
    auto& siblings = parent_->children_;
    auto it = std::find_if(siblings.begin(), siblings.end(),
        [this](const std::unique_ptr<BaseNode>& node) {
            return node.get() == this;
        });
    
    if (it != siblings.end() && std::next(it) != siblings.end()) {
        return std::next(it)->get();
    }
    
    return nullptr;
}

BaseNode* BaseNode::getPreviousSibling() const {
    if (!parent_) return nullptr;
    
    auto& siblings = parent_->children_;
    auto it = std::find_if(siblings.begin(), siblings.end(),
        [this](const std::unique_ptr<BaseNode>& node) {
            return node.get() == this;
        });
    
    if (it != siblings.begin()) {
        return std::prev(it)->get();
    }
    
    return nullptr;
}

size_t BaseNode::getChildIndex() const {
    if (!parent_) return 0;
    
    auto& siblings = parent_->children_;
    auto it = std::find_if(siblings.begin(), siblings.end(),
        [this](const std::unique_ptr<BaseNode>& node) {
            return node.get() == this;
        });
    
    return std::distance(siblings.begin(), it);
}

std::vector<BaseNode*> BaseNode::getDescendants() const {
    std::vector<BaseNode*> descendants;
    
    for (const auto& child : children_) {
        descendants.push_back(child.get());
        auto childDescendants = child->getDescendants();
        descendants.insert(descendants.end(), childDescendants.begin(), childDescendants.end());
    }
    
    return descendants;
}

std::vector<BaseNode*> BaseNode::getAncestors() const {
    std::vector<BaseNode*> ancestors;
    BaseNode* current = parent_;
    
    while (current) {
        ancestors.push_back(current);
        current = current->parent_;
    }
    
    return ancestors;
}

std::vector<BaseNode*> BaseNode::getSiblings() const {
    std::vector<BaseNode*> siblings;
    
    if (parent_) {
        for (const auto& child : parent_->children_) {
            if (child.get() != this) {
                siblings.push_back(child.get());
            }
        }
    }
    
    return siblings;
}

void BaseNode::setProperty(const std::string& key, const std::string& value) {
    properties_[key] = value;
    onPropertyChanged(key, value);
}

std::string BaseNode::getProperty(const std::string& key) const {
    auto it = properties_.find(key);
    return (it != properties_.end()) ? it->second : "";
}

bool BaseNode::hasProperty(const std::string& key) const {
    return properties_.find(key) != properties_.end();
}

void BaseNode::removeProperty(const std::string& key) {
    auto it = properties_.find(key);
    if (it != properties_.end()) {
        properties_.erase(it);
    }
}

void BaseNode::clearProperties() {
    properties_.clear();
}

void BaseNode::setMetadata(const std::string& key, const std::string& value) {
    metadata_[key] = value;
    onMetadataChanged(key, value);
}

std::string BaseNode::getMetadata(const std::string& key) const {
    auto it = metadata_.find(key);
    return (it != metadata_.end()) ? it->second : "";
}

bool BaseNode::hasMetadata(const std::string& key) const {
    return metadata_.find(key) != metadata_.end();
}

void BaseNode::removeMetadata(const std::string& key) {
    auto it = metadata_.find(key);
    if (it != metadata_.end()) {
        metadata_.erase(it);
    }
}

void BaseNode::clearMetadata() {
    metadata_.clear();
}

void BaseNode::accept(NodeVisitor* visitor) {
    if (visitor) {
        visitor->visit(this);
    }
}

bool BaseNode::isValid() const {
    return true; // Base implementation - always valid
}

std::vector<std::string> BaseNode::getValidationErrors() const {
    return {}; // Base implementation - no errors
}

std::unique_ptr<BaseNode> BaseNode::clone() const {
    auto cloned = std::make_unique<BaseNode>(type_);
    cloned->position_ = position_;
    cloned->properties_ = properties_;
    cloned->metadata_ = metadata_;
    
    for (const auto& child : children_) {
        cloned->addChild(child->clone());
    }
    
    return cloned;
}

std::string BaseNode::toString() const {
    std::ostringstream oss;
    oss << getTypeName();
    
    if (!properties_.empty()) {
        oss << " {";
        bool first = true;
        for (const auto& [key, value] : properties_) {
            if (!first) oss << ", ";
            oss << key << ": " << value;
            first = false;
        }
        oss << "}";
    }
    
    return oss.str();
}

std::string BaseNode::toDebugString() const {
    std::ostringstream oss;
    oss << toString();
    oss << " [line: " << position_.line << ", col: " << position_.column << "]";
    
    if (!children_.empty()) {
        oss << " children: " << children_.size();
    }
    
    return oss.str();
}

bool BaseNode::equals(const BaseNode* other) const {
    if (!other || type_ != other->type_) return false;
    
    if (properties_.size() != other->properties_.size()) return false;
    for (const auto& [key, value] : properties_) {
        auto it = other->properties_.find(key);
        if (it == other->properties_.end() || it->second != value) {
            return false;
        }
    }
    
    if (children_.size() != other->children_.size()) return false;
    for (size_t i = 0; i < children_.size(); ++i) {
        if (!children_[i]->equals(other->children_[i].get())) {
            return false;
        }
    }
    
    return true;
}

std::vector<BaseNode*> BaseNode::findNodes(NodeType type) const {
    std::vector<BaseNode*> nodes;
    
    if (type_ == type) {
        nodes.push_back(const_cast<BaseNode*>(this));
    }
    
    for (const auto& child : children_) {
        auto childNodes = child->findNodes(type);
        nodes.insert(nodes.end(), childNodes.begin(), childNodes.end());
    }
    
    return nodes;
}

std::vector<BaseNode*> BaseNode::findNodes(const std::function<bool(const BaseNode*)>& predicate) const {
    std::vector<BaseNode*> nodes;
    
    if (predicate(this)) {
        nodes.push_back(const_cast<BaseNode*>(this));
    }
    
    for (const auto& child : children_) {
        auto childNodes = child->findNodes(predicate);
        nodes.insert(nodes.end(), childNodes.begin(), childNodes.end());
    }
    
    return nodes;
}

BaseNode* BaseNode::findFirstNode(NodeType type) const {
    if (type_ == type) {
        return const_cast<BaseNode*>(this);
    }
    
    for (const auto& child : children_) {
        auto found = child->findFirstNode(type);
        if (found) return found;
    }
    
    return nullptr;
}

BaseNode* BaseNode::findFirstNode(const std::function<bool(const BaseNode*)>& predicate) const {
    if (predicate(this)) {
        return const_cast<BaseNode*>(this);
    }
    
    for (const auto& child : children_) {
        auto found = child->findFirstNode(predicate);
        if (found) return found;
    }
    
    return nullptr;
}

std::vector<BaseNode*> BaseNode::filterNodes(NodeType type) const {
    std::vector<BaseNode*> nodes;
    
    if (type_ == type) {
        nodes.push_back(const_cast<BaseNode*>(this));
    }
    
    for (const auto& child : children_) {
        auto childNodes = child->filterNodes(type);
        nodes.insert(nodes.end(), childNodes.begin(), childNodes.end());
    }
    
    return nodes;
}

std::vector<BaseNode*> BaseNode::filterNodes(const std::function<bool(const BaseNode*)>& predicate) const {
    std::vector<BaseNode*> nodes;
    
    if (predicate(this)) {
        nodes.push_back(const_cast<BaseNode*>(this));
    }
    
    for (const auto& child : children_) {
        auto childNodes = child->filterNodes(predicate);
        nodes.insert(nodes.end(), childNodes.begin(), childNodes.end());
    }
    
    return nodes;
}

size_t BaseNode::getNodeCount() const {
    size_t count = 1; // Count self
    
    for (const auto& child : children_) {
        count += child->getNodeCount();
    }
    
    return count;
}

size_t BaseNode::getNodeCount(NodeType type) const {
    size_t count = (type_ == type) ? 1 : 0;
    
    for (const auto& child : children_) {
        count += child->getNodeCount(type);
    }
    
    return count;
}

size_t BaseNode::getDepth() const {
    size_t depth = 0;
    BaseNode* current = parent_;
    
    while (current) {
        depth++;
        current = current->parent_;
    }
    
    return depth;
}

size_t BaseNode::getMaxDepth() const {
    size_t maxDepth = 0;
    
    for (const auto& child : children_) {
        maxDepth = std::max(maxDepth, child->getMaxDepth() + 1);
    }
    
    return maxDepth;
}

void BaseNode::replaceChild(BaseNode* oldChild, std::unique_ptr<BaseNode> newChild) {
    auto it = std::find_if(children_.begin(), children_.end(),
        [oldChild](const std::unique_ptr<BaseNode>& node) {
            return node.get() == oldChild;
        });
    
    if (it != children_.end()) {
        onChildRemoved(oldChild);
        if (newChild) {
            newChild->setParent(this);
            onChildAdded(newChild.get());
        }
        *it = std::move(newChild);
    }
}

void BaseNode::insertChild(size_t index, std::unique_ptr<BaseNode> child) {
    if (index > children_.size()) index = children_.size();
    
    if (child) {
        child->setParent(this);
        onChildAdded(child.get());
        children_.insert(children_.begin() + index, std::move(child));
    }
}

std::unique_ptr<BaseNode> BaseNode::removeChildAt(size_t index) {
    if (index >= children_.size()) return nullptr;
    
    auto it = children_.begin() + index;
    onChildRemoved(it->get());
    auto child = std::move(*it);
    children_.erase(it);
    
    return child;
}

std::string BaseNode::serialize() const {
    std::ostringstream oss;
    oss << "{\n";
    oss << "  \"type\": \"" << getTypeName() << "\",\n";
    oss << "  \"position\": {\n";
    oss << "    \"line\": " << position_.line << ",\n";
    oss << "    \"column\": " << position_.column << ",\n";
    oss << "    \"length\": " << position_.length << "\n";
    oss << "  },\n";
    
    if (!properties_.empty()) {
        oss << "  \"properties\": {\n";
        bool first = true;
        for (const auto& [key, value] : properties_) {
            if (!first) oss << ",\n";
            oss << "    \"" << key << "\": \"" << value << "\"";
            first = false;
        }
        oss << "\n  },\n";
    }
    
    if (!children_.empty()) {
        oss << "  \"children\": [\n";
        for (size_t i = 0; i < children_.size(); ++i) {
            if (i > 0) oss << ",\n";
            oss << "    " << children_[i]->serialize();
        }
        oss << "\n  ]\n";
    }
    
    oss << "}";
    return oss.str();
}

void BaseNode::deserialize(const std::string& data) {
    // Implementation would parse JSON and reconstruct node
    // This is a placeholder for the actual implementation
}

void BaseNode::onChildAdded(BaseNode* child) {
    // Default implementation - do nothing
}

void BaseNode::onChildRemoved(BaseNode* child) {
    // Default implementation - do nothing
}

void BaseNode::onPropertyChanged(const std::string& key, const std::string& value) {
    // Default implementation - do nothing
}

void BaseNode::onMetadataChanged(const std::string& key, const std::string& value) {
    // Default implementation - do nothing
}

// NodeBuilder implementation
NodeBuilder::NodeBuilder(NodeType type) {
    node_ = std::make_unique<BaseNode>(type);
}

NodeBuilder& NodeBuilder::position(size_t line, size_t column, size_t length) {
    node_->setPosition(line, column, length);
    return *this;
}

NodeBuilder& NodeBuilder::property(const std::string& key, const std::string& value) {
    node_->setProperty(key, value);
    return *this;
}

NodeBuilder& NodeBuilder::metadata(const std::string& key, const std::string& value) {
    node_->setMetadata(key, value);
    return *this;
}

NodeBuilder& NodeBuilder::child(std::unique_ptr<BaseNode> child) {
    node_->addChild(std::move(child));
    return *this;
}

std::unique_ptr<BaseNode> NodeBuilder::build() {
    return std::move(node_);
}

// NodeFactory implementation (forward declarations for now)
std::unique_ptr<ElementNode> NodeFactory::createElement(const std::string& name) {
    // Implementation would be in ElementNode.cpp
    return nullptr;
}

std::unique_ptr<TextNode> NodeFactory::createText(const std::string& content) {
    // Implementation would be in TextNode.cpp
    return nullptr;
}

std::unique_ptr<CommentNode> NodeFactory::createComment(const std::string& content) {
    // Implementation would be in CommentNode.cpp
    return nullptr;
}

std::unique_ptr<TemplateNode> NodeFactory::createTemplate(const std::string& type, const std::string& name) {
    // Implementation would be in TemplateNode.cpp
    return nullptr;
}

std::unique_ptr<CustomNode> NodeFactory::createCustom(const std::string& type, const std::string& name) {
    // Implementation would be in CustomNode.cpp
    return nullptr;
}

std::unique_ptr<StyleNode> NodeFactory::createStyle() {
    // Implementation would be in StyleNode.cpp
    return nullptr;
}

std::unique_ptr<ScriptNode> NodeFactory::createScript() {
    // Implementation would be in ScriptNode.cpp
    return nullptr;
}

std::unique_ptr<OriginNode> NodeFactory::createOrigin(const std::string& type, const std::string& name) {
    // Implementation would be in OriginNode.cpp
    return nullptr;
}

std::unique_ptr<ImportNode> NodeFactory::createImport(const std::string& type, const std::string& path) {
    // Implementation would be in ImportNode.cpp
    return nullptr;
}

std::unique_ptr<ConfigNode> NodeFactory::createConfig(const std::string& name) {
    // Implementation would be in ConfigNode.cpp
    return nullptr;
}

std::unique_ptr<NamespaceNode> NodeFactory::createNamespace(const std::string& name) {
    // Implementation would be in NamespaceNode.cpp
    return nullptr;
}

std::unique_ptr<OperatorNode> NodeFactory::createOperator(const std::string& op) {
    // Implementation would be in OperatorNode.cpp
    return nullptr;
}

std::unique_ptr<BehaviorNode> NodeFactory::createBehavior(const std::string& type) {
    // Implementation would be in BehaviorNode.cpp
    return nullptr;
}

std::unique_ptr<ConditionalNode> NodeFactory::createConditional(const std::string& condition) {
    // Implementation would be in ConditionalNode.cpp
    return nullptr;
}

// NodeUtils implementation
std::unique_ptr<BaseNode> NodeUtils::copyTree(const BaseNode* root) {
    if (!root) return nullptr;
    return root->clone();
}

void NodeUtils::mergeTrees(BaseNode* target, const BaseNode* source) {
    if (!target || !source) return;
    
    for (const auto& child : source->getChildren()) {
        target->addChild(child->clone());
    }
}

std::unique_ptr<BaseNode> NodeUtils::extractSubtree(BaseNode* root, BaseNode* node) {
    if (!root || !node) return nullptr;
    
    // Find the node in the tree
    auto found = root->findFirstNode([node](const BaseNode* n) { return n == node; });
    if (!found) return nullptr;
    
    // Remove from parent
    if (node->getParent()) {
        node->getParent()->removeChild(node);
    }
    
    // Return as unique_ptr (this is simplified - actual implementation would need careful memory management)
    return std::unique_ptr<BaseNode>(node);
}

std::vector<BaseNode*> NodeUtils::getAllNodes(const BaseNode* root) {
    if (!root) return {};
    
    std::vector<BaseNode*> allNodes;
    allNodes.push_back(const_cast<BaseNode*>(root));
    
    auto descendants = root->getDescendants();
    allNodes.insert(allNodes.end(), descendants.begin(), descendants.end());
    
    return allNodes;
}

std::vector<BaseNode*> NodeUtils::getNodesByType(const BaseNode* root, NodeType type) {
    if (!root) return {};
    return root->findNodes(type);
}

std::map<NodeType, size_t> NodeUtils::getNodeTypeCounts(const BaseNode* root) {
    std::map<NodeType, size_t> counts;
    
    if (!root) return counts;
    
    auto allNodes = getAllNodes(root);
    for (const auto* node : allNodes) {
        counts[node->getType()]++;
    }
    
    return counts;
}

bool NodeUtils::validateTree(const BaseNode* root) {
    if (!root) return true;
    
    return root->isValid() && std::all_of(root->getChildren().begin(), root->getChildren().end(),
        [](const std::unique_ptr<BaseNode>& child) {
            return validateTree(child.get());
        });
}

std::vector<std::string> NodeUtils::getTreeValidationErrors(const BaseNode* root) {
    std::vector<std::string> errors;
    
    if (!root) return errors;
    
    auto nodeErrors = root->getValidationErrors();
    errors.insert(errors.end(), nodeErrors.begin(), nodeErrors.end());
    
    for (const auto& child : root->getChildren()) {
        auto childErrors = getTreeValidationErrors(child.get());
        errors.insert(errors.end(), childErrors.begin(), childErrors.end());
    }
    
    return errors;
}

void NodeUtils::transformTree(BaseNode* root, std::function<std::unique_ptr<BaseNode>(BaseNode*)> transformer) {
    if (!root || !transformer) return;
    
    // Transform children first
    for (size_t i = 0; i < root->getChildren().size(); ++i) {
        auto child = root->removeChildAt(i);
        auto transformed = transformer(child.get());
        if (transformed) {
            root->insertChild(i, std::move(transformed));
            transformTree(root->getChildren()[i].get(), transformer);
        } else {
            root->insertChild(i, std::move(child));
        }
    }
}

void NodeUtils::pruneTree(BaseNode* root, std::function<bool(const BaseNode*)> predicate) {
    if (!root || !predicate) return;
    
    // Remove children that don't match predicate
    auto& children = root->getChildren();
    for (auto it = children.begin(); it != children.end();) {
        if (!predicate(it->get())) {
            it = children.erase(it);
        } else {
            pruneTree(it->get(), predicate);
            ++it;
        }
    }
}

std::string NodeUtils::serializeTree(const BaseNode* root) {
    if (!root) return "null";
    return root->serialize();
}

std::unique_ptr<BaseNode> NodeUtils::deserializeTree(const std::string& data) {
    // Implementation would parse JSON and reconstruct tree
    // This is a placeholder for the actual implementation
    return nullptr;
}

bool NodeUtils::treesEqual(const BaseNode* tree1, const BaseNode* tree2) {
    if (!tree1 && !tree2) return true;
    if (!tree1 || !tree2) return false;
    
    return tree1->equals(tree2);
}

std::vector<BaseNode*> NodeUtils::findDifferences(const BaseNode* tree1, const BaseNode* tree2) {
    std::vector<BaseNode*> differences;
    
    if (!tree1 || !tree2) {
        if (tree1) differences.push_back(const_cast<BaseNode*>(tree1));
        if (tree2) differences.push_back(const_cast<BaseNode*>(tree2));
        return differences;
    }
    
    if (!tree1->equals(tree2)) {
        differences.push_back(const_cast<BaseNode*>(tree1));
        differences.push_back(const_cast<BaseNode*>(tree2));
    }
    
    // Compare children
    auto children1 = tree1->getChildren();
    auto children2 = tree2->getChildren();
    
    size_t maxSize = std::max(children1.size(), children2.size());
    for (size_t i = 0; i < maxSize; ++i) {
        if (i >= children1.size()) {
            differences.push_back(children2[i].get());
        } else if (i >= children2.size()) {
            differences.push_back(children1[i].get());
        } else {
            auto childDifferences = findDifferences(children1[i].get(), children2[i].get());
            differences.insert(differences.end(), childDifferences.begin(), childDifferences.end());
        }
    }
    
    return differences;
}

} // namespace CHTL