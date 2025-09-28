#include "EventBindingNode.h"
#include <utility>

EventBindingNode::EventBindingNode(std::vector<std::string> event_names, std::vector<std::unique_ptr<CHTLJSNode>> handler_body)
    : event_names(std::move(event_names)), handler_body(std::move(handler_body)) {}

CHTLJSNodeType EventBindingNode::getType() const {
    return CHTLJSNodeType::EventBinding;
}