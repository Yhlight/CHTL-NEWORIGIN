#include "BaseNode.h"
#include <algorithm>
#include <sstream>

namespace CHTL {

BaseNode::BaseNode(NodeType type) 
    : type_(type), parent_(nullptr), line_(0), column_(0), position_(0), 
      visible_(true), processed_(false) {}

NodeType BaseNode::getType() const {
    return type_;
}

void BaseNode::setType(NodeType type) {
    type_ = type;
}

const std::string& BaseNode::getName() const {
    return name_;
}

void BaseNode::setName(const std::string& name) {
    name_ = name;
}

const std::string& BaseNode::getValue() const {
    return value_;
}

void BaseNode::setValue(const std::string& value) {
    value_ = value;
}

BaseNode* BaseNode::getParent() const {
    return parent_;
}

void BaseNode::setParent(BaseNode* parent) {
    parent_ = parent;
}

const std::vector<std::shared_ptr<BaseNode>>& BaseNode::getChildren() const {
    return children_;
}

void BaseNode::addChild(std::shared_ptr<BaseNode> child) {
    if (child) {
        child->setParent(this);
        children_.push_back(child);
    }
}

void BaseNode::removeChild(std::shared_ptr<BaseNode> child) {
    auto it = std::find(children_.begin(), children_.end(), child);
    if (it != children_.end()) {
        (*it)->setParent(nullptr);
        children_.erase(it);
    }
}

void BaseNode::removeChild(size_t index) {
    if (index < children_.size()) {
        children_[index]->setParent(nullptr);
        children_.erase(children_.begin() + index);
    }
}

void BaseNode::clearChildren() {
    for (auto& child : children_) {
        child->setParent(nullptr);
    }
    children_.clear();
}

const std::unordered_map<std::string, std::string>& BaseNode::getAttributes() const {
    return attributes_;
}

void BaseNode::setAttribute(const std::string& name, const std::string& value) {
    attributes_[name] = value;
}

std::string BaseNode::getAttribute(const std::string& name) const {
    auto it = attributes_.find(name);
    return (it != attributes_.end()) ? it->second : "";
}

bool BaseNode::hasAttribute(const std::string& name) const {
    return attributes_.find(name) != attributes_.end();
}

void BaseNode::removeAttribute(const std::string& name) {
    attributes_.erase(name);
}

void BaseNode::clearAttributes() {
    attributes_.clear();
}

size_t BaseNode::getLine() const {
    return line_;
}

void BaseNode::setLine(size_t line) {
    line_ = line;
}

size_t BaseNode::getColumn() const {
    return column_;
}

void BaseNode::setColumn(size_t column) {
    column_ = column;
}

size_t BaseNode::getPosition() const {
    return position_;
}

void BaseNode::setPosition(size_t position) {
    position_ = position;
}

bool BaseNode::isVisible() const {
    return visible_;
}

void BaseNode::setVisible(bool visible) {
    visible_ = visible;
}

bool BaseNode::isProcessed() const {
    return processed_;
}

void BaseNode::setProcessed(bool processed) {
    processed_ = processed;
}

std::string BaseNode::toString() const {
    std::ostringstream oss;
    oss << "BaseNode(type=" << static_cast<int>(type_) 
        << ", name=" << name_ 
        << ", value=" << value_ 
        << ", children=" << children_.size() << ")";
    return oss.str();
}

std::string BaseNode::toHTML() const {
    return toString();
}

std::string BaseNode::toCSS() const {
    return toString();
}

std::string BaseNode::toJS() const {
    return toString();
}

std::vector<std::shared_ptr<BaseNode>> BaseNode::findChildren(NodeType type) const {
    std::vector<std::shared_ptr<BaseNode>> result;
    for (const auto& child : children_) {
        if (child->getType() == type) {
            result.push_back(child);
        }
    }
    return result;
}

std::vector<std::shared_ptr<BaseNode>> BaseNode::findChildren(const std::string& name) const {
    std::vector<std::shared_ptr<BaseNode>> result;
    for (const auto& child : children_) {
        if (child->getName() == name) {
            result.push_back(child);
        }
    }
    return result;
}

std::shared_ptr<BaseNode> BaseNode::findFirstChild(NodeType type) const {
    for (const auto& child : children_) {
        if (child->getType() == type) {
            return child;
        }
    }
    return nullptr;
}

std::shared_ptr<BaseNode> BaseNode::findFirstChild(const std::string& name) const {
    for (const auto& child : children_) {
        if (child->getName() == name) {
            return child;
        }
    }
    return nullptr;
}

std::shared_ptr<BaseNode> BaseNode::clone() const {
    auto cloned = std::make_shared<BaseNode>(type_);
    cloned->name_ = name_;
    cloned->value_ = value_;
    cloned->attributes_ = attributes_;
    cloned->line_ = line_;
    cloned->column_ = column_;
    cloned->position_ = position_;
    cloned->visible_ = visible_;
    cloned->processed_ = processed_;
    
    for (const auto& child : children_) {
        cloned->addChild(child->clone());
    }
    
    return cloned;
}

bool BaseNode::equals(const BaseNode& other) const {
    if (type_ != other.type_ || name_ != other.name_ || value_ != other.value_) {
        return false;
    }
    
    if (attributes_.size() != other.attributes_.size()) {
        return false;
    }
    
    for (const auto& attr : attributes_) {
        auto it = other.attributes_.find(attr.first);
        if (it == other.attributes_.end() || it->second != attr.second) {
            return false;
        }
    }
    
    if (children_.size() != other.children_.size()) {
        return false;
    }
    
    for (size_t i = 0; i < children_.size(); ++i) {
        if (!children_[i]->equals(*other.children_[i])) {
            return false;
        }
    }
    
    return true;
}

bool BaseNode::validate() const {
    // 基础验证
    if (type_ == NodeType::UNKNOWN) {
        return false;
    }
    
    // 验证子节点
    for (const auto& child : children_) {
        if (!child->validate()) {
            return false;
        }
    }
    
    return true;
}

void BaseNode::optimize() {
    // 移除空的子节点
    children_.erase(
        std::remove_if(children_.begin(), children_.end(),
                      [](const std::shared_ptr<BaseNode>& child) {
                          return child->getType() == NodeType::TEXT && 
                                 child->getValue().empty();
                      }),
        children_.end()
    );
    
    // 优化子节点
    for (auto& child : children_) {
        child->optimize();
    }
}

size_t BaseNode::getNodeCount() const {
    size_t count = 1;
    for (const auto& child : children_) {
        count += child->getNodeCount();
    }
    return count;
}

size_t BaseNode::getDepth() const {
    size_t maxDepth = 0;
    for (const auto& child : children_) {
        maxDepth = std::max(maxDepth, child->getDepth() + 1);
    }
    return maxDepth;
}

size_t BaseNode::getChildCount() const {
    return children_.size();
}

std::string BaseNode::escapeString(const std::string& str) const {
    std::string result;
    for (char c : str) {
        switch (c) {
            case '"': result += "\\\""; break;
            case '\\': result += "\\\\"; break;
            case '\n': result += "\\n"; break;
            case '\r': result += "\\r"; break;
            case '\t': result += "\\t"; break;
            default: result += c; break;
        }
    }
    return result;
}

std::string BaseNode::unescapeString(const std::string& str) const {
    std::string result;
    for (size_t i = 0; i < str.length(); ++i) {
        if (str[i] == '\\' && i + 1 < str.length()) {
            switch (str[i + 1]) {
                case '"': result += '"'; i++; break;
                case '\\': result += '\\'; i++; break;
                case 'n': result += '\n'; i++; break;
                case 'r': result += '\r'; i++; break;
                case 't': result += '\t'; i++; break;
                default: result += str[i]; break;
            }
        } else {
            result += str[i];
        }
    }
    return result;
}

// NodeFactory实现
std::shared_ptr<BaseNode> NodeFactory::createNode(NodeType type) {
    return std::make_shared<BaseNode>(type);
}

std::shared_ptr<BaseNode> NodeFactory::createElement(const std::string& name) {
    auto node = std::make_shared<BaseNode>(NodeType::ELEMENT);
    node->setName(name);
    return node;
}

std::shared_ptr<BaseNode> NodeFactory::createText(const std::string& value) {
    auto node = std::make_shared<BaseNode>(NodeType::TEXT);
    node->setValue(value);
    return node;
}

std::shared_ptr<BaseNode> NodeFactory::createComment(const std::string& value) {
    auto node = std::make_shared<BaseNode>(NodeType::COMMENT);
    node->setValue(value);
    return node;
}

std::shared_ptr<BaseNode> NodeFactory::createTemplate(const std::string& name) {
    auto node = std::make_shared<BaseNode>(NodeType::TEMPLATE);
    node->setName(name);
    return node;
}

std::shared_ptr<BaseNode> NodeFactory::createCustom(const std::string& name) {
    auto node = std::make_shared<BaseNode>(NodeType::CUSTOM);
    node->setName(name);
    return node;
}

std::shared_ptr<BaseNode> NodeFactory::createStyle(const std::string& value) {
    auto node = std::make_shared<BaseNode>(NodeType::STYLE);
    node->setValue(value);
    return node;
}

std::shared_ptr<BaseNode> NodeFactory::createScript(const std::string& value) {
    auto node = std::make_shared<BaseNode>(NodeType::SCRIPT);
    node->setValue(value);
    return node;
}

std::shared_ptr<BaseNode> NodeFactory::createOrigin(const std::string& type, const std::string& value) {
    auto node = std::make_shared<BaseNode>(NodeType::ORIGIN);
    node->setName(type);
    node->setValue(value);
    return node;
}

std::shared_ptr<BaseNode> NodeFactory::createImport(const std::string& type, const std::string& path) {
    auto node = std::make_shared<BaseNode>(NodeType::IMPORT);
    node->setName(type);
    node->setValue(path);
    return node;
}

std::shared_ptr<BaseNode> NodeFactory::createConfig(const std::string& name) {
    auto node = std::make_shared<BaseNode>(NodeType::CONFIG);
    node->setName(name);
    return node;
}

std::shared_ptr<BaseNode> NodeFactory::createNamespace(const std::string& name) {
    auto node = std::make_shared<BaseNode>(NodeType::NAMESPACE);
    node->setName(name);
    return node;
}

std::shared_ptr<BaseNode> NodeFactory::createOperator(const std::string& op) {
    auto node = std::make_shared<BaseNode>(NodeType::OPERATOR);
    node->setName(op);
    return node;
}

} // namespace CHTL