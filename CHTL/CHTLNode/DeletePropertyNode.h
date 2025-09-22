#pragma once

#include "StyleStatementNode.h"
#include <vector>
#include <string>

class DeletePropertyNode : public StyleStatementNode {
public:
    DeletePropertyNode(std::vector<std::string> propertiesToDelete)
        : properties(std::move(propertiesToDelete)) {}

    const std::vector<std::string>& getProperties() const { return properties; }

private:
    std::vector<std::string> properties;
};
