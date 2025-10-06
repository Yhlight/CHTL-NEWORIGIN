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

class MockSaltBridge : public CHTL::SaltBridge {
public:
    mutable bool processDynamicIfCalled = false;
    mutable std::vector<std::shared_ptr<CHTL::IfNode>> captured_if_nodes;
    mutable std::string captured_parent_selector;

    std::string processScript(const std::string& raw_script) override { return ""; }
    std::string processAnimation(const CHTL::AnimationData& data) override { return ""; }
    std::string processDynamicIf(const std::vector<std::shared_ptr<CHTL::IfNode>>& if_nodes, const std::string& parent_selector) override {
        processDynamicIfCalled = true;
        captured_if_nodes = if_nodes;
        captured_parent_selector = parent_selector;
        return "<script>/* Dynamic if processed */</script>";
    }
};

TEST_CASE("Generator handles dynamic conditional rendering", "[generator]") {
    std::string input = R"(
        div {
            id: my-div;
            style {
                if {
                    condition: {{some.value}} > 10,
                    color: red
                } else {
                    color: blue
                }
            }
        }
    )";

    auto mock_bridge = std::make_shared<MockSaltBridge>();
    auto generator = generateOutputWithBridge(input, mock_bridge.get());

    REQUIRE(mock_bridge->processDynamicIfCalled == true);
    REQUIRE(mock_bridge->captured_if_nodes.size() == 2);
    REQUIRE(mock_bridge->captured_parent_selector == "#my-div");
    REQUIRE(generator.getHtml().find("<script>/* Dynamic if processed */</script>") != std::string::npos);
}

TEST_CASE("Generator handles conditional rendering with if-else-if-else blocks", "[generator]") {
    SECTION("Test 'if' block is chosen") {
        std::string input = R"(
            html {
                width: 200px;
                body {
                    div {
                        style {
                            if {
                                condition: html.width > 100px,
                                display: block
                            } else if {
                                condition: html.width < 50px,
                                display: inline-block
                            } else {
                                display: none
                            }
                        }
                    }
                }
            }
        )";
        auto generator = generateOutput(input);
        REQUIRE(generator.getHtml() == R"(<html width="200px"><body><div style="display:block;"></div></body></html>)");
    }

    SECTION("Test 'else if' block is chosen") {
        std::string input = R"(
            html {
                width: 40px;
                body {
                    div {
                        style {
                            if {
                                condition: html.width > 100px,
                                display: block
                            } else if {
                                condition: html.width < 50px,
                                display: inline-block
                            } else {
                                display: none
                            }
                        }
                    }
                }
            }
        )";
        auto generator = generateOutput(input);
        REQUIRE(generator.getHtml() == R"(<html width="40px"><body><div style="display:inline-block;"></div></body></html>)");
    }

    SECTION("Test 'else' block is chosen") {
        std::string input = R"(
            html {
                width: 75px;
                body {
                    div {
                        style {
                            if {
                                condition: html.width > 100px,
                                display: block
                            } else if {
                                condition: html.width < 50px,
                                display: inline-block
                            } else {
                                display: none
                            }
                        }
                    }
                }
            }
        )";
        auto generator = generateOutput(input);
        REQUIRE(generator.getHtml() == R"(<html width="75px"><body><div style="display:none;"></div></body></html>)");
    }
}