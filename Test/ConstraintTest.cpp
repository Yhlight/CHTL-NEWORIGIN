#include "TestUtils.h"
#include <stdexcept>

TEST_CASE("Parser handles 'except' syntax without error", "[parser][constraint]") {
    std::string input = R"(
        div {
            except span;
        }
    )";
    REQUIRE_NOTHROW(generateOutput(input));
}

TEST_CASE("SemanticAnalyzer throws on violated tag name constraint", "[semantic][constraint]") {
    std::string input = R"(
        div {
            except span;
            span { text { "this should not be allowed" } }
        }
    )";
    REQUIRE_THROWS_AS(generateOutput(input), std::runtime_error);
}

TEST_CASE("SemanticAnalyzer allows valid child elements", "[semantic][constraint]") {
    std::string input = R"(
        div {
            except span;
            p { text { "this is fine" } }
        }
    )";
    REQUIRE_NOTHROW(generateOutput(input));
}