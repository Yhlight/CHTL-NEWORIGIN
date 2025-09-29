#include "EventBindingNode.h"
#include <utility>

EventBindingNode::EventBindingNode(std::vector<std::string> event_names, std::string handler_code)
    : event_names(std::move(event_names)), handler_code(std::move(handler_code)) {}

CHTLJSNodeType EventBindingNode::getType() const {
    return CHTLJSNodeType::EventBinding;
}