#pragma once

#include "StyleValue.h"
#include <string>

class StaticStyleNode : public StyleValue {
private:
    std::string value;

public:
    explicit StaticStyleNode(const std::string& val) : value(val) {}

    virtual StyleValueType getType() const override {
        return StyleValueType::Static;
    }

    virtual std::string toString() const override {
        return value;
    }
};