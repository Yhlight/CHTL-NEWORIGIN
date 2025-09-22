#pragma once

#include "StyleStatementNode.h"
#include <string>

class StylePropertyNode : public StyleStatementNode {
public:
    // Value is now optional for no-value properties
    StylePropertyNode(const std::string& property, const std::string& value = "")
        : property(property), value(value) {}

    std::string getProperty() const { return property; }
    std::string getValue() const { return value; }
    bool hasValue() const { return !value.empty(); }
    void setValue(const std::string& newValue) { value = newValue; }

private:
    std::string property;
    std::string value;
};
