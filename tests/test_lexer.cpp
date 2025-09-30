#include "catch.hpp"
#include "CHTL/CHTLLexer/Lexer.h"
#include "CHTL/CHTLLexer/Token.h"

TEST_CASE("Lexer Basic Functionality", "[lexer]") {
    CHTL::Lexer lexer;
    
    SECTION("Empty input") {
        auto tokens = lexer.tokenize("");
        REQUIRE(tokens.size() == 1);
        REQUIRE(tokens[0].getType() == CHTL::TokenType::EOF_TOKEN);
    }
    
    SECTION("Simple identifier") {
        auto tokens = lexer.tokenize("div");
        REQUIRE(tokens.size() == 2);
        REQUIRE(tokens[0].getType() == CHTL::TokenType::IDENTIFIER);
        REQUIRE(tokens[0].getValue() == "div");
        REQUIRE(tokens[1].getType() == CHTL::TokenType::EOF_TOKEN);
    }
    
    SECTION("String literal") {
        auto tokens = lexer.tokenize("\"hello world\"");
        REQUIRE(tokens.size() == 2);
        REQUIRE(tokens[0].getType() == CHTL::TokenType::STRING_LITERAL);
        REQUIRE(tokens[0].getValue() == "hello world");
    }
    
    SECTION("Single quote literal") {
        auto tokens = lexer.tokenize("'hello world'");
        REQUIRE(tokens.size() == 2);
        REQUIRE(tokens[0].getType() == CHTL::TokenType::SINGLE_QUOTE_LITERAL);
        REQUIRE(tokens[0].getValue() == "hello world");
    }
    
    SECTION("Number literal") {
        auto tokens = lexer.tokenize("123");
        REQUIRE(tokens.size() == 2);
        REQUIRE(tokens[0].getType() == CHTL::TokenType::NUMBER_LITERAL);
        REQUIRE(tokens[0].getValue() == "123");
    }
    
    SECTION("Decimal number") {
        auto tokens = lexer.tokenize("123.45");
        REQUIRE(tokens.size() == 2);
        REQUIRE(tokens[0].getType() == CHTL::TokenType::NUMBER_LITERAL);
        REQUIRE(tokens[0].getValue() == "123.45");
    }
}

TEST_CASE("Lexer CHTL Keywords", "[lexer]") {
    CHTL::Lexer lexer;
    
    SECTION("Text keyword") {
        auto tokens = lexer.tokenize("text");
        REQUIRE(tokens.size() == 2);
        REQUIRE(tokens[0].getType() == CHTL::TokenType::TEXT_KEYWORD);
    }
    
    SECTION("Style keyword") {
        auto tokens = lexer.tokenize("style");
        REQUIRE(tokens.size() == 2);
        REQUIRE(tokens[0].getType() == CHTL::TokenType::STYLE_KEYWORD);
    }
    
    SECTION("Script keyword") {
        auto tokens = lexer.tokenize("script");
        REQUIRE(tokens.size() == 2);
        REQUIRE(tokens[0].getType() == CHTL::TokenType::SCRIPT_KEYWORD);
    }
    
    SECTION("Template keyword") {
        auto tokens = lexer.tokenize("[Template]");
        REQUIRE(tokens.size() == 2);
        REQUIRE(tokens[0].getType() == CHTL::TokenType::TEMPLATE_KEYWORD);
    }
    
    SECTION("Custom keyword") {
        auto tokens = lexer.tokenize("[Custom]");
        REQUIRE(tokens.size() == 2);
        REQUIRE(tokens[0].getType() == CHTL::TokenType::CUSTOM_KEYWORD);
    }
    
    SECTION("Import keyword") {
        auto tokens = lexer.tokenize("[Import]");
        REQUIRE(tokens.size() == 2);
        REQUIRE(tokens[0].getType() == CHTL::TokenType::IMPORT_KEYWORD);
    }
}

TEST_CASE("Lexer Comments", "[lexer]") {
    CHTL::Lexer lexer;
    
    SECTION("Line comment") {
        auto tokens = lexer.tokenize("// This is a comment");
        REQUIRE(tokens.size() == 2);
        REQUIRE(tokens[0].getType() == CHTL::TokenType::LINE_COMMENT);
        REQUIRE(tokens[0].getValue() == "// This is a comment");
    }
    
    SECTION("Block comment") {
        auto tokens = lexer.tokenize("/* This is a block comment */");
        REQUIRE(tokens.size() == 2);
        REQUIRE(tokens[0].getType() == CHTL::TokenType::BLOCK_COMMENT);
        REQUIRE(tokens[0].getValue() == "/* This is a block comment */");
    }
    
    SECTION("Generator comment") {
        auto tokens = lexer.tokenize("# This is a generator comment");
        REQUIRE(tokens.size() == 2);
        REQUIRE(tokens[0].getType() == CHTL::TokenType::GENERATOR_COMMENT);
        REQUIRE(tokens[0].getValue() == "# This is a generator comment");
    }
}

