#include "CHTLJSPlaceholderNode.h"

CHTLJSPlaceholderNode::CHTLJSPlaceholderNode(std::string code) : code(std::move(code)) {}

CHTLJSNodeType CHTLJSPlaceholderNode::getType() const {
    return CHTLJSNodeType::Placeholder;
}

std::string CHTLJSPlaceholderNode::getCode() const {
    return code;
}