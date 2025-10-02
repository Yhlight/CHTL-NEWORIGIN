/**
 * @file test_generator.cpp
 * @brief Tests for CHTL Generator
 */

#include "catch.hpp"
#include "CHTL/CHTLGenerator/Generator.h"
#include "CHTL/CHTLContext.h"
#include "CHTL/CHTLNode/ElementNode.h"
#include "CHTL/CHTLNode/TextNode.h"

using namespace CHTL;

TEST_CASE("Generator creates HTML", "[generator]") {
    CHTLContext context;
    Generator generator(context);
    
    SECTION("Simple element") {
        auto element = std::make_shared<ElementNode>("div");
        element->setAttribute("id", "test");
        
        std::string html = generator.generateHTML(element);
        
        REQUIRE(!html.empty());
        REQUIRE(html.find("<div") != std::string::npos);
        REQUIRE(html.find("id=\"test\"") != std::string::npos);
        REQUIRE(html.find("</div>") != std::string::npos);
    }
    
    SECTION("Element with text") {
        auto element = std::make_shared<ElementNode>("p");
        auto text = std::make_shared<TextNode>("Hello, World!");
        element->addChild(text);
        
        std::string html = generator.generateHTML(element);
        
        REQUIRE(html.find("<p>") != std::string::npos);
        REQUIRE(html.find("Hello, World!") != std::string::npos);
        REQUIRE(html.find("</p>") != std::string::npos);
    }
}
