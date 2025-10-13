#include "catch.hpp"
#include "chtl/Parser.hpp"

TEST_CASE("parse simple element with attr and text") {
    chtl::Lexer lx("div { class: box; } text");
    chtl::Parser ps(std::move(lx));
    auto doc = ps.parseDocument();
    REQUIRE(doc.children.size() >= 1);
}
