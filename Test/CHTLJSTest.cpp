#include "TestUtils.h"
#include <stdexcept>
#include <string>
#include <algorithm>

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

TEST_CASE("Generator handles CHTL JS Animate block", "[generator][chtljs][animate]") {
    std::string input =
        "div {\n"
        "    id: \"box\";\n"
        "    script {\n"
        "        Animate {\n"
        "            target: \"#box\",\n"
        "            duration: 2000,\n"
        "            begin: {\n"
        "                opacity: \"0\";\n"
        "                transform: \"translateX(-100px)\";\n"
        "            },\n"
        "            end: {\n"
        "                opacity: \"1\";\n"
        "                transform: \"translateX(0px)\";\n"
        "            }\n"
        "        }\n"
        "    }\n"
        "}\n";

    auto generator = generateOutput(input);
    std::string actual_html = generator.getHtml();

    // The expected JS is complex, so we'll check for key parts rather than a full string match
    // This makes the test less brittle to formatting changes in the generator
    REQUIRE(actual_html.find("const targetElement = document.querySelector('#box');") != std::string::npos);
    REQUIRE(actual_html.find("const duration = 2000;") != std::string::npos);
    REQUIRE(actual_html.find("keyframes.push({ time: 0, styles: {\"opacity\": \"0\",\"transform\": \"translateX(-100px)\"} });") != std::string::npos);
    REQUIRE(actual_html.find("keyframes.push({ time: 1, styles: {\"opacity\": \"1\",\"transform\": \"translateX(0px)\"} });") != std::string::npos);
    REQUIRE(actual_html.find("requestAnimationFrame(animationStep);") != std::string::npos);
}