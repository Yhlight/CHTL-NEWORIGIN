#pragma once

#include "StyleValue.h"
#include <string>

class ResponsiveValueNode : public StyleValue {
private:
    std::string variable_name;
    std::string unit;

public:
    ResponsiveValueNode(const std::string& var_name, const std::string& u = "")
        : variable_name(var_name), unit(u) {}

    const std::string& getVariableName() const {
        return variable_name;
    }

    const std::string& getUnit() const {
        return unit;
    }

    virtual StyleValueType getType() const override {
        // This node type can be considered a form of dynamic value.
        return StyleValueType::Dynamic;
    }

    virtual std::string toString() const override {
        return "$" + variable_name + "$" + unit;
    }

    virtual std::unique_ptr<StyleValue> clone() const override {
        return std::make_unique<ResponsiveValueNode>(variable_name, unit);
    }
};