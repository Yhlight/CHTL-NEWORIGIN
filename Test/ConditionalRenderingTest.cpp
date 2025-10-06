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

TEST_CASE("Conditional Rendering: 'if' block applies style to parent", "[generator][conditional-rendering]") {
    std::string input = R"(
        div {
            id: header;
            style { width: 400px; }
        }
        body {
            if {
                condition: #header.width < 500px,
                background-color: red,
            }
        }
    )";
    auto generator = generateOutput(input);
    std::string expected_html = R"(
        <div id="header" style="width:400px;"></div>
        <body style="background-color:red;"></body>
    )";
    REQUIRE(normalize_html(generator.getHtml()) == normalize_html(expected_html));
}

TEST_CASE("Conditional Rendering: 'if' block with mixed content", "[generator][conditional-rendering]") {
    std::string input = R"(
        div {
            id: header;
            style { width: 600px; }
        }
        div {
            id: main;
            if {
                condition: #header.width > 500px,
                border: 1px solid black,
                p { text: "Content inside if"; }
            }
        }
    )";
    auto generator = generateOutput(input);
    std::string expected_html = R"(
        <div id="header" style="width:600px;"></div>
        <div id="main" style="border:1px solid black;">
            <p>Content inside if</p>
        </div>
    )";
    REQUIRE(normalize_html(generator.getHtml()) == normalize_html(expected_html));
}

// =================================================================================================
// Generation Tests for Conditional Rendering
// =================================================================================================

TEST_CASE("Conditional Rendering: 'if' block renders when condition is true", "[generator][conditional-rendering]") {
    std::string input = R"(
        div {
            id: header;
            style { width: 600px; }
        }
        body {
            if {
                condition: #header.width > 500px,
                p { text: "Wide screen"; }
            }
        }
    )";
    auto generator = generateOutput(input);
    std::string expected_html = R"(
        <div id="header" style="width:600px;"></div>
        <body>
            <p>Wide screen</p>
        </body>
    )";
    REQUIRE(normalize_html(generator.getHtml()) == normalize_html(expected_html));
}

TEST_CASE("Conditional Rendering: 'if' block does not render when condition is false", "[generator][conditional-rendering]") {
    std::string input = R"(
        div {
            id: header;
            style { width: 400px; }
        }
        body {
            if {
                condition: #header.width > 500px,
                p { text: "Wide screen"; }
            }
        }
    )";
    auto generator = generateOutput(input);
    std::string expected_html = R"(
        <div id="header" style="width:400px;"></div>
        <body></body>
    )";
    REQUIRE(normalize_html(generator.getHtml()) == normalize_html(expected_html));
}

TEST_CASE("Conditional Rendering: 'if-else' chain renders 'if' block when true", "[generator][conditional-rendering]") {
    std::string input = R"(
        div {
            id: header;
            style { width: 600px; }
        }
        body {
            if {
                condition: #header.width > 500px,
                p { text: "Wide screen"; }
            }
            else {
                p { text: "Narrow screen"; }
            }
        }
    )";
    auto generator = generateOutput(input);
    std::string expected_html = R"(
        <div id="header" style="width:600px;"></div>
        <body>
            <p>Wide screen</p>
        </body>
    )";
    REQUIRE(normalize_html(generator.getHtml()) == normalize_html(expected_html));
}

TEST_CASE("Conditional Rendering: 'if-else' chain renders 'else' block when 'if' is false", "[generator][conditional-rendering]") {
    std::string input = R"(
        div {
            id: header;
            style { width: 400px; }
        }
        body {
            if {
                condition: #header.width > 500px,
                p { text: "Wide screen"; }
            }
            else {
                p { text: "Narrow screen"; }
            }
        }
    )";
    auto generator = generateOutput(input);
    std::string expected_html = R"(
        <div id="header" style="width:400px;"></div>
        <body>
            <p>Narrow screen</p>
        </body>
    )";
    REQUIRE(normalize_html(generator.getHtml()) == normalize_html(expected_html));
}

TEST_CASE("Conditional Rendering: 'if-else if-else' renders correct block", "[generator][conditional-rendering]") {
    std::string input = R"(
        div {
            id: header;
            style { width: 450px; }
        }
        body {
            if {
                condition: #header.width > 500px,
                p { text: "Wide screen"; }
            }
            else if {
                condition: #header.width > 400px,
                p { text: "Medium screen"; }
            }
            else {
                p { text: "Narrow screen"; }
            }
        }
    )";
    auto generator = generateOutput(input);
    std::string expected_html = R"(
        <div id="header" style="width:450px;"></div>
        <body>
            <p>Medium screen</p>
        </body>
    )";
    REQUIRE(normalize_html(generator.getHtml()) == normalize_html(expected_html));
}

TEST_CASE("Conditional Rendering: 'if' block with invalid condition is not rendered", "[generator][conditional-rendering]") {
    std::string input = R"(
        body {
            if {
                condition: some invalid condition,
                p { text: "Should not appear"; }
            }
        }
    )";
    auto generator = generateOutput(input);
    std::string expected_html = R"(
        <body></body>
    )";
    REQUIRE(normalize_html(generator.getHtml()) == normalize_html(expected_html));
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