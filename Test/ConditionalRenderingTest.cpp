#include "TestUtils.h"
#include <stdexcept>

// =================================================================================================
// Parsing Tests
// =================================================================================================

TEST_CASE("Parser handles a simple 'if' block", "[parser][conditional]") {
    std::string input = R"(
        div {
            style {
                if {
                    condition: screen.width > 100px,
                    display: block,
                }
            }
        }
    )";
    REQUIRE_NOTHROW(generateOutput(input));
}

TEST_CASE("Parser handles an 'if-else' chain", "[parser][conditional]") {
    std::string input = R"(
        div {
            style {
                if {
                    condition: screen.width > 100px,
                    display: block,
                }
                else {
                    display: none,
                }
            }
        }
    )";
    REQUIRE_NOTHROW(generateOutput(input));
}

TEST_CASE("Parser handles an 'if-else if-else' chain", "[parser][conditional]") {
    std::string input = R"(
        div {
            style {
                if {
                    condition: screen.width > 200px,
                    display: block,
                }
                else if {
                    condition: screen.width > 100px,
                    display: flex,
                }
                else {
                    display: none,
                }
            }
        }
    )";
    REQUIRE_NOTHROW(generateOutput(input));
}

TEST_CASE("Parser throws if 'if' block is missing a condition", "[parser][conditional]") {
    std::string input = R"(
        div {
            style {
                if {
                    display: block,
                }
            }
        }
    )";
    REQUIRE_THROWS_AS(generateOutput(input), std::runtime_error);
}

TEST_CASE("Parser throws if 'else' block has a condition", "[parser][conditional]") {
    std::string input = R"(
        div {
            style {
                if { condition: screen.width > 100px, color: red, }
                else {
                    condition: screen.width > 50px,
                    display: none,
                }
            }
        }
    )";
    REQUIRE_THROWS_AS(generateOutput(input), std::runtime_error);
}


// =================================================================================================
// Generation Tests for Media Queries
// =================================================================================================

TEST_CASE("Generator creates correct media query for 'if' with '>'", "[generator][conditional]") {
    std::string input = R"(
        div {
            id: my-div;
            style {
                if {
                    condition: screen.width > 600px,
                    color: red,
                }
            }
        }
    )";
    auto generator = generateOutput(input);
    std::string expected_css = R"(
        @media (min-width: 600px) {
            div#my-div {
                color: red;
            }
        }
    )";
    REQUIRE(normalize_css(generator.getCss()) == normalize_css(expected_css));
}

TEST_CASE("Generator creates correct media query for 'if' with '<'", "[generator][conditional]") {
    std::string input = R"(
        div {
            class: my-div;
            style {
                if {
                    condition: screen.width < 400px,
                    font-size: 12px,
                }
            }
        }
    )";
    auto generator = generateOutput(input);
    std::string expected_css = R"(
        @media (max-width: 400px) {
            div.my-div {
                font-size: 12px;
            }
        }
    )";
    REQUIRE(normalize_css(generator.getCss()) == normalize_css(expected_css));
}

TEST_CASE("Generator handles 'else if' correctly", "[generator][conditional]") {
    std::string input = R"(
        div {
            id: responsive-box;
            style {
                if {
                    condition: screen.width > 1200px,
                    background: blue,
                }
                else if {
                    condition: screen.width > 800px,
                    background: green,
                }
            }
        }
    )";
    auto generator = generateOutput(input);
    std::string expected_css = R"(
        @media (min-width: 1200px) {
            div#responsive-box {
                background: blue;
            }
        }
        @media (min-width: 800px) {
            div#responsive-box {
                background: green;
            }
        }
    )";
    REQUIRE(normalize_css(generator.getCss()) == normalize_css(expected_css));
}

TEST_CASE("Generator ignores 'else' blocks for media queries", "[generator][conditional]") {
    std::string input = R"(
        div {
            style {
                if {
                    condition: screen.width > 600px,
                    color: red,
                }
                else {
                    color: blue,
                }
            }
        }
    )";
    auto generator = generateOutput(input);
    std::string expected_css = R"(
        @media (min-width: 600px) {
            div {
                color: red;
            }
        }
    )";
    // Note: the selector for the div is just 'div' since no id/class is provided
    REQUIRE(normalize_css(generator.getCss()) == normalize_css(expected_css));
}

TEST_CASE("Generator handles multiple properties in an 'if' block", "[generator][conditional]") {
    std::string input = R"(
        p {
            class: special;
            style {
                if {
                    condition: screen.height < 500px,
                    line-height: 1.2,
                    font-weight: bold,
                }
            }
        }
    )";
    auto generator = generateOutput(input);
    std::string expected_css = R"(
        @media (max-height: 500px) {
            p.special {
                line-height: 1.2;
                font-weight: bold;
            }
        }
    )";
    REQUIRE(normalize_css(generator.getCss()) == normalize_css(expected_css));
}