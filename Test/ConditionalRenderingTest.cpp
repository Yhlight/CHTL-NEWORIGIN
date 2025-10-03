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

TEST_CASE("Conditional rendering with a false condition", "[conditional]") {
    const std::string input = R"(
div {
    style {
        width: 30px;
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

    // The color property should not be present
    REQUIRE(result.find("style=\"width: 30px;\"") != std::string::npos);
    REQUIRE(result.find("color: red;") == std::string::npos);
}

TEST_CASE("Conditional rendering with if-else", "[conditional]") {
    // Test the 'if' branch
    const std::string input_if_true = R"(
div {
    style {
        width: 100px;
    }
    if {
        condition: width > 50,
        color: red;
    } else {
        color: blue;
    }
}
)";
    CHTLContext context_if_true;
    CHTLParser parser_if_true(input_if_true, context_if_true);
    auto doc_if_true = parser_if_true.parse();
    HtmlGenerator generator_if_true;
    doc_if_true->accept(generator_if_true);
    std::string result_if_true = generator_if_true.getResult();
    REQUIRE(result_if_true.find("style=\"width: 100px; color: red;\"") != std::string::npos);

    // Test the 'else' branch
    const std::string input_if_false = R"(
div {
    style {
        width: 30px;
    }
    if {
        condition: width > 50,
        color: red;
    } else {
        color: blue;
    }
}
)";
    CHTLContext context_if_false;
    CHTLParser parser_if_false(input_if_false, context_if_false);
    auto doc_if_false = parser_if_false.parse();
    HtmlGenerator generator_if_false;
    doc_if_false->accept(generator_if_false);
    std::string result_if_false = generator_if_false.getResult();
    REQUIRE(result_if_false.find("style=\"width: 30px; color: blue;\"") != std::string::npos);
}

TEST_CASE("Conditional rendering with if-else if-else", "[conditional]") {
    // Test the 'if' branch
    const std::string input_if = R"(
div {
    style { width: 300px; }
    if { condition: width > 200, color: red; }
    else if { condition: width > 100, color: orange; }
    else { color: yellow; }
}
)";
    CHTLContext ctx_if;
    CHTLParser p_if(input_if, ctx_if);
    auto doc_if = p_if.parse();
    HtmlGenerator g_if;
    doc_if->accept(g_if);
    REQUIRE(g_if.getResult().find("style=\"width: 300px; color: red;\"") != std::string::npos);

    // Test the 'else if' branch
    const std::string input_else_if = R"(
div {
    style { width: 150px; }
    if { condition: width > 200, color: red; }
    else if { condition: width > 100, color: orange; }
    else { color: yellow; }
}
)";
    CHTLContext ctx_else_if;
    CHTLParser p_else_if(input_else_if, ctx_else_if);
    auto doc_else_if = p_else_if.parse();
    HtmlGenerator g_else_if;
    doc_else_if->accept(g_else_if);
    REQUIRE(g_else_if.getResult().find("style=\"width: 150px; color: orange;\"") != std::string::npos);

    // Test the 'else' branch
    const std::string input_else = R"(
div {
    style { width: 50px; }
    if { condition: width > 200, color: red; }
    else if { condition: width > 100, color: orange; }
    else { color: yellow; }
}
)";
    CHTLContext ctx_else;
    CHTLParser p_else(input_else, ctx_else);
    auto doc_else = p_else.parse();
    HtmlGenerator g_else;
    doc_else->accept(g_else);
    REQUIRE(g_else.getResult().find("style=\"width: 50px; color: yellow;\"") != std::string::npos);
}