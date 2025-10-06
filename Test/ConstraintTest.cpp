#include "TestUtils.h"
#include <stdexcept>

// =================================================================================================
// Parser Tests
// =================================================================================================

TEST_CASE("Parser handles basic 'except' with a tag name", "[parser][constraint]") {
    std::string input = R"(div { except span; })";
    REQUIRE_NOTHROW(generateOutput(input));
}

TEST_CASE("Parser handles multiple tag names in 'except'", "[parser][constraint]") {
    std::string input = R"(div { except span, p, a; })";
    REQUIRE_NOTHROW(generateOutput(input));
}

TEST_CASE("Parser handles 'except @Html'", "[parser][constraint]") {
    std::string input = R"(div { except @Html; })";
    REQUIRE_NOTHROW(generateOutput(input));
}

TEST_CASE("Parser handles 'except [Template]'", "[parser][constraint]") {
    std::string input = R"(div { except [Template]; })";
    REQUIRE_NOTHROW(generateOutput(input));
}

TEST_CASE("Parser handles 'except [Custom]'", "[parser][constraint]") {
    std::string input = R"(div { except [Custom]; })";
    REQUIRE_NOTHROW(generateOutput(input));
}

TEST_CASE("Parser handles 'except [Template] @Style'", "[parser][constraint]") {
    std::string input = R"(div { except [Template] @Style; })";
    REQUIRE_NOTHROW(generateOutput(input));
}

TEST_CASE("Parser handles named template constraint", "[parser][constraint]") {
    std::string input = R"(div { except [Template] @Element MyTemplate; })";
    REQUIRE_NOTHROW(generateOutput(input));
}

TEST_CASE("Parser handles mixed constraints", "[parser][constraint]") {
    std::string input = R"(div { except span, [Template] @Element MyTemplate, @Html; })";
    REQUIRE_NOTHROW(generateOutput(input));
}

TEST_CASE("Parser throws on missing semicolon in constraint", "[parser][constraint]") {
    std::string input = R"(div { except span })";
    REQUIRE_THROWS_AS(generateOutput(input), std::runtime_error);
}

// =================================================================================================
// Semantic Analyzer Tests
// =================================================================================================

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

TEST_CASE("SemanticAnalyzer throws on violated @Html constraint", "[semantic][constraint]") {
    std::string input = R"(
        div {
            except @Html;
            p { text { "this should not be allowed" } }
        }
    )";
    REQUIRE_THROWS_AS(generateOutput(input), std::runtime_error);
}

TEST_CASE("SemanticAnalyzer throws on violated named template constraint", "[semantic][constraint]") {
    std::string input = R"(
        [Template] @Element MyTemplate { p { text { "template content" } } }
        div {
            except [Template] @Element MyTemplate;
            @Element MyTemplate;
        }
    )";
    REQUIRE_THROWS_AS(generateOutput(input), std::runtime_error);
}

TEST_CASE("SemanticAnalyzer allows non-violated named template", "[semantic][constraint]") {
    std::string input = R"(
        [Template] @Element MyTemplate { p { text { "template content" } } }
        [Template] @Element AnotherTemplate { p { text { "another one" } } }
        div {
            except [Template] @Element MyTemplate;
            @Element AnotherTemplate;
        }
    )";
    REQUIRE_NOTHROW(generateOutput(input));
}

TEST_CASE("SemanticAnalyzer throws on violated [Template] type constraint", "[semantic][constraint]") {
    std::string input = R"(
        [Template] @Element MyTemplate { p { text { "template content" } } }
        div {
            except [Template];
            @Element MyTemplate;
        }
    )";
    REQUIRE_THROWS_AS(generateOutput(input), std::runtime_error);
}

TEST_CASE("Constraint is applied to nested children", "[semantic][constraint]") {
    std::string input = R"(
        div {
            except p;
            section {
                p { text { "this should be a violation" } }
            }
        }
    )";
    REQUIRE_THROWS_AS(generateOutput(input), std::runtime_error);
}

TEST_CASE("Constraint is correctly scoped and does not leak", "[semantic][constraint]") {
    std::string input = R"(
        body {
            div {
                except p;
                a { text { "ok" } }
            }
            p { text { "this should be fine outside the div" } }
        }
    )";
    REQUIRE_NOTHROW(generateOutput(input));
}