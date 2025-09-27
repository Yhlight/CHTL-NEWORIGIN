#include "RawJavaScriptNode.h"

RawJavaScriptNode::RawJavaScriptNode(std::string js_code)
    : js_code(std::move(js_code)) {}

CHTLJSNodeType RawJavaScriptNode::getType() const {
    return CHTLJSNodeType::RawJavaScript;
}