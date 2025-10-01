#define CATCH_CONFIG_MAIN
#include "../../catch.hpp"
#include "../../CHTL/CHTLLexer/Lexer.h"

TEST_CASE("Lexer tokenizes an empty string", "[lexer]") {
    Lexer lexer("");
    auto tokens = lexer.tokenize();
    REQUIRE(tokens.size() == 1);
    REQUIRE(tokens[0].getType() == TokenType::EndOfFile);
}