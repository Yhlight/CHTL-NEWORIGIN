#include "../third-party/catch.hpp"
#include "../CHTL/CHTLGenerator/CHTLGenerator.h"
#include "../CHTL/CHTLNode/ElementNode.h"
#include "../CHTL/CHTLNode/TextNode.h"
#include "../CHTL/CHTLNode/StyleNode.h"
#include "../CHTL/CHTLNode/PropertyNode.h"

TEST_CASE("Generator handles simple element", "[generator]") {
    auto element = std::make_shared<CHTL::ElementNode>("div");
    CHTL::CHTLGenerator generator;
    std::string result = generator.generate(element);
    REQUIRE(result == "<div></div>");
}

TEST_CASE("Generator handles element with attributes", "[generator]") {
    auto element = std::make_shared<CHTL::ElementNode>("a");
    element->setAttribute("href", "https://example.com");
    element->setAttribute("target", "_blank");
    CHTL::CHTLGenerator generator;
    std::string result = generator.generate(element);
    REQUIRE(result == "<a href=\"https://example.com\" target=\"_blank\"></a>");
}

TEST_CASE("Generator handles nested elements", "[generator]") {
    auto parent = std::make_shared<CHTL::ElementNode>("div");
    auto child = std::make_shared<CHTL::ElementNode>("p");
    parent->addChild(child);
    CHTL::CHTLGenerator generator;
    std::string result = generator.generate(parent);
    REQUIRE(result == "<div><p></p></div>");
}

TEST_CASE("Generator handles text node", "[generator]") {
    auto text = std::make_shared<CHTL::TextNode>("Hello");
    CHTL::CHTLGenerator generator;
    std::string result = generator.generate(text);
    REQUIRE(result == "Hello");
}

TEST_CASE("Generator handles style block", "[generator]") {
    auto style = std::make_shared<CHTL::StyleNode>();
    style->addChild(std::make_shared<CHTL::PropertyNode>("color", "blue"));
    style->addChild(std::make_shared<CHTL::PropertyNode>("font-size", "16px"));
    CHTL::CHTLGenerator generator;
    std::string result = generator.generate(style);
    REQUIRE(result == "<style>color: blue;font-size: 16px;</style>");
}

TEST_CASE("Generator handles complex structure", "[generator]") {
    auto div = std::make_shared<CHTL::ElementNode>("div");
    div->setAttribute("class", "container");

    auto style = std::make_shared<CHTL::StyleNode>();
    style->addChild(std::make_shared<CHTL::PropertyNode>("border", "1px solid black"));
    div->addChild(style);

    auto p = std::make_shared<CHTL::ElementNode>("p");
    p->addChild(std::make_shared<CHTL::TextNode>("This is a test."));
    div->addChild(p);

    CHTL::CHTLGenerator generator;
    std::string result = generator.generate(div);
    REQUIRE(result == "<div class=\"container\"><style>border: 1px solid black;</style><p>This is a test.</p></div>");
}