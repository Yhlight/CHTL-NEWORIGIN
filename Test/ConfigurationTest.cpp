#include "TestUtils.h"
#include <stdexcept>

TEST_CASE("Parser handles [Configuration] block syntax", "[parser][config]") {
    std::string input = R"(
        [Configuration] {
            [Name] {
                style: "css";
            }
        }
    )";
    REQUIRE_NOTHROW(generateOutput(input));
}

TEST_CASE("Generator handles renamed keyword from [Configuration]", "[generator][config]") {
    std::string input = R"(
        [Configuration] {
            [Name] {
                style: "css";
            }
        }
        div {
            css {
                color: blue;
            }
        }
    )";
    auto generator = generateOutput(input);
    REQUIRE(generator.getHtml() == R"(<div style="color:blue;"></div>)");
}

TEST_CASE("Compiler works with default keywords when no config is present", "[compiler][config]") {
    std::string input = R"(
        div {
            style {
                color: green;
            }
        }
    )";
    auto generator = generateOutput(input);
    REQUIRE(generator.getHtml() == R"(<div style="color:green;"></div>)");
}