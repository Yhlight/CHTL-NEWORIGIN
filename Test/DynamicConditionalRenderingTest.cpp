#include "catch.hpp"
#include "../CHTL/CHTLParser/CHTLParser.h"
#include "../CHTL/CHTLGenerator/HtmlGenerator.h"
#include "../CHTL/CHTLContext/CHTLContext.h"
#include <string>
#include <algorithm>
#include <cctype>

// Helper function to normalize JS for comparison by removing all whitespace
static std::string normalizeJs(const std::string& js) {
    std::string normalized;
    for (char c : js) {
        if (!std::isspace(static_cast<unsigned char>(c))) {
            normalized += c;
        }
    }
    return normalized;
}

static void runDynamicConditionalTest(const std::string& chtlInput, const std::string& expectedJs) {
    CHTLContext context;
    CHTLParser parser(chtlInput, context);
    auto document = parser.parse();

    HtmlGenerator generator;
    document->accept(generator);
    std::string result = generator.getResult();

    // Extract script content
    size_t script_start = result.find("<script>");
    size_t script_end = result.find("</script>");
    REQUIRE(script_start != std::string::npos);
    REQUIRE(script_end != std::string::npos);

    script_start += std::string("<script>").length();
    std::string script_content = result.substr(script_start, script_end - script_start);

    REQUIRE(normalizeJs(script_content) == normalizeJs(expectedJs));
}

TEST_CASE("Dynamic Conditional Rendering", "[dynamic_conditional]") {
    SECTION("Simple dynamic if") {
        std::string chtl = R"(
            div {
                if {
                    condition: {{window.innerWidth}} > 500,
                    display: "block";
                }
            }
        )";

        std::string expectedJs = R"(
            function update_chtl_dynamic_0() {
              const element = document.getElementById('chtl-dynamic-0');
              if (!element) return;
              if (window.innerWidth > 500 ) {
                element.style.display = 'block';
              }
            }
            window.addEventListener('DOMContentLoaded', update_chtl_dynamic_0);
            window.addEventListener('resize', update_chtl_dynamic_0);
        )";
        runDynamicConditionalTest(chtl, expectedJs);
    }

    SECTION("Dynamic if-else") {
        std::string chtl = R"(
            div {
                if {
                    condition: {{window.innerWidth}} > 1000,
                    color: "red";
                } else {
                    color: "blue";
                }
            }
        )";

        std::string expectedJs = R"(
            function update_chtl_dynamic_0() {
              const element = document.getElementById('chtl-dynamic-0');
              if (!element) return;
              if (window.innerWidth > 1000 ) {
                element.style.color = 'red';
              } else {
                element.style.color = 'blue';
              }
            }
            window.addEventListener('DOMContentLoaded', update_chtl_dynamic_0);
            window.addEventListener('resize', update_chtl_dynamic_0);
        )";
        runDynamicConditionalTest(chtl, expectedJs);
    }

    SECTION("Dynamic if-else if-else") {
        std::string chtl = R"(
            div {
                if {
                    condition: {{window.innerWidth}} > 1200,
                    font-size: "20px";
                } else if {
                    condition: {{window.innerWidth}} > 800,
                    font-size: "16px";
                }
                else {
                    font-size: "12px";
                }
            }
        )";

        std::string expectedJs = R"(
            function update_chtl_dynamic_0() {
              const element = document.getElementById('chtl-dynamic-0');
              if (!element) return;
              if (window.innerWidth > 1200 ) {
                element.style.fontSize = '20px';
              } else if (window.innerWidth > 800 ) {
                element.style.fontSize = '16px';
              } else {
                element.style.fontSize = '12px';
              }
            }
            window.addEventListener('DOMContentLoaded', update_chtl_dynamic_0);
            window.addEventListener('resize', update_chtl_dynamic_0);
        )";
        runDynamicConditionalTest(chtl, expectedJs);
    }
}