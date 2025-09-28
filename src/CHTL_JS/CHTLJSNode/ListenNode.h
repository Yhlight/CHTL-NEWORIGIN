#pragma once

#include "CHTLJSNode.h"
#include <string>
#include <map>
#include <vector>
#include <memory>

class ListenNode : public CHTLJSNode {
public:
    ListenNode() = default;
    CHTLJSNodeType getType() const override;

    // A map where the key is the event name (e.g., "click")
    // and the value is a vector of nodes representing the handler's body.
    std::map<std::string, std::vector<std::unique_ptr<CHTLJSNode>>> event_handlers;
};