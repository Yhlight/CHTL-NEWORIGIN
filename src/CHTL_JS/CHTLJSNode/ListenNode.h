#pragma once

#include "CHTLJSNode.h"
#include <string>
#include <map>

class ListenNode : public CHTLJSNode {
public:
    ListenNode() = default;
    CHTLJSNodeType getType() const override;

    // A map where the key is the event name (e.g., "click")
    // and the value is the raw string of the handler function.
    std::map<std::string, std::string> event_handlers;
};