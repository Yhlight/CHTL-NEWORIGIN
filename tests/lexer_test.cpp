#include "catch.hpp"
#include "CHTL/CHTLLexer/Lexer.h"
#include "CHTL/CHTLLexer/Token.h"

using namespace CHTL;

TEST_CASE("Lexer - Basic Tokens", "[lexer]") {
    SECTION("Single character tokens") {
        Lexer lexer("{ } ( ) [ ] ; : , .");
        auto tokens = lexer.tokenize();
        
        REQUIRE(tokens.size() == 11);  // 10 tokens + EOF
        REQUIRE(tokens[0].getType() == TokenType::LeftBrace);
        REQUIRE(tokens[1].getType() == TokenType::RightBrace);
        REQUIRE(tokens[2].getType() == TokenType::LeftParen);
        REQUIRE(tokens[3].getType() == TokenType::RightParen);
        REQUIRE(tokens[4].getType() == TokenType::LeftBracket);
        REQUIRE(tokens[5].getType() == TokenType::RightBracket);
        REQUIRE(tokens[6].getType() == TokenType::Semicolon);
        REQUIRE(tokens[7].getType() == TokenType::Colon);
        REQUIRE(tokens[8].getType() == TokenType::Comma);
        REQUIRE(tokens[9].getType() == TokenType::Dot);
    }
    
    SECTION("Operators") {
        Lexer lexer("+ - * / % ** == != < > <= >= && || !");
        auto tokens = lexer.tokenize();
        
        REQUIRE(tokens[0].getType() == TokenType::Plus);
        REQUIRE(tokens[1].getType() == TokenType::Minus);
        REQUIRE(tokens[2].getType() == TokenType::Star);
        REQUIRE(tokens[3].getType() == TokenType::Slash);
        REQUIRE(tokens[4].getType() == TokenType::Percent);
        REQUIRE(tokens[5].getType() == TokenType::Power);
        REQUIRE(tokens[6].getType() == TokenType::EqualEqual);
        REQUIRE(tokens[7].getType() == TokenType::NotEqual);
        REQUIRE(tokens[8].getType() == TokenType::Less);
        REQUIRE(tokens[9].getType() == TokenType::Greater);
        REQUIRE(tokens[10].getType() == TokenType::LessEqual);
        REQUIRE(tokens[11].getType() == TokenType::GreaterEqual);
        REQUIRE(tokens[12].getType() == TokenType::And);
        REQUIRE(tokens[13].getType() == TokenType::Or);
        REQUIRE(tokens[14].getType() == TokenType::Not);
    }
}

TEST_CASE("Lexer - Literals", "[lexer]") {
    SECTION("String literals") {
        Lexer lexer(R"("hello" 'world')");
        auto tokens = lexer.tokenize();
        
        REQUIRE(tokens.size() == 3);  // 2 strings + EOF
        REQUIRE(tokens[0].getType() == TokenType::StringLiteral);
        REQUIRE(tokens[0].getValue() == "hello");
        REQUIRE(tokens[1].getType() == TokenType::StringLiteral);
        REQUIRE(tokens[1].getValue() == "world");
    }
    
    SECTION("Number literals") {
        Lexer lexer("42 3.14 -10 100px 50%");
        auto tokens = lexer.tokenize();
        
        REQUIRE(tokens[0].getType() == TokenType::NumberLiteral);
        REQUIRE(tokens[0].getValue() == "42");
        REQUIRE(tokens[1].getType() == TokenType::NumberLiteral);
        REQUIRE(tokens[1].getValue() == "3.14");
        REQUIRE(tokens[2].getType() == TokenType::NumberLiteral);
        REQUIRE(tokens[2].getValue() == "-10");
        REQUIRE(tokens[3].getType() == TokenType::NumberLiteral);
        REQUIRE(tokens[3].getValue() == "100px");
        REQUIRE(tokens[4].getType() == TokenType::NumberLiteral);
        REQUIRE(tokens[4].getValue() == "50%");
    }
    
    SECTION("Identifiers") {
        Lexer lexer("myVar _private $special test123");
        auto tokens = lexer.tokenize();
        
        REQUIRE(tokens[0].getType() == TokenType::Identifier);
        REQUIRE(tokens[0].getValue() == "myVar");
        REQUIRE(tokens[1].getType() == TokenType::Identifier);
        REQUIRE(tokens[1].getValue() == "_private");
        REQUIRE(tokens[2].getType() == TokenType::Identifier);
        REQUIRE(tokens[2].getValue() == "$special");
        REQUIRE(tokens[3].getType() == TokenType::Identifier);
        REQUIRE(tokens[3].getValue() == "test123");
    }
}

