#pragma once
#include "chtl/Parser.hpp"
#include "chtl/Generator.hpp"

namespace chtl {

class Compiler {
public:
    struct Result {
        std::string html;
    };

    static Result compile(std::string_view source);
};

} // namespace chtl
