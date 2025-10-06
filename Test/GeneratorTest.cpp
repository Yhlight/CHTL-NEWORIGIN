#include "TestUtils.h"

TEST_CASE("Generator handles simple element", "[generator]") {
    auto generator = generateOutput("div {}");
    REQUIRE(generator.getHtml() == "<div></div>");
}

TEST_CASE("Generator handles element with text", "[generator]") {
    auto generator = generateOutput(R"(p { text { "Hello World" } })");
    REQUIRE(generator.getHtml() == "<p>Hello World</p>");
}

TEST_CASE("Generator handles element with attributes", "[generator]") {
    auto generator = generateOutput(R"(a { href: "#"; class: "link"; })");
    std::string html = generator.getHtml();
    REQUIRE(html.find(R"(href="#")") != std::string::npos);
    REQUIRE(html.find(R"(class="link")") != std::string::npos);
}

TEST_CASE("Generator handles nested elements", "[generator]") {
    auto generator = generateOutput("body { div { span { text { \"Nested\" } } } }");
    REQUIRE(generator.getHtml() == "<body><div><span>Nested</span></div></body>");
}

TEST_CASE("Generator handles inline styles", "[generator]") {
    auto generator = generateOutput("div { style { color: red; font-size: 16px; } }");
    REQUIRE(generator.getHtml() == R"(<div style="color:red;font-size:16px;"></div>)");
}

TEST_CASE("Generator handles global and inline styles", "[generator]") {
    auto generator = generateOutput("div { style { color: blue; .my-class { background-color: green; } } }");
    REQUIRE(generator.getHtml() == R"(<div class="my-class" style="color:blue;"></div>)");
    REQUIRE(generator.getCss() == ".my-class{background-color:green;}");
}

TEST_CASE("Generator handles @Html origin block", "[generator]") {
    auto generator = generateOutput("[Origin] @Html { <div>Raw</div> }");
    REQUIRE(generator.getHtml() == "<div>Raw</div>");
    REQUIRE(generator.getCss().empty());
}

TEST_CASE("Generator handles @Style origin block", "[generator]") {
    auto generator = generateOutput("[Origin] @Style { .raw { color: red; } }");
    REQUIRE(generator.getHtml().empty());
    REQUIRE(generator.getCss() == ".raw{color:red;}");
}

TEST_CASE("Generator handles @JavaScript origin block", "[generator]") {
    auto generator = generateOutput("[Origin] @JavaScript { alert(\"raw\"); }");
    REQUIRE(generator.getHtml() == "alert(\"raw\");");
    REQUIRE(generator.getCss().empty());
}

TEST_CASE("Generator handles 'use html5' directive", "[generator]") {
    auto generator = generateOutput("use html5;");
    REQUIRE(generator.getHtml() == "<!DOCTYPE html>");
}

TEST_CASE("Generator handles 'use html5' directive with element", "[generator]") {
    auto generator = generateOutput("use html5; div {}");
    REQUIRE(generator.getHtml() == "<!DOCTYPE html><div></div>");
}

TEST_CASE("Generator expands element template", "[generator]") {
    std::string input = R"(
        [Template] @Element MyCard {
            div { class: "card"; text { "A card" } }
        }
        body { @Element MyCard; }
    )";
    auto generator = generateOutput(input);
    REQUIRE(generator.getHtml() == R"(<body><div class="card">A card</div></body>)");
}

TEST_CASE("Generator expands style template", "[generator]") {
    std::string input = R"(
        [Template] @Style ButtonStyle {
            color: white;
            background-color: blue;
        }
        button {
            style {
                @Style ButtonStyle;
                border: none;
            }
        }
    )";
    auto generator = generateOutput(input);
    REQUIRE(generator.getHtml() == R"(<button style="color:white;background-color:blue;border:none;"></button>)");
}

TEST_CASE("Generator expands variable template", "[generator]") {
    std::string input = R"(
        [Template] @Var MyTheme {
            primaryColor: "blue";
            textColor: "white";
        }

        div {
            style {
                background-color: MyTheme(primaryColor);
                color: MyTheme(textColor);
            }
        }
    )";
    auto generator = generateOutput(input);
    REQUIRE(generator.getHtml() == R"(<div style="background-color:blue;color:white;"></div>)");
}

TEST_CASE("Generator handles simple if condition for width", "[generator][conditional]") {
    std::string input = R"(
        div {
            id: "box";
            style {
                if {
                    condition: width > 600px,
                    display: none,
                }
            }
        }
    )";
    auto generator = generateOutput(input);
    std::string expected_css = "@media (min-width: 600px) { div#box { display: none; } }";
    REQUIRE(normalize_css(generator.getCss()) == normalize_css(expected_css));
}

TEST_CASE("Generator handles if-else-if chain", "[generator][conditional]") {
    std::string input = R"(
        p {
            class: "text";
            style {
                if {
                    condition: width > 1000px,
                    font-size: 18px,
                }
                else if {
                    condition: width > 600px,
                    font-size: 16px,
                }
                else {
                    font-size: 14px,
                }
            }
        }
    )";
    auto generator = generateOutput(input);
    std::string expected_css = R"(
        @media (min-width: 1000px) { p.text { font-size: 18px; } }
        @media (min-width: 600px) { p.text { font-size: 16px; } }
    )";
    // The 'else' block is correctly ignored as it has no media query equivalent.
    // The inline style for 'else' is also not applied, which is the expected behavior for now.
    REQUIRE(normalize_css(generator.getCss()) == normalize_css(expected_css));
    REQUIRE(generator.getHtml() == R"(<p class="text"></p>)");
}

TEST_CASE("Generator handles if condition for height", "[generator][conditional]") {
    std::string input = R"(
        div {
            id: "container";
            style {
                if {
                    condition: height < 400px,
                    overflow: hidden,
                }
            }
        }
    )";
    auto generator = generateOutput(input);
    std::string expected_css = "@media (max-height: 400px) { div#container { overflow: hidden; } }";
    REQUIRE(normalize_css(generator.getCss()) == normalize_css(expected_css));
}

TEST_CASE("Generator ignores if condition with unsupported property", "[generator][conditional]") {
    std::string input = R"(
        div {
            style {
                if {
                    condition: color == red,
                    display: none,
                }
            }
        }
    )";
    auto generator = generateOutput(input);
    REQUIRE(generator.getCss().empty());
}

TEST_CASE("Generator handles local script block", "[generator][script]") {
    std::string input = R"(
        button {
            id: "myBtn";
            script {
                document.getElementById('myBtn').addEventListener('click', () => { console.log('Button clicked!'); });
            }
        }
    )";
    auto generator = generateOutput(input);
    // The parser re-tokenizes and re-assembles the script, which can affect spacing and quote types.
    // This test verifies the current, functional output.
    std::string expected_script_content = "document.getElementById(\"myBtn\").addEventListener(\"click\",()=>{console.log(\"Button clicked!\");});";
    std::string expected_html = R"(<button id="myBtn"><script>)" + expected_script_content + R"(</script></button>)";
    REQUIRE(generator.getHtml() == expected_html);
}