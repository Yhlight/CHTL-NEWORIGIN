#include "../catch.hpp"
#include "CHTL/CHTLLexer/Lexer.h"
#include "CHTL/CHTLParser/Parser.h"
#include "CHTL/CHTLGenerator/Generator.h"
#include "CHTL/Bridge/SharedContext.h"
#include "CHTL/CHTLManage/TemplateManager.h"
#include "CHTL/CHTLManage/ConfigurationManager.h"
#include "Dispatcher/CompilerDispatcher.h"
#include <string>
#include <vector>
#include <memory>
#include <iostream>
#include <regex>
#include <filesystem>
#include <fstream>

// Helper function to normalize HTML output for consistent testing.
// This removes indentation and extra newlines.
static std::string normalize_html_for_core_test(const std::string& html) {
    std::string s = html;
    // Replace newlines with a single space for a flat comparison, then trim.
    s = std::regex_replace(s, std::regex(R"(\n)"), "");
    // Remove space between tags
    s = std::regex_replace(s, std::regex(R"(>\s*<)"), "><");
    // Remove leading/trailing whitespace
    s = std::regex_replace(s, std::regex(R"(^\s+|\s+$)"), "");
    return s;
}


// Helper function to streamline the process of converting CHTL code to HTML for testing.
static std::string chtl_to_html_for_core_test(const std::string& chtl_code) {
    Lexer lexer(chtl_code);
    TemplateManager tm;
    ConfigurationManager cm;
    Parser parser(lexer, tm, cm);
    parser.parse();

    Generator generator;
    SharedContext context;
    // Generate with default parameters (no doctype, no external files)
    return generator.generate(parser.ast, tm, "", "", context, false, false, false, "", "");
}

TEST_CASE("Core CHTL Feature Parsing and Generation", "[core]") {

    SECTION("A single element with a text attribute should be parsed and generated correctly") {
        std::string chtl = R"(
            p {
                text: "Hello, CHTL.";
            }
        )";
        std::string expected_html = "<p>Hello, CHTL.</p>";
        std::string actual_html = chtl_to_html_for_core_test(chtl);
        REQUIRE(normalize_html_for_core_test(actual_html) == expected_html);
    }

    SECTION("Nested elements should be parsed and generated in the correct hierarchy") {
        std::string chtl = R"(
            html {
                body {
                    div {
                        text: "I am in a div.";
                    }
                }
            }
        )";
        std::string expected_html = "<html><body><div>I am in a div.</div></body></html>";
        std::string actual_html = chtl_to_html_for_core_test(chtl);
        REQUIRE(normalize_html_for_core_test(actual_html) == expected_html);
    }

    SECTION("An element with a text block should be parsed and generated correctly") {
        std::string chtl = R"(
            div {
                text {
                    "This is a block of text."
                }
            }
        )";
        std::string expected_html = "<div>This is a block of text.</div>";
        std::string actual_html = chtl_to_html_for_core_test(chtl);
        REQUIRE(normalize_html_for_core_test(actual_html) == expected_html);
    }

    SECTION("Unquoted text in a text block should be concatenated") {
        std::string chtl = R"(
            p {
                text {
                    some unquoted words
                }
            }
        )";
        std::string expected_html = "<p>some unquoted words</p>";
        std::string actual_html = chtl_to_html_for_core_test(chtl);
        REQUIRE(normalize_html_for_core_test(actual_html) == expected_html);
    }

    SECTION("A mix of child elements and text nodes should be ordered correctly") {
        std::string chtl = R"(
            main {
                h1 { text: "Title"; }
                p { text: "Paragraph 1."; }
                p { text: "Paragraph 2."; }
            }
        )";
        std::string expected_html = "<main><h1>Title</h1><p>Paragraph 1.</p><p>Paragraph 2.</p></main>";
        std::string actual_html = chtl_to_html_for_core_test(chtl);
        REQUIRE(normalize_html_for_core_test(actual_html) == expected_html);
    }

    SECTION("Element template defined and used in the same file") {
        std::string chtl = R"(
            [Template] @Element MyHeader {
                h1 { text: "Hello Template"; }
            }
            body {
                @Element MyHeader;
            }
        )";
        std::string expected_html = "<body><h1>Hello Template</h1></body>";
        std::string actual_html = chtl_to_html_for_core_test(chtl);
        REQUIRE(normalize_html_for_core_test(actual_html) == expected_html);
    }
}

// Helper function to read a file's content into a string
static std::string read_test_file(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        FAIL("Failed to open test file: " << path);
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

TEST_CASE("CHTL Import Feature", "[core][import]") {
    SECTION("Should correctly import CHTL templates and CSS files") {
        // Setup paths relative to the build directory where the test runs
        std::string base_dir = "../";
        std::string input_dir = base_dir + "tests/temp_import_test";
        std::string output_dir = "temp_output"; // Use a local output dir
        std::string input_file = input_dir + "/main.chtl";

        // Ensure the output directory exists and is empty
        if (std::filesystem::exists(output_dir)) {
            std::filesystem::remove_all(output_dir);
        }
        std::filesystem::create_directory(output_dir);

        // Compile the main CHTL file using the dispatcher
        CompilerDispatcher dispatcher;
        CompilationResult result = dispatcher.compile(
            read_test_file(input_file), // source
            input_file,                 // source_path
            false,                      // default_struct
            false,                      // inline_css
            false,                      // inline_js
            output_dir + "/main.css",   // css_output_filename
            output_dir + "/main.js"     // js_output_filename
        );

        // Write the output files
        std::ofstream(output_dir + "/main.html") << result.html_content;
        std::ofstream(output_dir + "/main.css") << result.css_content;

        // Verify the output
        std::string generated_html = read_test_file(output_dir + "/main.html");
        std::string generated_css = read_test_file(output_dir + "/main.css");

        // --- HTML Verification ---
        // Check if the imported template was rendered
        REQUIRE(normalize_html_for_core_test(generated_html).find("<h1>Welcome to the Imported Page</h1>") != std::string::npos);
        // Check for the paragraph from the main file
        REQUIRE(normalize_html_for_core_test(generated_html).find("<p>This page demonstrates the CHTL import functionality.</p>") != std::string::npos);

        // --- CSS Verification ---
        // Check for the style from the imported CHTL template
        REQUIRE(generated_css.find("color: navy;") != std::string::npos);
        // Check for the style from the imported CSS file
        REQUIRE(generated_css.find("background-color: #f0f0f0;") != std::string::npos);

        // Cleanup the temporary output directory
        std::filesystem::remove_all(output_dir);
    }
}