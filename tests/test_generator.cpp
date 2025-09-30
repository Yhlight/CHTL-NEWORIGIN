#include <catch.hpp>
#include "CHTL/CHTLGenerator.h"
#include "CHTL/CHTLNode/ElementNode.h"
#include "CHTL/CHTLNode/BaseNode.h"

TEST_CASE("CHTLGenerator - Basic Generation", "[generator]") {
    CHTL::CHTLGenerator generator;
    generator.initialize();
    
    SECTION("Empty AST") {
        auto root = std::make_shared<CHTL::RootNode>();
        bool success = generator.generate(root);
        
        REQUIRE(success);
        REQUIRE(generator.isSuccessful());
    }
    
    SECTION("Simple element generation") {
        auto root = std::make_shared<CHTL::RootNode>();
        auto element = std::make_shared<CHTL::ElementNode>("div");
        element->setContent("Hello World");
        root->addChild(element);
        
        bool success = generator.generate(root);
        
        REQUIRE(success);
        REQUIRE(generator.isSuccessful());
        REQUIRE(!generator.getHtmlOutput().empty());
    }
}

TEST_CASE("CHTLGenerator - Output Formats", "[generator]") {
    CHTL::CHTLGenerator generator;
    generator.initialize();
    
    SECTION("HTML output") {
        auto root = std::make_shared<CHTL::RootNode>();
        bool success = generator.generate(root);
        
        REQUIRE(success);
        REQUIRE(generator.getHtmlOutput().empty()); // Empty root should produce empty HTML
    }
    
    SECTION("CSS output") {
        auto root = std::make_shared<CHTL::RootNode>();
        bool success = generator.generate(root);
        
        REQUIRE(success);
        REQUIRE(generator.getCssOutput().empty()); // Empty root should produce empty CSS
    }
    
    SECTION("JS output") {
        auto root = std::make_shared<CHTL::RootNode>();
        bool success = generator.generate(root);
        
        REQUIRE(success);
        REQUIRE(generator.getJsOutput().empty()); // Empty root should produce empty JS
    }
}