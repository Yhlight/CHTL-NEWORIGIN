#pragma once

#include <string>
#include <variant>

namespace CHTL {

struct Value {
    enum class Type {
        UNDEFINED,
        BOOL,
        NUMBER,
        STRING
    };

    Type type = Type::UNDEFINED;
    std::variant<bool, double, std::string> value;
    std::string unit; // For numbers
};

}