#pragma once

#include <string>
#include <memory>

enum class StyleValueType {
    Static,
    Dynamic
};

class StyleValue {
public:
    virtual ~StyleValue() = default;
    virtual StyleValueType getType() const = 0;
    virtual std::string toString() const = 0;
};