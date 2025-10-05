#include "TestUtils.h"
#include <stdexcept>

TEST_CASE("Parser handles simple addition in property value", "[parser][arithmetic]") {
    std::string input = R"(
        div {
            style {
                width: 100px + 50px;
            }
        }
    )";
    REQUIRE_NOTHROW(generateOutput(input));
}

TEST_CASE("Generator correctly evaluates addition", "[generator][arithmetic]") {
    std::string input = R"(
        div {
            style {
                width: 100px + 50px;
            }
        }
    )";
    auto generator = generateOutput(input);
    std::string expected_html = R"(<div style="width:150.000000px;"></div>)";
    REQUIRE(normalize_html(generator.getHtml()) == normalize_html(expected_html));
}

TEST_CASE("Generator correctly evaluates subtraction", "[generator][arithmetic]") {
    std::string input = R"(
        div {
            style {
                height: 200px - 25px;
            }
        }
    )";
    auto generator = generateOutput(input);
    std::string expected_html = R"(<div style="height:175.000000px;"></div>)";
    REQUIRE(normalize_html(generator.getHtml()) == normalize_html(expected_html));
}

TEST_CASE("Generator correctly evaluates multiplication", "[generator][arithmetic]") {
    std::string input = R"(
        div {
            style {
                font-size: 16px * 2;
            }
        }
    )";
    auto generator = generateOutput(input);
    std::string expected_html = R"(<div style="font-size:32.000000px;"></div>)";
    REQUIRE(normalize_html(generator.getHtml()) == normalize_html(expected_html));
}

TEST_CASE("Generator correctly evaluates division", "[generator][arithmetic]") {
    std::string input = R"(
        div {
            style {
                margin: 100px / 4;
            }
        }
    )";
    auto generator = generateOutput(input);
    std::string expected_html = R"(<div style="margin:25.000000px;"></div>)";
    REQUIRE(normalize_html(generator.getHtml()) == normalize_html(expected_html));
}

TEST_CASE("Generator correctly evaluates power operator", "[generator][arithmetic]") {
    std::string input = R"(
        div {
            style {
                padding: 2px ** 3;
            }
        }
    )";
    auto generator = generateOutput(input);
    std::string expected_html = R"(<div style="padding:8.000000px;"></div>)";
    REQUIRE(normalize_html(generator.getHtml()) == normalize_html(expected_html));
}

TEST_CASE("Generator correctly handles operator precedence", "[generator][arithmetic]") {
    std::string input = R"(
        div {
            style {
                width: 100px + 50px * 2;
            }
        }
    )";
    auto generator = generateOutput(input);
    std::string expected_html = R"(<div style="width:200.000000px;"></div>)";
    REQUIRE(normalize_html(generator.getHtml()) == normalize_html(expected_html));
}

TEST_CASE("Generator handles parenthesized expressions", "[generator][arithmetic]") {
    std::string input = R"(
        div {
            style {
                width: (100px + 50px) * 2;
            }
        }
    )";
    auto generator = generateOutput(input);
    std::string expected_html = R"(<div style="width:300.000000px;"></div>)";
    REQUIRE(normalize_html(generator.getHtml()) == normalize_html(expected_html));
}

TEST_CASE("Semantic analyzer throws on unit mismatch", "[semantic][arithmetic]") {
    std::string input = R"(
        div {
            style {
                width: 100px + 50s;
            }
        }
    )";
    REQUIRE_THROWS_AS(generateOutput(input), std::runtime_error);
}