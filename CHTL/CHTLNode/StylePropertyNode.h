#pragma once

#include "StyleStatementNode.h"
#include <string>

class StylePropertyNode : public StyleStatementNode {
public:
    StylePropertyNode(const std::string& property, const std::string& value)
        : property(property), value(value) {}

    std::string getProperty() const { return property; }
    std::string getValue() const { return value; }

private:
    std::string property;
    std::string value;
};
