#include "chtl/Compiler.hpp"

namespace chtl {

Compiler::Result Compiler::compile(std::string_view source) {
    Lexer lx(source);
    Parser ps(std::move(lx));
    auto doc = ps.parseDocument();
    Generator gen;
    Result r;
    r.html = gen.generateHTML(doc);
    return r;
}

} // namespace chtl
