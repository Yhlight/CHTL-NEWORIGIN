#include "catch.hpp"
#include "chtl/Compiler.hpp"

TEST_CASE("compile to html") {
    auto res = chtl::Compiler::compile("div { class: hero; } Hello");
    REQUIRE(res.html.find("<div") != std::string::npos);
}