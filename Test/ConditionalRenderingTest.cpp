#include "TestUtils.h"
#include <stdexcept>

// =================================================================================================
// Parsing Tests
// =================================================================================================

TEST_CASE("Parser handles a simple 'if' block", "[parser][conditional]") {
    std::string input = R"(
        div {
            if {
                condition: 1 > 0,
                display: block,
            }
        }
    )";
    REQUIRE_NOTHROW(generateOutput(input));
}

TEST_CASE("Parser handles an 'if-else' chain", "[parser][conditional]") {
    std::string input = R"(
        div {
            if {
                condition: 1 > 0,
                display: block,
            }
            else {
                display: none,
            }
        }
    )";
    REQUIRE_NOTHROW(generateOutput(input));
}

TEST_CASE("Parser handles an 'if-else if-else' chain", "[parser][conditional]") {
    std::string input = R"(
        div {
            if {
                condition: 1 > 2,
                display: block,
            }
            else if {
                condition: 1 > 0,
                display: flex,
            }
            else {
                display: none,
            }
        }
    )";
    REQUIRE_NOTHROW(generateOutput(input));
}

TEST_CASE("Parser throws if 'if' block is missing a condition", "[parser][conditional]") {
    std::string input = R"(
        div {
            if {
                display: block,
            }
        }
    )";
    REQUIRE_THROWS_AS(generateOutput(input), std::runtime_error);
}

TEST_CASE("Parser throws if 'else' block has a condition", "[parser][conditional]") {
    std::string input = R"(
        div {
            if { condition: 1 > 2, color: red, }
            else {
                condition: 1 > 0,
                display: none,
            }
        }
    )";
    REQUIRE_THROWS_AS(generateOutput(input), std::runtime_error);
}


// =================================================================================================
// Semantic Analysis & Generation Tests
// =================================================================================================

TEST_CASE("Generator correctly renders 'if' when condition is true", "[semantic][conditional]") {
    std::string input = R"(
        div {
            style {
                if {
                    condition: 10 > 5,
                    width: 100px,
                }
            }
        }
    )";
    auto generator = generateOutput(input);
    std::string expected_html = R"(<div style="width:100px;"></div>)";
    REQUIRE(normalize_html(generator.getHtml()) == normalize_html(expected_html));
}

TEST_CASE("Generator correctly skips 'if' when condition is false", "[semantic][conditional]") {
    std::string input = R"(
        div {
            style {
                if {
                    condition: 10 < 5,
                    width: 100px,
                }
            }
        }
    )";
    auto generator = generateOutput(input);
    std::string expected_html = R"(<div></div>)";
    REQUIRE(normalize_html(generator.getHtml()) == normalize_html(expected_html));
}

TEST_CASE("Generator correctly renders 'else' when 'if' is false", "[semantic][conditional]") {
    std::string input = R"(
        div {
            style {
                if {
                    condition: 10 < 5,
                    width: 100px,
                }
                else {
                    width: 200px,
                }
            }
        }
    )";
    auto generator = generateOutput(input);
    std::string expected_html = R"(<div style="width:200px;"></div>)";
    REQUIRE(normalize_html(generator.getHtml()) == normalize_html(expected_html));
}

TEST_CASE("Generator correctly renders 'else if' when true", "[semantic][conditional]") {
    std::string input = R"(
        div {
            style {
                if {
                    condition: 10 < 5,
                    width: 100px,
                }
                else if {
                    condition: 10 > 5,
                    width: 200px,
                }
                else {
                    width: 300px,
                }
            }
        }
    )";
    auto generator = generateOutput(input);
    std::string expected_html = R"(<div style="width:200px;"></div>)";
    REQUIRE(normalize_html(generator.getHtml()) == normalize_html(expected_html));
}

TEST_CASE("Generator correctly handles condition based on element attribute", "[semantic][conditional]") {
    std::string input = R"(
        div {
            width: 150px;
            style {
                if {
                    condition: width > 100,
                    height: 150px,
                }
                else {
                    height: 50px,
                }
            }
        }
    )";
    auto generator = generateOutput(input);
    std::string expected_html = R"(<div width="150px" style="height:150px;"></div>)";
    REQUIRE(normalize_html(generator.getHtml()) == normalize_html(expected_html));
}

TEST_CASE("Generator only evaluates the first true condition in a chain", "[semantic][conditional]") {
    std::string input = R"(
        div {
            style {
                if {
                    condition: 10 > 5,
                    width: 100px,
                }
                else if {
                    condition: 20 > 10,
                    width: 200px,
                }
            }
        }
    )";
    auto generator = generateOutput(input);
    std::string expected_html = R"(<div style="width:100px;"></div>)";
    REQUIRE(normalize_html(generator.getHtml()) == normalize_html(expected_html));
}