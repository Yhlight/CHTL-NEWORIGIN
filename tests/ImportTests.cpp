#include "Dispatcher/CompilerDispatcher.h"
#include "Dispatcher/CompilationResult.h"
#include <cassert>
#include <string>
#include <iostream>
#include <algorithm>

// Helper function to remove all whitespace from a string for robust comparison
// This is defined in CoreFeatures.cpp, so we just need to declare it here.
static std::string remove_whitespace(std::string str) {
    str.erase(std::remove_if(str.begin(), str.end(), ::isspace), str.end());
    return str;
}


void test_import_style_template() {
    std::cout << "--- Running Test: test_import_style_template ---\n";
    std::string source = R"chtl(
        [Import] [Template] @Style ImportedStyle from "import_samples/styles.chtl";

        div {
            style {
                @Style ImportedStyle;
            }
        }
    )chtl";

    // The dispatcher needs the path of the source file to resolve relative imports.
    // We are in the build/ directory when tests run, so the path is relative from there.
    CompilerDispatcher dispatcher;
    CompilationResult result = dispatcher.compile(source, "../tests/test_file.chtl", false);

    std::string expected_css = "div{color:blue;font-size:16px;}";
    assert(remove_whitespace(result.css_content) == remove_whitespace(expected_css));
}

void test_import_element_template() {
    std::cout << "--- Running Test: test_import_element_template ---\n";
     std::string source = R"chtl(
        [Import] [Template] @Element ImportedElement from "import_samples/elements.chtl";

        body {
            @Element ImportedElement;
        }
    )chtl";

    CompilerDispatcher dispatcher;
    CompilationResult result = dispatcher.compile(source, "../tests/test_file.chtl", false);

    std::string expected_html = "<body><p>This element was imported.</p></body>";
    assert(remove_whitespace(result.html_content) == remove_whitespace(expected_html));
}

void test_import_non_chtl_as_origin() {
    std::cout << "--- Running Test: test_import_non_chtl_as_origin ---\n";
    std::string source = R"chtl(
        [Import] @Style from "import_samples/sample.css" as MyCss;

        head {
            style {
                [Origin] @Style MyCss;
            }
        }
    )chtl";

    CompilerDispatcher dispatcher;
    CompilationResult result = dispatcher.compile(source, "../tests/test_file.chtl", false);

    std::string expected_css = ".imported-class{border:1pxsolidgreen;}";
    assert(remove_whitespace(result.css_content).find(expected_css) != std::string::npos);
}