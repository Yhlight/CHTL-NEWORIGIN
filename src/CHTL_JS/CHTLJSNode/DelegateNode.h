#pragma once

#include "CHTLJSNode.h"
#include <string>
#include <vector>
#include <map>

class DelegateNode : public CHTLJSNode {
public:
    DelegateNode() = default;
    CHTLJSNodeType getType() const override;

    // The selector(s) for the child elements to which events are delegated.
    std::vector<std::string> target_selectors;

    // A map where the key is the event name (e.g., "click")
    // and the value is the raw string of the handler function.
    std::map<std::string, std::string> event_handlers;
};