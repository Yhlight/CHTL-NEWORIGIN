#include "catch.hpp"
#include "CHTLLexer/CHTLLexer.h"
#include <vector>

TEST_CASE("Lexer Initialization", "[lexer]") {
    CHTL::CHTLLexer lexer;
    SUCCEED("Lexer can be initialized.");
}

TEST_CASE("Tokenize Single-Line Comment", "[lexer]") {
    CHTL::CHTLLexer lexer;
    std::string input = "// This is a test comment";
    std::vector<CHTL::Token> tokens = lexer.tokenize(input);

    REQUIRE(tokens.size() == 1);
    REQUIRE(tokens[0].type == CHTL::TokenType::COMMENT);
    REQUIRE(tokens[0].value == " This is a test comment");
}

TEST_CASE("Tokenize Multi-Line Comment", "[lexer]") {
    CHTL::CHTLLexer lexer;
    std::string input = "/* This is a multi-line comment */";
    std::vector<CHTL::Token> tokens = lexer.tokenize(input);

    REQUIRE(tokens.size() == 1);
    REQUIRE(tokens[0].type == CHTL::TokenType::COMMENT);
    REQUIRE(tokens[0].value == " This is a multi-line comment ");
}

TEST_CASE("Tokenize Generator Comment", "[lexer]") {
    CHTL::CHTLLexer lexer;
    std::string input = "# this is a generator comment";
    std::vector<CHTL::Token> tokens = lexer.tokenize(input);

    REQUIRE(tokens.size() == 1);
    REQUIRE(tokens[0].type == CHTL::TokenType::COMMENT);
    // As per spec, the space after '#' is a delimiter and not part of the value.
    REQUIRE(tokens[0].value == "this is a generator comment");
}

TEST_CASE("Tokenize Text Block", "[lexer]") {
    CHTL::CHTLLexer lexer;
    std::string input = "text { \"Hello\" }";
    std::vector<CHTL::Token> tokens = lexer.tokenize(input);

    REQUIRE(tokens.size() == 4);
    REQUIRE(tokens[0].type == CHTL::TokenType::KEYWORD_TEXT);
    REQUIRE(tokens[0].value == "text");
    REQUIRE(tokens[1].type == CHTL::TokenType::L_BRACE);
    REQUIRE(tokens[1].value == "{");
    REQUIRE(tokens[2].type == CHTL::TokenType::STRING_LITERAL);
    REQUIRE(tokens[2].value == "Hello");
    REQUIRE(tokens[3].type == CHTL::TokenType::R_BRACE);
    REQUIRE(tokens[3].value == "}");
}

TEST_CASE("Tokenize Attribute Syntax Characters", "[lexer]") {
    CHTL::CHTLLexer lexer;
    std::string input = ":=;";
    std::vector<CHTL::Token> tokens = lexer.tokenize(input);

    REQUIRE(tokens.size() == 3);
    REQUIRE(tokens[0].type == CHTL::TokenType::COLON);
    REQUIRE(tokens[1].type == CHTL::TokenType::EQUAL);
    REQUIRE(tokens[2].type == CHTL::TokenType::SEMICOLON);
}

TEST_CASE("Tokenize Number", "[lexer]") {
    CHTL::CHTLLexer lexer;
    std::string input = "123";
    std::vector<CHTL::Token> tokens = lexer.tokenize(input);

    REQUIRE(tokens.size() == 1);
    REQUIRE(tokens[0].type == CHTL::TokenType::NUMBER);
    REQUIRE(tokens[0].value == "123");
}

TEST_CASE("Tokenize Script Keyword", "[lexer]") {
    CHTL::CHTLLexer lexer;
    std::string input = "script";
    std::vector<CHTL::Token> tokens = lexer.tokenize(input);

    REQUIRE(tokens.size() == 1);
    REQUIRE(tokens[0].type == CHTL::TokenType::KEYWORD_SCRIPT);
    REQUIRE(tokens[0].value == "script");
}

TEST_CASE("Tokenize Arithmetic Operators", "[lexer]") {
    CHTL::CHTLLexer lexer;
    std::string input = "+ - * / % **";
    std::vector<CHTL::Token> tokens = lexer.tokenize(input);

    REQUIRE(tokens.size() == 6);
    REQUIRE(tokens[0].type == CHTL::TokenType::PLUS);
    REQUIRE(tokens[1].type == CHTL::TokenType::MINUS);
    REQUIRE(tokens[2].type == CHTL::TokenType::STAR);
    REQUIRE(tokens[3].type == CHTL::TokenType::SLASH);
    REQUIRE(tokens[4].type == CHTL::TokenType::PERCENT);
    REQUIRE(tokens[5].type == CHTL::TokenType::STAR_STAR);
}

