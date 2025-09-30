#include <catch.hpp>
#include "CHTL/CHTLParser.h"
#include "CHTL/CHTLLexer.h"

TEST_CASE("CHTLParser - Basic Parsing", "[parser]") {
    CHTL::CHTLLexer lexer;
    lexer.initialize();
    
    CHTL::CHTLParser parser;
    parser.initialize();
    
    SECTION("Simple element parsing") {
        std::string input = "div { text: \"Hello World\"; }";
        auto tokens = lexer.tokenize(input);
        auto ast = parser.parse(tokens);
        
        REQUIRE(parser.isSuccessful());
        REQUIRE(ast != nullptr);
    }
    
    SECTION("Empty input") {
        std::string input = "";
        auto tokens = lexer.tokenize(input);
        auto ast = parser.parse(tokens);
        
        REQUIRE(parser.isSuccessful());
        REQUIRE(ast != nullptr);
    }
}

TEST_CASE("CHTLParser - Error Handling", "[parser]") {
    CHTL::CHTLLexer lexer;
    lexer.initialize();
    
    CHTL::CHTLParser parser;
    parser.initialize();
    
    SECTION("Invalid syntax") {
        std::string input = "div { text: \"Hello World\" // Missing closing brace";
        auto tokens = lexer.tokenize(input);
        auto ast = parser.parse(tokens);
        
        // Parser should handle this gracefully
        REQUIRE(ast != nullptr);
    }
}