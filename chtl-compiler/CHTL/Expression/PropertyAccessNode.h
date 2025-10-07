#pragma once

#include "ExprNode.h"

class PropertyAccessNode : public ExprNode {
public:
    PropertyAccessNode(const std::string& propertyName) : propertyName(propertyName) {}

    std::string evaluate(const std::map<std::string, std::string>& context) const override {
        auto it = context.find(propertyName);
        if (it != context.end()) {
            // It's a property that exists in the context, so return its value.
            return it->second;
        }
        // If it's not in the context, treat it as a literal keyword (e.g., 'red', 'bold').
        return propertyName;
    }

private:
    std::string propertyName;
};