TEST_CASE("Tokenize Block Keywords", "[lexer]") {
    CHTL::CHTLLexer lexer;
    std::string input = "[Custom] [Template] [Origin] [Import] [Namespace] [Configuration] [Info] [Export] [OriginType] [Name]";
    std::vector<CHTL::Token> tokens = lexer.tokenize(input);

    REQUIRE(tokens.size() == 10);
    REQUIRE(tokens[0].type == CHTL::TokenType::KEYWORD_CUSTOM);
    REQUIRE(tokens[1].type == CHTL::TokenType::KEYWORD_TEMPLATE);
    REQUIRE(tokens[2].type == CHTL::TokenType::KEYWORD_ORIGIN);
    REQUIRE(tokens[3].type == CHTL::TokenType::KEYWORD_IMPORT);
    REQUIRE(tokens[4].type == CHTL::TokenType::KEYWORD_NAMESPACE);
    REQUIRE(tokens[5].type == CHTL::TokenType::KEYWORD_CONFIGURATION);
    REQUIRE(tokens[6].type == CHTL::TokenType::KEYWORD_INFO);
    REQUIRE(tokens[7].type == CHTL::TokenType::KEYWORD_EXPORT);
    REQUIRE(tokens[8].type == CHTL::TokenType::KEYWORD_ORIGIN_TYPE);
    REQUIRE(tokens[9].type == CHTL::TokenType::KEYWORD_NAME);
}

TEST_CASE("Tokenize Simple Keywords", "[lexer]") {
    CHTL::CHTLLexer lexer;
    std::string input = "inherit delete insert after before replace from as except use html5 at top at bottom";
    std::vector<CHTL::Token> tokens = lexer.tokenize(input);

    REQUIRE(tokens.size() == 13);
    REQUIRE(tokens[0].type == CHTL::TokenType::KEYWORD_INHERIT);
    REQUIRE(tokens[1].type == CHTL::TokenType::KEYWORD_DELETE);
    REQUIRE(tokens[2].type == CHTL::TokenType::KEYWORD_INSERT);
    REQUIRE(tokens[3].type == CHTL::TokenType::KEYWORD_AFTER);
    REQUIRE(tokens[4].type == CHTL::TokenType::KEYWORD_BEFORE);
    REQUIRE(tokens[5].type == CHTL::TokenType::KEYWORD_REPLACE);
    REQUIRE(tokens[6].type == CHTL::TokenType::KEYWORD_FROM);
    REQUIRE(tokens[7].type == CHTL::TokenType::KEYWORD_AS);
    REQUIRE(tokens[8].type == CHTL::TokenType::KEYWORD_EXCEPT);
    REQUIRE(tokens[9].type == CHTL::TokenType::KEYWORD_USE);
    REQUIRE(tokens[10].type == CHTL::TokenType::KEYWORD_HTML5);
    REQUIRE(tokens[11].type == CHTL::TokenType::KEYWORD_AT_TOP);
    REQUIRE(tokens[12].type == CHTL::TokenType::KEYWORD_AT_BOTTOM);
}

TEST_CASE("Tokenize Type Keywords", "[lexer]") {
    CHTL::CHTLLexer lexer;
    std::string input = "@Style @Element @Var @Html @JavaScript @Chtl @Config @CJmod";
    std::vector<CHTL::Token> tokens = lexer.tokenize(input);

    REQUIRE(tokens.size() == 8);
    REQUIRE(tokens[0].type == CHTL::TokenType::TYPE_STYLE);
    REQUIRE(tokens[1].type == CHTL::TokenType::TYPE_ELEMENT);
    REQUIRE(tokens[2].type == CHTL::TokenType::TYPE_VAR);
    REQUIRE(tokens[3].type == CHTL::TokenType::TYPE_HTML);
    REQUIRE(tokens[4].type == CHTL::TokenType::TYPE_JAVASCRIPT);
    REQUIRE(tokens[5].type == CHTL::TokenType::TYPE_CHTL);
    REQUIRE(tokens[6].type == CHTL::TokenType::TYPE_CONFIG);
    REQUIRE(tokens[7].type == CHTL::TokenType::TYPE_CJMOD);
}

