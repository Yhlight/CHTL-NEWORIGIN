#pragma once

#include "CHTLJSNode.h"
#include <string>
#include <vector>
#include <memory>

class EventBindingNode : public CHTLJSNode {
public:
    explicit EventBindingNode(std::vector<std::string> event_names, std::vector<std::unique_ptr<CHTLJSNode>> handler_body);
    CHTLJSNodeType getType() const override;

    std::vector<std::string> event_names;
    std::vector<std::unique_ptr<CHTLJSNode>> handler_body;
};