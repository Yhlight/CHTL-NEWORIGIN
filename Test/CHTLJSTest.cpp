#include "TestUtils.h"
#include <stdexcept>

TEST_CASE("Generator handles basic CHTL JS selector", "[generator][chtljs]") {
    std::string input = R"(
        div {
            class: "my-div";
            script {
                {{.my-div}}.style.color = "red";
            }
        }
    )";

    auto generator = generateOutput(input);
    std::string expected_html = R"(<div class="my-div"><script>document.querySelector('.my-div').style.color = "red";</script></div>)";

    // Normalize whitespace and remove it for comparison
    std::string actual_html = generator.getHtml();
    actual_html.erase(std::remove_if(actual_html.begin(), actual_html.end(), ::isspace), actual_html.end());
    expected_html.erase(std::remove_if(expected_html.begin(), expected_html.end(), ::isspace), expected_html.end());

    REQUIRE(actual_html == expected_html);
}