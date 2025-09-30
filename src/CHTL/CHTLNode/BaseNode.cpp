#include "CHTL/CHTLNode/BaseNode.h"
#include <sstream>
#include <algorithm>

namespace CHTL {

BaseNode::BaseNode(NodeType type, const std::string& name)
    : type_(type)
    , name_(name)
    , line_(0)
    , column_(0) {
}

void BaseNode::addChild(std::shared_ptr<BaseNode> child) {
    if (child) {
        child->setParent(shared_from_this());
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

std::shared_ptr<BaseNode> BaseNode::getChild(size_t index) const {
    if (index < children_.size()) {
        return children_[index];
    }
    return nullptr;
}

void BaseNode::setAttribute(const std::string& key, const std::string& value) {
    attributes_[key] = value;
}

std::string BaseNode::getAttribute(const std::string& key) const {
    auto it = attributes_.find(key);
    if (it != attributes_.end()) {
        return it->second;
    }
    return "";
}

bool BaseNode::hasAttribute(const std::string& key) const {
    return attributes_.find(key) != attributes_.end();
}

void BaseNode::removeAttribute(const std::string& key) {
    attributes_.erase(key);
}

std::string BaseNode::toString() const {
    std::stringstream ss;
    ss << "BaseNode{type=" << static_cast<int>(type_) 
       << ", name='" << name_ 
       << "', children=" << children_.size() 
       << ", attributes=" << attributes_.size() << "}";
    return ss.str();
}

void BaseNode::accept(std::function<void(const BaseNode&)> visitor) const {
    visitor(*this);
    for (const auto& child : children_) {
        child->accept(visitor);
    }
}

std::vector<std::shared_ptr<BaseNode>> BaseNode::findNodes(NodeType type) const {
    std::vector<std::shared_ptr<BaseNode>> result;
    
    if (type_ == type) {
        result.push_back(std::const_pointer_cast<BaseNode>(shared_from_this()));
    }
    
    for (const auto& child : children_) {
        auto childResults = child->findNodes(type);
        result.insert(result.end(), childResults.begin(), childResults.end());
    }
    
    return result;
}

std::vector<std::shared_ptr<BaseNode>> BaseNode::findNodes(const std::string& name) const {
    std::vector<std::shared_ptr<BaseNode>> result;
    
    if (name_ == name) {
        result.push_back(std::const_pointer_cast<BaseNode>(shared_from_this()));
    }
    
    for (const auto& child : children_) {
        auto childResults = child->findNodes(name);
        result.insert(result.end(), childResults.begin(), childResults.end());
    }
    
    return result;
}

std::shared_ptr<BaseNode> BaseNode::findFirstNode(NodeType type) const {
    if (type_ == type) {
        return std::const_pointer_cast<BaseNode>(shared_from_this());
    }
    
    for (const auto& child : children_) {
        auto result = child->findFirstNode(type);
        if (result) {
            return result;
        }
    }
    
    return nullptr;
}

std::shared_ptr<BaseNode> BaseNode::findFirstNode(const std::string& name) const {
    if (name_ == name) {
        return std::const_pointer_cast<BaseNode>(shared_from_this());
    }
    
    for (const auto& child : children_) {
        auto result = child->findFirstNode(name);
        if (result) {
            return result;
        }
    }
    
    return nullptr;
}

void BaseNode::insertChild(size_t index, std::shared_ptr<BaseNode> child) {
    if (child && index <= children_.size()) {
        child->setParent(shared_from_this());
        children_.insert(children_.begin() + index, child);
    }
}

void BaseNode::replaceChild(size_t index, std::shared_ptr<BaseNode> newChild) {
    if (newChild && index < children_.size()) {
        children_[index]->setParent(nullptr);
        newChild->setParent(shared_from_this());
        children_[index] = newChild;
    }
}

void BaseNode::clearChildren() {
    for (auto& child : children_) {
        child->setParent(nullptr);
    }
    children_.clear();
}

size_t BaseNode::getDepth() const {
    size_t depth = 0;
    auto parent = getParent();
    while (parent) {
        depth++;
        parent = parent->getParent();
    }
    return depth;
}

std::string BaseNode::getPath() const {
    std::vector<std::string> path;
    auto current = shared_from_this();
    
    while (current) {
        path.insert(path.begin(), current->getName());
        current = current->getParent();
    }
    
    std::stringstream ss;
    for (size_t i = 0; i < path.size(); ++i) {
        if (i > 0) {
            ss << "/";
        }
        ss << path[i];
    }
    
    return ss.str();
}

std::string BaseNode::indent(size_t level) const {
    return std::string(level * 2, ' ');
}

std::string BaseNode::escapeHtml(const std::string& text) const {
    std::string result = text;
    
    // Replace & first to avoid double escaping
    size_t pos = 0;
    while ((pos = result.find("&", pos)) != std::string::npos) {
        result.replace(pos, 1, "&amp;");
        pos += 5;
    }
    
    pos = 0;
    while ((pos = result.find("<", pos)) != std::string::npos) {
        result.replace(pos, 1, "&lt;");
        pos += 4;
    }
    
    pos = 0;
    while ((pos = result.find(">", pos)) != std::string::npos) {
        result.replace(pos, 1, "&gt;");
        pos += 4;
    }
    
    pos = 0;
    while ((pos = result.find("\"", pos)) != std::string::npos) {
        result.replace(pos, 1, "&quot;");
        pos += 6;
    }
    
    pos = 0;
    while ((pos = result.find("'", pos)) != std::string::npos) {
        result.replace(pos, 1, "&#39;");
        pos += 5;
    }
    
    return result;
}

std::string BaseNode::escapeCss(const std::string& text) const {
    // CSS escaping is minimal - just handle quotes
    std::string result = text;
    
    size_t pos = 0;
    while ((pos = result.find("\"", pos)) != std::string::npos) {
        result.replace(pos, 1, "\\\"");
        pos += 2;
    }
    
    return result;
}

std::string BaseNode::escapeJs(const std::string& text) const {
    std::string result = text;
    
    // JavaScript escaping
    size_t pos = 0;
    while ((pos = result.find("\\", pos)) != std::string::npos) {
        result.replace(pos, 1, "\\\\");
        pos += 2;
    }
    
    pos = 0;
    while ((pos = result.find("\"", pos)) != std::string::npos) {
        result.replace(pos, 1, "\\\"");
        pos += 2;
    }
    
    pos = 0;
    while ((pos = result.find("\n", pos)) != std::string::npos) {
        result.replace(pos, 1, "\\n");
        pos += 2;
    }
    
    pos = 0;
    while ((pos = result.find("\r", pos)) != std::string::npos) {
        result.replace(pos, 1, "\\r");
        pos += 2;
    }
    
    pos = 0;
    while ((pos = result.find("\t", pos)) != std::string::npos) {
        result.replace(pos, 1, "\\t");
        pos += 2;
    }
    
    return result;
}

// RootNode implementation
RootNode::RootNode() : BaseNode(NodeType::ROOT, "root") {
}

std::string RootNode::toHtml() const {
    std::stringstream ss;
    
    for (const auto& child : getChildren()) {
        ss << child->toHtml();
    }
    
    return ss.str();
}

std::string RootNode::toCss() const {
    std::stringstream ss;
    
    for (const auto& child : getChildren()) {
        std::string css = child->toCss();
        if (!css.empty()) {
            ss << css << "\n";
        }
    }
    
    return ss.str();
}

std::string RootNode::toJs() const {
    std::stringstream ss;
    
    for (const auto& child : getChildren()) {
        std::string js = child->toJs();
        if (!js.empty()) {
            ss << js << "\n";
        }
    }
    
    return ss.str();
}

bool RootNode::isValid() const {
    return true; // Root node is always valid
}

} // namespace CHTL