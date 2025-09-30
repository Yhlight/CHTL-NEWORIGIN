#include "../catch.hpp"
#include "CHTL/CHTLLexer/Lexer.h"
#include "CHTL/CHTLParser/Parser.h"
#include "CHTL/CHTLGenerator/Generator.h"
#include "CHTL/Bridge/SharedContext.h"
#include "Dispatcher/CompilerDispatcher.h"
#include <string>
#include <vector>
#include <memory>
#include <regex>

// Helper function to normalize HTML output for consistent testing.
static std::string normalize_html_for_gen_test(const std::string& html) {
    std::string s = html;
    s = std::regex_replace(s, std::regex(R"(\n)"), "");
    s = std::regex_replace(s, std::regex(R"(>\s*<)"), "><");
    s = std::regex_replace(s, std::regex(R"(^\s+|\s+$)"), "");
    return s;
}

TEST_CASE("Generator for Origin Blocks", "[Generator][Origin]") {
    std::string source = R"(
        [Origin] @Style {
            .raw-css { content: "css"; }
        }

        body {
            [Origin] @Html {
                <div class="raw-html"></div>
            }
        }

        [Origin] @JavaScript {
            console.log("raw js");
        }
    )";

    // Use the CompilerDispatcher to get the final compiled result,
    // as it correctly handles the separation and injection of CSS/JS.
    CompilerDispatcher dispatcher;
    CompilationResult result = dispatcher.compile(
        source,
        "test.chtl",
        false, // default_struct
        true,  // inline_css
        true,  // inline_js
        "",    // css_output_filename
        ""     // js_output_filename
    );

    std::string final_html = normalize_html_for_gen_test(result.html_content);

    // 1. Check for the raw HTML content.
    REQUIRE(final_html.find(R"(<div class="raw-html"></div>)") != std::string::npos);

    // 2. Check for the raw CSS content inside a style tag.
    REQUIRE(final_html.find(R"(<style>.raw-css { content: "css"; }</style>)") != std::string::npos);

    // 3. Check for the raw JS content inside a script tag.
    REQUIRE(final_html.find(R"(<script>console.log("raw js");</script>)") != std::string::npos);
}