#include "../CHTL/CHTLLexer/Lexer.h"
#include "Assert.h"
#include <iostream>

void testSimpleTokens() {
    std::string source = "{}()[]:;,.";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();

    CHTL::Test::assertEquals(TokenType::LEFT_BRACE, tokens[0].type, "testSimpleTokens: LEFT_BRACE");
    CHTL::Test::assertEquals(TokenType::RIGHT_BRACE, tokens[1].type, "testSimpleTokens: RIGHT_BRACE");
    CHTL::Test::assertEquals(TokenType::LEFT_PAREN, tokens[2].type, "testSimpleTokens: LEFT_PAREN");
    CHTL::Test::assertEquals(TokenType::RIGHT_PAREN, tokens[3].type, "testSimpleTokens: RIGHT_PAREN");
    CHTL::Test::assertEquals(TokenType::LEFT_BRACKET, tokens[4].type, "testSimpleTokens: LEFT_BRACKET");
    CHTL::Test::assertEquals(TokenType::RIGHT_BRACKET, tokens[5].type, "testSimpleTokens: RIGHT_BRACKET");
    CHTL::Test::assertEquals(TokenType::COLON, tokens[6].type, "testSimpleTokens: COLON");
    CHTL::Test::assertEquals(TokenType::SEMICOLON, tokens[7].type, "testSimpleTokens: SEMICOLON");
    CHTL::Test::assertEquals(TokenType::COMMA, tokens[8].type, "testSimpleTokens: COMMA");
    CHTL::Test::assertEquals(TokenType::DOT, tokens[9].type, "testSimpleTokens: DOT");
    CHTL::Test::assertEquals(TokenType::END_OF_FILE, tokens[10].type, "testSimpleTokens: END_OF_FILE");
}

void testStringLiterals() {
    std::string source = "\"hello\" 'world'";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();

    CHTL::Test::assertEquals(TokenType::STRING, tokens[0].type, "testStringLiterals: STRING (double)");
    CHTL::Test::assertEquals("hello", tokens[0].lexeme, "testStringLiterals: STRING value (double)");
    CHTL::Test::assertEquals(TokenType::STRING, tokens[1].type, "testStringLiterals: STRING (single)");
    CHTL::Test::assertEquals("world", tokens[1].lexeme, "testStringLiterals: STRING value (single)");
}

void testNumberLiterals() {
    std::string source = "123 45.67";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();

    CHTL::Test::assertEquals(TokenType::NUMBER, tokens[0].type, "testNumberLiterals: NUMBER (int)");
    CHTL::Test::assertEquals("123", tokens[0].lexeme, "testNumberLiterals: NUMBER value (int)");
    CHTL::Test::assertEquals(TokenType::NUMBER, tokens[1].type, "testNumberLiterals: NUMBER (float)");
    CHTL::Test::assertEquals("45.67", tokens[1].lexeme, "testNumberLiterals: NUMBER value (float)");
}

void testIdentifiersAndKeywords() {
    std::string source = "div text style";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();

    CHTL::Test::assertEquals(TokenType::IDENTIFIER, tokens[0].type, "testIdentifiersAndKeywords: IDENTIFIER");
    CHTL::Test::assertEquals("div", tokens[0].lexeme, "testIdentifiersAndKeywords: IDENTIFIER value");
    CHTL::Test::assertEquals(TokenType::TEXT, tokens[1].type, "testIdentifiersAndKeywords: TEXT");
    CHTL::Test::assertEquals(TokenType::STYLE, tokens[2].type, "testIdentifiersAndKeywords: STYLE");
}

void testComments() {
    std::string source = "// this is a comment\n"
                         "/* block comment */\n"
                         "# generator comment";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();

    CHTL::Test::assertEquals(TokenType::END_OF_FILE, tokens[0].type, "testComments: END_OF_FILE");
}

int main() {
    testSimpleTokens();
    testStringLiterals();
    testNumberLiterals();
    testIdentifiersAndKeywords();
    testComments();

    std::cout << "All lexer tests passed!" << std::endl;

    return 0;
}