TEST_CASE("Lexer Operators", "[lexer]") {
    CHTL::Lexer lexer;
    
    SECTION("Single character operators") {
        auto tokens = lexer.tokenize(": = ; , . # & ? ! * + - / % ^ | ~");
        REQUIRE(tokens.size() == 16); // 15 operators + EOF
        
        REQUIRE(tokens[0].getType() == CHTL::TokenType::COLON);
        REQUIRE(tokens[1].getType() == CHTL::TokenType::EQUALS);
        REQUIRE(tokens[2].getType() == CHTL::TokenType::SEMICOLON);
        REQUIRE(tokens[3].getType() == CHTL::TokenType::COMMA);
        REQUIRE(tokens[4].getType() == CHTL::TokenType::DOT);
        REQUIRE(tokens[5].getType() == CHTL::TokenType::HASH);
        REQUIRE(tokens[6].getType() == CHTL::TokenType::AMPERSAND);
        REQUIRE(tokens[7].getType() == CHTL::TokenType::QUESTION);
        REQUIRE(tokens[8].getType() == CHTL::TokenType::EXCLAMATION);
        REQUIRE(tokens[9].getType() == CHTL::TokenType::ASTERISK);
        REQUIRE(tokens[10].getType() == CHTL::TokenType::PLUS);
        REQUIRE(tokens[11].getType() == CHTL::TokenType::MINUS);
        REQUIRE(tokens[12].getType() == CHTL::TokenType::SLASH);
        REQUIRE(tokens[13].getType() == CHTL::TokenType::PERCENT);
        REQUIRE(tokens[14].getType() == CHTL::TokenType::CARET);
    }
    
    SECTION("Multi-character operators") {
        auto tokens = lexer.tokenize("== != <= >= && || ++ -- += -= *= /= %= **=");
        REQUIRE(tokens.size() == 13); // 12 operators + EOF
        
        REQUIRE(tokens[0].getType() == CHTL::TokenType::EQUAL_EQUAL);
        REQUIRE(tokens[1].getType() == CHTL::TokenType::NOT_EQUAL);
        REQUIRE(tokens[2].getType() == CHTL::TokenType::LESS_EQUAL);
        REQUIRE(tokens[3].getType() == CHTL::TokenType::GREATER_EQUAL);
        REQUIRE(tokens[4].getType() == CHTL::TokenType::AND_AND);
        REQUIRE(tokens[5].getType() == CHTL::TokenType::OR_OR);
        REQUIRE(tokens[6].getType() == CHTL::TokenType::PLUS_PLUS);
        REQUIRE(tokens[7].getType() == CHTL::TokenType::MINUS_MINUS);
        REQUIRE(tokens[8].getType() == CHTL::TokenType::PLUS_EQUAL);
        REQUIRE(tokens[9].getType() == CHTL::TokenType::MINUS_EQUAL);
        REQUIRE(tokens[10].getType() == CHTL::TokenType::MULTIPLY_EQUAL);
        REQUIRE(tokens[11].getType() == CHTL::TokenType::DIVIDE_EQUAL);
    }
}

TEST_CASE("Lexer Brackets", "[lexer]") {
    CHTL::Lexer lexer;
    
    SECTION("All bracket types") {
        auto tokens = lexer.tokenize("() [] {} <>");
        REQUIRE(tokens.size() == 9); // 8 brackets + EOF
        
        REQUIRE(tokens[0].getType() == CHTL::TokenType::LEFT_PAREN);
        REQUIRE(tokens[1].getType() == CHTL::TokenType::RIGHT_PAREN);
        REQUIRE(tokens[2].getType() == CHTL::TokenType::LEFT_BRACKET);
        REQUIRE(tokens[3].getType() == CHTL::TokenType::RIGHT_BRACKET);
        REQUIRE(tokens[4].getType() == CHTL::TokenType::LEFT_BRACE);
        REQUIRE(tokens[5].getType() == CHTL::TokenType::RIGHT_BRACE);
        REQUIRE(tokens[6].getType() == CHTL::TokenType::LEFT_ANGLE);
        REQUIRE(tokens[7].getType() == CHTL::TokenType::RIGHT_ANGLE);
    }
}

TEST_CASE("Lexer Error Handling", "[lexer]") {
    CHTL::Lexer lexer;
    
    SECTION("Unterminated string") {
        auto tokens = lexer.tokenize("\"unterminated string");
        REQUIRE(lexer.hasError());
        REQUIRE(!tokens.empty());
        REQUIRE(tokens.back().getType() == CHTL::TokenType::ERROR_TOKEN);
    }
    
    SECTION("Unterminated block comment") {
        auto tokens = lexer.tokenize("/* unterminated comment");
        REQUIRE(lexer.hasError());
        REQUIRE(!tokens.empty());
        REQUIRE(tokens.back().getType() == CHTL::TokenType::ERROR_TOKEN);
    }
}

TEST_CASE("Lexer CHTL JS Keywords", "[lexer]") {
    CHTL::Lexer lexer;
    
    SECTION("CHTL JS keywords") {
        auto tokens = lexer.tokenize("Vir Listen Animate Router Delegate ScriptLoader");
        REQUIRE(tokens.size() == 7); // 6 keywords + EOF
        
        REQUIRE(tokens[0].getType() == CHTL::TokenType::VIR_KEYWORD);
        REQUIRE(tokens[1].getType() == CHTL::TokenType::LISTEN_KEYWORD);
        REQUIRE(tokens[2].getType() == CHTL::TokenType::ANIMATE_KEYWORD);
        REQUIRE(tokens[3].getType() == CHTL::TokenType::ROUTER_KEYWORD);
        REQUIRE(tokens[4].getType() == CHTL::TokenType::DELEGATE_KEYWORD);
        REQUIRE(tokens[5].getType() == CHTL::TokenType::SCRIPT_LOADER_KEYWORD);
    }
}

TEST_CASE("Lexer CSS Units", "[lexer]") {
    CHTL::Lexer lexer;
    
    SECTION("CSS units") {
        auto tokens = lexer.tokenize("px em rem % vh vw vmin vmax pt pc in cm mm");
        REQUIRE(tokens.size() == 13); // 12 units + EOF
        
        for (size_t i = 0; i < 12; ++i) {
            REQUIRE(tokens[i].getType() == CHTL::TokenType::CSS_UNIT);
        }
    }
}