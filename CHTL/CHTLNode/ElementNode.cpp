#include "ElementNode.h"

namespace CHTL {

ElementNode::ElementNode(const std::string& name) 
    : BaseNode(NodeType::ELEMENT), name_(name) {
}

void ElementNode::setAttribute(const std::string& key, const std::string& value) {
    attributes_[key] = value;
}

std::string ElementNode::getAttribute(const std::string& key) const {
    auto it = attributes_.find(key);
    return (it != attributes_.end()) ? it->second : "";
}

bool ElementNode::hasAttribute(const std::string& key) const {
    return attributes_.find(key) != attributes_.end();
}

void ElementNode::removeAttribute(const std::string& key) {
    attributes_.erase(key);
}

void ElementNode::addBehavior(std::unique_ptr<BaseNode> behavior) {
    if (behavior) {
        behaviors_.push_back(std::move(behavior));
    }
}

std::vector<BaseNode*> ElementNode::getBehaviors() const {
    std::vector<BaseNode*> result;
    for (const auto& behavior : behaviors_) {
        result.push_back(behavior.get());
    }
    return result;
}

BaseNode* ElementNode::getBehavior(const std::string& type) const {
    for (const auto& behavior : behaviors_) {
        if (behavior->getTypeName() == type) {
            return behavior.get();
        }
    }
    return nullptr;
}

bool ElementNode::isValid() const {
    return !name_.empty();
}

std::vector<std::string> ElementNode::getValidationErrors() const {
    std::vector<std::string> errors;
    if (name_.empty()) {
        errors.push_back("Element name cannot be empty");
    }
    return errors;
}

void ElementNode::accept(NodeVisitor* visitor) {
    if (visitor) {
        visitor->visit(this);
    }
}

std::string ElementNode::toString() const {
    return "ElementNode(" + name_ + ")";
}

} // namespace CHTL