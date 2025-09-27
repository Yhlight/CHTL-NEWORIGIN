#pragma once

#include "CHTLJSNode.h"
#include <string>
#include <vector>
#include <optional>
#include <unordered_map>

class RouterNode : public CHTLJSNode {
public:
    RouterNode() = default;

    CHTLJSNodeType getType() const override {
        return CHTLJSNodeType::Router;
    }

    std::unordered_map<std::string, std::string> routes;
    std::optional<std::string> rootPath;
    std::optional<std::string> rootContainer;
    std::optional<std::string> mode;
};