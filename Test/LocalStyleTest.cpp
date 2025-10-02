#include "catch.hpp"
#include "CHTLProcessor/CHTLProcessor.h"
#include "CHTLGenerator/HtmlGenerator.h"
#include <string>

TEST_CASE("Local style blocks are parsed and generated correctly", "[local_style]") {
    const std::string entryFile = "Test/integration/local_style.chtl";
    CHTLProcessor processor(entryFile);
    auto ast = processor.process();

    REQUIRE(ast != nullptr);

    HtmlGenerator generator(std::move(ast));
    std::string result = generator.generate();

    // Check for hoisted CSS rule in the head
    std::string expected_hoisted_css = ".nested";
    REQUIRE(result.find(expected_hoisted_css) != std::string::npos);
    REQUIRE(result.find("<head>") < result.find(expected_hoisted_css));
    REQUIRE(result.find("</head>") > result.find(expected_hoisted_css));

    // Check for inline style on the element
    std::string expected_inline_style = "style=\"background-color: blue;\"";
    REQUIRE(result.find(expected_inline_style) != std::string::npos);
}