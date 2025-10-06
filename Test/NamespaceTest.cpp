#include "TestUtils.h"

TEST_CASE("Generator handles namespaced element and style templates", "[generator][namespace]") {
    std::string input = R"(
        [Import] @Chtl from "namespaced_templates.chtl";

        body {
            @Element BasicElement from MyTheme;

            div {
                style {
                    @Style BasicStyle from MyTheme;
                }
            }
        }
    )";

    auto generator = generateOutput(input, "Test/");

    std::string expected_html = "<body><p>Hello from BasicElement</p><div style=\"color:blue;\"></div></body>";
    std::string expected_css = "";

    REQUIRE(normalize_html(generator.getHtml()) == normalize_html(expected_html));
    REQUIRE(normalize_css(generator.getCss()) == normalize_css(expected_css));
}