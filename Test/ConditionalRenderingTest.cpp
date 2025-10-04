#include "TestUtils.h"

TEST_CASE("Generator handles conditional rendering with 'if' block", "[generator][conditional]") {
    std::string input = R"(
        div {
            id: myDiv;
            if {
                condition: max-width: 600px;
                background-color: lightblue;
                font-size: 14px;
            }
        }
    )";

    auto generator = generateOutput(input);
    std::string expected_css = "@media screen and (max-width: 600px) {#myDiv {background-color:lightblue;font-size:14px;}}";

    std::string actual_css = generator.getCss();

    // Normalize whitespace
    actual_css.erase(std::remove_if(actual_css.begin(), actual_css.end(), ::isspace), actual_css.end());
    expected_css.erase(std::remove_if(expected_css.begin(), expected_css.end(), ::isspace), expected_css.end());

    REQUIRE(actual_css == expected_css);
}

TEST_CASE("Generator handles complex conditional rendering with 'if' block", "[generator][conditional]") {
    std::string input = R"(
        div {
            id: myDiv;
            if {
                condition: (min-width: 600px) && (max-width: 900px);
                background-color: lightgreen;
            }
        }
    )";

    auto generator = generateOutput(input);
    std::string expected_css = "@media screen and ((min-width:600px) and (max-width:900px)) {#myDiv {background-color:lightgreen;}}";

    std::string actual_css = generator.getCss();

    // Normalize whitespace
    actual_css.erase(std::remove_if(actual_css.begin(), actual_css.end(), ::isspace), actual_css.end());
    expected_css.erase(std::remove_if(expected_css.begin(), expected_css.end(), ::isspace), expected_css.end());

    REQUIRE(actual_css == expected_css);
}