TEST_CASE("Tokenize CHTL JS and Multi-char Operators", "[lexer]") {
    CHTL::CHTLLexer lexer;
    std::string input = "{{ }} -> &-> && || != <= >= == ** += -= *= /=";
    std::vector<CHTL::Token> tokens = lexer.tokenize(input);

    REQUIRE(tokens.size() == 15);
    REQUIRE(tokens[0].type == CHTL::TokenType::DOUBLE_L_BRACE);
    REQUIRE(tokens[1].type == CHTL::TokenType::DOUBLE_R_BRACE);
    REQUIRE(tokens[2].type == CHTL::TokenType::ARROW);
    REQUIRE(tokens[3].type == CHTL::TokenType::EVENT_BIND_OP);
    REQUIRE(tokens[4].type == CHTL::TokenType::DOUBLE_AMPERSAND);
    REQUIRE(tokens[5].type == CHTL::TokenType::DOUBLE_PIPE);
    REQUIRE(tokens[6].type == CHTL::TokenType::NOT_EQUAL);
    REQUIRE(tokens[7].type == CHTL::TokenType::LESS_EQUAL);
    REQUIRE(tokens[8].type == CHTL::TokenType::GREATER_EQUAL);
    REQUIRE(tokens[9].type == CHTL::TokenType::DOUBLE_EQUAL);
    REQUIRE(tokens[10].type == CHTL::TokenType::STAR_STAR);
    REQUIRE(tokens[11].type == CHTL::TokenType::PLUS_EQUAL);
    REQUIRE(tokens[12].type == CHTL::TokenType::MINUS_EQUAL);
    REQUIRE(tokens[13].type == CHTL::TokenType::STAR_EQUAL);
    REQUIRE(tokens[14].type == CHTL::TokenType::SLASH_EQUAL);
}

TEST_CASE("Tokenize String Literals", "[lexer]") {
    CHTL::CHTLLexer lexer;
    std::string input = "\"double quoted\" 'single quoted'";
    std::vector<CHTL::Token> tokens = lexer.tokenize(input);

    REQUIRE(tokens.size() == 2);
    REQUIRE(tokens[0].type == CHTL::TokenType::STRING_LITERAL);
    REQUIRE(tokens[0].value == "double quoted");
    REQUIRE(tokens[1].type == CHTL::TokenType::STRING_LITERAL);
    REQUIRE(tokens[1].value == "single quoted");
}

TEST_CASE("Tokenize Unquoted Literal in Text Block", "[lexer]") {
    CHTL::CHTLLexer lexer;
    std::string input = "text {  Hello World 文本  }";
    std::vector<CHTL::Token> tokens = lexer.tokenize(input);

    REQUIRE(tokens.size() == 4);
    REQUIRE(tokens[0].type == CHTL::TokenType::KEYWORD_TEXT);
    REQUIRE(tokens[1].type == CHTL::TokenType::L_BRACE);
    REQUIRE(tokens[2].type == CHTL::TokenType::UNQUOTED_LITERAL);
    REQUIRE(tokens[2].value == "Hello World 文本");
    REQUIRE(tokens[3].type == CHTL::TokenType::R_BRACE);
}

TEST_CASE("Tokenize Punctuation", "[lexer]") {
    CHTL::CHTLLexer lexer;
    std::string input = "()[],.&$@#";
    std::vector<CHTL::Token> tokens = lexer.tokenize(input);

    REQUIRE(tokens.size() == 10);
    REQUIRE(tokens[0].type == CHTL::TokenType::L_PAREN);
    REQUIRE(tokens[1].type == CHTL::TokenType::R_PAREN);
    REQUIRE(tokens[2].type == CHTL::TokenType::L_BRACKET);
    REQUIRE(tokens[3].type == CHTL::TokenType::R_BRACKET);
    REQUIRE(tokens[4].type == CHTL::TokenType::COMMA);
    REQUIRE(tokens[5].type == CHTL::TokenType::DOT);
    REQUIRE(tokens[6].type == CHTL::TokenType::AMPERSAND);
    REQUIRE(tokens[7].type == CHTL::TokenType::DOLLAR);
    REQUIRE(tokens[8].type == CHTL::TokenType::AT);
    REQUIRE(tokens[9].type == CHTL::TokenType::HASH);
}