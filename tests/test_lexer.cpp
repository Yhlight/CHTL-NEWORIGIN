#include "catch.hpp"
#include "chtl/Lexer.hpp"

TEST_CASE("lexer identifiers and symbols") {
    chtl::Lexer lx("div { class: hero; }");
    auto t1 = lx.next();
    REQUIRE(t1.kind == chtl::TokenKind::Identifier);
    auto t2 = lx.next();
    REQUIRE(t2.kind == chtl::TokenKind::LBrace);
}
