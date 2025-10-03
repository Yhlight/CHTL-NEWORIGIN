#include "catch.hpp"
#include "CHTLParser/CHTLParser.h"
#include "CHTLGenerator/HtmlGenerator.h"
#include "CHTLNode/DocumentNode.h"
#include "CHTLContext/CHTLContext.h"
#include <memory>

TEST_CASE("Conditional rendering with property from parent", "[conditional]") {
    const std::string input = R"(
div {
    style {
        width: 100px;
    }
    if {
        condition: width > 50,
        color: red;
    }
}
)";
    CHTLContext context;
    CHTLParser parser(input, context);
    auto doc = parser.parse();
    HtmlGenerator generator;
    doc->accept(generator);
    std::string result = generator.getResult();

    REQUIRE(result.find("style=\"width: 100px; color: red;\"") != std::string::npos);
}

TEST_CASE("Conditional rendering with complex expression", "[conditional]") {
    const std::string input = R"(
div {
    style {
        width: 100px;
        height: 200px;
    }
    if {
        condition: (width > 50 && height < 300) || height > 500,
        color: green;
    }
}
)";
    CHTLContext context;
    CHTLParser parser(input, context);
    auto doc = parser.parse();
    HtmlGenerator generator;
    doc->accept(generator);
    std::string result = generator.getResult();

    REQUIRE(result.find("style=\"width: 100px; height: 200px; color: green;\"") != std::string::npos);
}

TEST_CASE("Conditionally set both HTML attribute and CSS style", "[conditional]") {
    const std::string input = R"(
div {
    style {
        width: 100px;
    }
    if {
        condition: width > 50,
        hidden: "true";
        color: blue;
    }
}
)";
    CHTLContext context;
    CHTLParser parser(input, context);
    auto doc = parser.parse();
    HtmlGenerator generator;
    doc->accept(generator);
    std::string result = generator.getResult();

    REQUIRE(result.find("hidden=\"true\"") != std::string::npos);
    REQUIRE(result.find("style=\"width: 100px; color: blue;\"") != std::string::npos);
}