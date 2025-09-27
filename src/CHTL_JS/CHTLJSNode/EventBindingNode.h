#pragma once

#include "CHTLJSNode.h"
#include <string>
#include <vector>

class EventBindingNode : public CHTLJSNode {
public:
    explicit EventBindingNode(std::vector<std::string> event_names, std::string handler_code);
    CHTLJSNodeType getType() const override;

    std::vector<std::string> event_names;
    std::string handler_code;
};