/**
 * @file test_parser.cpp
 * @brief Tests for CHTL Parser
 */

#include "catch.hpp"
#include "CHTL/CHTLParser/Parser.h"
#include "CHTL/CHTLLexer/Lexer.h"
#include "CHTL/CHTLContext.h"
#include "CHTL/CHTLNode/BaseNode.h"

using namespace CHTL;

TEST_CASE("Parser creates AST", "[parser]") {
    CHTLContext context;
    
    SECTION("Empty input") {
        Lexer lexer("");
        auto tokens = lexer.tokenize();
        Parser parser(tokens, context);
        auto ast = parser.parse();
        
        REQUIRE(ast != nullptr);
    }
}
