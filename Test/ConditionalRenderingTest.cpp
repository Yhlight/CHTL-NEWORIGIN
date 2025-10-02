#include "catch.hpp"
#include "CHTLParser/CHTLParser.h"
#include "CHTLGenerator/HtmlGenerator.h"
#include "CHTLNode/DocumentNode.h"
#include "CHTLContext/CHTLContext.h"
#include <memory>

TEST_CASE("Conditional rendering with a simple if block", "[conditional]") {
    const std::string input = R"(
div {
    if {
        condition: true,
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

    REQUIRE(result.find("style=\"color: red;\"") != std::string::npos);
}

TEST_CASE("Conditional rendering with a false if block", "[conditional]") {
    const std::string input = R"(
div {
    if {
        condition: false,
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

    REQUIRE(result.find("style=") == std::string::npos);
}

TEST_CASE("Conditional rendering with if-else, if true", "[conditional]") {
    const std::string input = R"(
div {
    if {
        condition: true,
        color: red;
    } else {
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

    REQUIRE(result.find("style=\"color: red;\"") != std::string::npos);
    REQUIRE(result.find("color: blue;") == std::string::npos);
}

TEST_CASE("Conditional rendering with if-else, else true", "[conditional]") {
    const std::string input = R"(
div {
    if {
        condition: false,
        color: red;
    } else {
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

    REQUIRE(result.find("style=\"color: blue;\"") != std::string::npos);
    REQUIRE(result.find("color: red;") == std::string::npos);
}

TEST_CASE("Conditional rendering with if-else if-else", "[conditional]") {
    const std::string input = R"(
div {
    if {
        condition: false,
        color: red;
    } else if {
        condition: true,
        color: green;
    } else {
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

    REQUIRE(result.find("style=\"color: green;\"") != std::string::npos);
    REQUIRE(result.find("color: red;") == std::string::npos);
    REQUIRE(result.find("color: blue;") == std::string::npos);
}