TEST_CASE("Lexer - Keywords", "[lexer]") {
    SECTION("CHTL keywords") {
        Lexer lexer("text style script");
        auto tokens = lexer.tokenize();
        
        REQUIRE(tokens[0].getType() == TokenType::TextKeyword);
        REQUIRE(tokens[1].getType() == TokenType::StyleKeyword);
        REQUIRE(tokens[2].getType() == TokenType::ScriptKeyword);
    }
    
    SECTION("HTML tags") {
        Lexer lexer("div span html body");
        auto tokens = lexer.tokenize();
        
        REQUIRE(tokens[0].getType() == TokenType::HtmlKeyword);
        REQUIRE(tokens[1].getType() == TokenType::HtmlKeyword);
        REQUIRE(tokens[2].getType() == TokenType::HtmlKeyword);
        REQUIRE(tokens[3].getType() == TokenType::HtmlKeyword);
    }
    
    SECTION("Bracket keywords") {
        Lexer lexer("[Template] [Custom] [Origin] [Import]");
        auto tokens = lexer.tokenize();
        
        REQUIRE(tokens[0].getType() == TokenType::Template);
        REQUIRE(tokens[1].getType() == TokenType::Custom);
        REQUIRE(tokens[2].getType() == TokenType::Origin);
        REQUIRE(tokens[3].getType() == TokenType::Import);
    }
    
    SECTION("@ keywords") {
        Lexer lexer("@Style @Element @Var @Html @JavaScript");
        auto tokens = lexer.tokenize();
        
        REQUIRE(tokens[0].getType() == TokenType::AtStyle);
        REQUIRE(tokens[1].getType() == TokenType::AtElement);
        REQUIRE(tokens[2].getType() == TokenType::AtVar);
        REQUIRE(tokens[3].getType() == TokenType::AtHtml);
        REQUIRE(tokens[4].getType() == TokenType::AtJavaScript);
    }
}

TEST_CASE("Lexer - Comments", "[lexer]") {
    SECTION("Line comments") {
        Lexer lexer("// this is a comment\n42");
        auto tokens = lexer.tokenize();
        
        REQUIRE(tokens[0].getType() == TokenType::LineComment);
        REQUIRE(tokens[1].getType() == TokenType::NumberLiteral);
    }
    
    SECTION("Block comments") {
        Lexer lexer("/* multi\nline\ncomment */ 42");
        auto tokens = lexer.tokenize();
        
        REQUIRE(tokens[0].getType() == TokenType::BlockComment);
        REQUIRE(tokens[1].getType() == TokenType::NumberLiteral);
    }
    
    SECTION("Generator comments") {
        Lexer lexer("# This is a generator comment\n42");
        auto tokens = lexer.tokenize();
        
        REQUIRE(tokens[0].getType() == TokenType::GeneratorComment);
        REQUIRE(tokens[0].getValue() == "This is a generator comment");
        REQUIRE(tokens[1].getType() == TokenType::NumberLiteral);
    }
}

TEST_CASE("Lexer - CHTL Structure", "[lexer]") {
    SECTION("Simple element") {
        String source = R"(
            div {
                text: "Hello World";
            }
        )";
        
        Lexer lexer(source);
        auto tokens = lexer.tokenize();
        
        REQUIRE(tokens[0].getType() == TokenType::HtmlKeyword);
        REQUIRE(tokens[0].getValue() == "div");
        REQUIRE(tokens[1].getType() == TokenType::LeftBrace);
        REQUIRE(tokens[2].getType() == TokenType::TextKeyword);
        REQUIRE(tokens[3].getType() == TokenType::Colon);
        REQUIRE(tokens[4].getType() == TokenType::StringLiteral);
        REQUIRE(tokens[4].getValue() == "Hello World");
        REQUIRE(tokens[5].getType() == TokenType::Semicolon);
        REQUIRE(tokens[6].getType() == TokenType::RightBrace);
    }
}

TEST_CASE("Lexer - Enhanced Selector", "[lexer]") {
    SECTION("Simple selector") {
        Lexer lexer("{{.box}}");
        auto tokens = lexer.tokenize();
        
        REQUIRE(tokens[0].getType() == TokenType::EnhancedSelector);
        REQUIRE(tokens[0].getValue() == ".box");
    }
    
    SECTION("Complex selector") {
        Lexer lexer("{{div.container button[0]}}");
        auto tokens = lexer.tokenize();
        
        REQUIRE(tokens[0].getType() == TokenType::EnhancedSelector);
        REQUIRE(tokens[0].getValue() == "div.container button[0]");
    }
}
