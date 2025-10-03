#include "CHTL/CHTLNode/ElementNode.h"
#include <sstream>

namespace CHTL {

ElementNode::ElementNode(const std::string& tagName)
    : BaseNode(NodeType::ELEMENT, tagName), tagName_(tagName), selfClosing_(false) {
}

bool ElementNode::isValid() const {
    return !tagName_.empty();
}

std::string ElementNode::getValidationError() const {
    if (tagName_.empty()) {
        return "Element tag name cannot be empty";
    }
    return "";
}

std::string ElementNode::generateHTML() const {
    std::ostringstream oss;
    oss << "<" << tagName_;
    
    // Add attributes
    for (const auto& prop : getProperties()) {
        if (prop.first != "tagName" && prop.first != "selfClosing") {
            oss << " " << prop.first << "=\"" << std::any_cast<std::string>(prop.second) << "\"";
        }
    }
    
    if (selfClosing_) {
        oss << " />";
    } else {
        oss << ">";
        
        // Add children
        for (const auto& child : getChildren()) {
            oss << child->generateHTML();
        }
        
        oss << "</" << tagName_ << ">";
    }
    
    return oss.str();
}

std::string ElementNode::generateCSS() const {
    std::ostringstream oss;
    for (const auto& child : getChildren()) {
        std::string css = child->generateCSS();
        if (!css.empty()) {
            oss << css << "\n";
        }
    }
    return oss.str();
}

std::string ElementNode::generateJS() const {
    std::ostringstream oss;
    for (const auto& child : getChildren()) {
        std::string js = child->generateJS();
        if (!js.empty()) {
            oss << js << "\n";
        }
    }
    return oss.str();
}

std::unique_ptr<BaseNode> ElementNode::clone() const {
    auto cloned = std::make_unique<ElementNode>(tagName_);
    cloned->setSelfClosing(selfClosing_);
    
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

void ElementNode::setTagName(const std::string& tagName) {
    tagName_ = tagName;
    setName(tagName);
}

const std::string& ElementNode::getTagName() const {
    return tagName_;
}

void ElementNode::setSelfClosing(bool selfClosing) {
    selfClosing_ = selfClosing;
}

bool ElementNode::isSelfClosing() const {
    return selfClosing_;
}

} // namespace CHTL