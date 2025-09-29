#include <cassert>
#include <string>
#include <algorithm>
#include <iostream>

#include "Dispatcher/CompilerDispatcher.h"
#include "Dispatcher/CompilationResult.h"

// Helper function to remove all whitespace from a string for robust comparison
static std::string remove_whitespace(std::string str) {
    str.erase(std::remove_if(str.begin(), str.end(), ::isspace), str.end());
    return str;
}

/**
 * @brief Tests the most basic CHTL feature: an element with a text attribute.
 *
 * Verifies that the compiler can parse `div { text: "hello world"; }` and
 * generate the corresponding HTML `<div>hello world</div>`. This confirms the
 * core parser -> generator pipeline is functional.
 */
void test_basic_element_and_text_attribute() {
    std::cout << "--- Running Test: test_basic_element_and_text_attribute ---\n";
    std::string source = R"(div { text: "hello world"; })";
    CompilerDispatcher dispatcher;
    CompilationResult result = dispatcher.compile(source, "test.chtl", true);

    std::string expected_html = "<div>helloworld</div>";
    std::string actual_html_no_whitespace = remove_whitespace(result.html_content);

    assert(actual_html_no_whitespace == expected_html);
}

/**
 * @brief Tests that single-line comments are ignored.
 *
 * Verifies that `//` comments are correctly stripped by the parser
 * and do not affect the final output.
 */
void test_single_line_comment_is_ignored() {
    std::cout << "--- Running Test: test_single_line_comment_is_ignored ---\n";
    std::string source = R"(
        div {
            // This is a comment and should be ignored.
            text: "visible content";
        }
    )";
    CompilerDispatcher dispatcher;
    CompilationResult result = dispatcher.compile(source, "test.chtl", true);

    std::string expected_html = R"(<div>visiblecontent</div>)";
    std::string actual_html_no_whitespace = remove_whitespace(result.html_content);

    assert(actual_html_no_whitespace == expected_html);
    assert(result.html_content.find("This is a comment") == std::string::npos);
}

/**
 * @brief Tests a local style block that generates an inline style attribute.
 *
 * Verifies that `div { style { color: red; } }` compiles to
 * `<div style="color:red;"></div>`. This confirms the parser can handle
 * local style blocks and the generator correctly creates inline styles.
 */
void test_local_inline_style_block() {
    std::cout << "--- Running Test: test_local_inline_style_block ---\n";
    std::string source = R"(div { style { color: red; } })";
    CompilerDispatcher dispatcher;
    CompilationResult result = dispatcher.compile(source, "test.chtl", true);

    std::string expected_html = R"(<divstyle="color:red;"></div>)";
    std::string actual_html_no_whitespace = remove_whitespace(result.html_content);

    assert(actual_html_no_whitespace.find(remove_whitespace(R"(style="color:red;")")) != std::string::npos);
}

/**
 * @brief Tests the definition and application of a Style Template.
 *
 * Verifies that a `[Template] @Style` can be defined and then used
 * within an element's style block, and that the template's properties
 * are correctly applied to the element's inline style.
 */
void test_style_template_definition_and_usage() {
    std::cout << "--- Running Test: test_style_template_definition_and_usage ---\n";
    std::string source = R"(
        [Template] @Style MyBox {
            width: 100px;
            height: 200px;
        }

        div {
            style {
                @Style MyBox;
                background-color: blue;
            }
        }
    )";
    CompilerDispatcher dispatcher;
    CompilationResult result = dispatcher.compile(source, "test.chtl", true);

    std::string actual_html_no_whitespace = remove_whitespace(result.html_content);

    assert(actual_html_no_whitespace.find(remove_whitespace(R"(width:100px;)")) != std::string::npos);
    assert(actual_html_no_whitespace.find(remove_whitespace(R"(height:200px;)")) != std::string::npos);
    assert(actual_html_no_whitespace.find(remove_whitespace(R"(background-color:blue;)")) != std::string::npos);
}

/**
 * @brief Tests the definition and instantiation of an Element Template.
 *
 * Verifies that a `[Template] @Element` can be defined and then
 * instantiated within the document body, correctly rendering its content.
 */
void test_element_template_definition_and_usage() {
    std::cout << "--- Running Test: test_element_template_definition_and_usage ---\n";
    std::string source = R"(
        [Template] @Element MyComponent {
            div {
                text: "Hello from a template!";
            }
        }

        body {
            @Element MyComponent;
        }
    )";
    CompilerDispatcher dispatcher;
    CompilationResult result = dispatcher.compile(source, "test.chtl", true);

    std::string expected_html = R"(<body><div>Hellofromatemplate!</div></body>)";
    std::string actual_html_no_whitespace = remove_whitespace(result.html_content);

    assert(actual_html_no_whitespace == expected_html);
}

/**
 * @brief Tests an element with a standard HTML attribute (`id`).
 *
 * Verifies that `div { id: "main"; }` is correctly compiled to
 * `<div id="main"></div>`. This confirms the attribute parsing logic
 * works for common cases beyond the special `text` attribute.
 */
void test_element_with_id_attribute() {
    std::cout << "--- Running Test: test_element_with_id_attribute ---\n";
    std::string source = R"(div { id: "main"; })";
    CompilerDispatcher dispatcher;
    CompilationResult result = dispatcher.compile(source, "test.chtl", true);

    std::string expected_html = R"(<divid="main"></div>)";
    std::string actual_html_no_whitespace = remove_whitespace(result.html_content);

    assert(actual_html_no_whitespace == expected_html);
}

/**
 * @brief Tests the parsing of nested elements.
 *
 * Verifies that `body { div { text: "nested"; } }` compiles to
 * `<body><div>nested</div></body>`. This confirms the parser's ability
 * to handle hierarchical structures.
 */
void test_nested_elements() {
    std::cout << "--- Running Test: test_nested_elements ---\n";
    std::string source = R"(body { div { text: "nested"; } })";
    CompilerDispatcher dispatcher;
    CompilationResult result = dispatcher.compile(source, "test.chtl", true);

    std::string expected_html = R"(<body><div>nested</div></body>)";
    std::string actual_html_no_whitespace = remove_whitespace(result.html_content);

    assert(actual_html_no_whitespace == expected_html);
}