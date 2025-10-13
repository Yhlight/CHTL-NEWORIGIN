#include "../third-party/catch.hpp"
#include "../include/CHTL/Lexer.hpp"

using namespace CHTL;

TEST_CASE("Lexer基础功能", "[lexer]") {
    SECTION("识别标识符") {
        Lexer lexer("div span text");
        auto tokens = lexer.tokenize();
        
        REQUIRE(tokens.size() == 4); // 包括EOF
        REQUIRE(tokens[0].type() == TokenType::Identifier);
        REQUIRE(tokens[0].value() == "div");
        REQUIRE(tokens[1].type() == TokenType::Identifier);
        REQUIRE(tokens[1].value() == "span");
        REQUIRE(tokens[2].type() == TokenType::Text);
        REQUIRE(tokens[2].value() == "text");
    }
    
    SECTION("识别字符串") {
        Lexer lexer("\"Hello World\" 'Single Quote'");
        auto tokens = lexer.tokenize();
        
        REQUIRE(tokens.size() == 3);
        REQUIRE(tokens[0].type() == TokenType::String);
        REQUIRE(tokens[0].value() == "Hello World");
        REQUIRE(tokens[1].type() == TokenType::String);
        REQUIRE(tokens[1].value() == "Single Quote");
    }
    
    SECTION("识别数字") {
        Lexer lexer("100 3.14 50px 2em");
        auto tokens = lexer.tokenize();
        
        REQUIRE(tokens.size() == 5);
        REQUIRE(tokens[0].type() == TokenType::Number);
        REQUIRE(tokens[0].value() == "100");
        REQUIRE(tokens[1].type() == TokenType::Number);
        REQUIRE(tokens[1].value() == "3.14");
        REQUIRE(tokens[2].type() == TokenType::Number);
        REQUIRE(tokens[2].value() == "50px");
    }
    
    SECTION("识别操作符") {
        Lexer lexer("{ } [ ] ( ) : ; , . = + - * / %");
        auto tokens = lexer.tokenize();
        
        REQUIRE(tokens[0].type() == TokenType::LeftBrace);
        REQUIRE(tokens[1].type() == TokenType::RightBrace);
        REQUIRE(tokens[2].type() == TokenType::LeftBracket);
        REQUIRE(tokens[3].type() == TokenType::RightBracket);
        REQUIRE(tokens[4].type() == TokenType::LeftParen);
        REQUIRE(tokens[5].type() == TokenType::RightParen);
        REQUIRE(tokens[6].type() == TokenType::Colon);
        REQUIRE(tokens[7].type() == TokenType::Semicolon);
    }
    
    SECTION("识别注释") {
        Lexer lexer("// 这是注释\n/* 多行注释 */\n# 生成器注释");
        auto tokens = lexer.tokenize();
        
        REQUIRE(tokens.size() == 4);
        REQUIRE(tokens[0].type() == TokenType::Comment);
        REQUIRE(tokens[1].type() == TokenType::Comment);
        REQUIRE(tokens[2].type() == TokenType::GeneratorComment);
    }
    
    SECTION("识别关键字") {
        Lexer lexer("[Template] [Custom] [Origin] from as");
        auto tokens = lexer.tokenize();
        
        REQUIRE(tokens[0].type() == TokenType::Template);
        REQUIRE(tokens[1].type() == TokenType::Custom);
        REQUIRE(tokens[2].type() == TokenType::Origin);
        REQUIRE(tokens[3].type() == TokenType::From);
        REQUIRE(tokens[4].type() == TokenType::As);
    }
}

TEST_CASE("Lexer高级功能", "[lexer]") {
    SECTION("识别@标识符") {
        Lexer lexer("@Style @Element @Var @Html");
        auto tokens = lexer.tokenize();
        
        REQUIRE(tokens[0].type() == TokenType::AtStyle);
        REQUIRE(tokens[1].type() == TokenType::AtElement);
        REQUIRE(tokens[2].type() == TokenType::AtVar);
        REQUIRE(tokens[3].type() == TokenType::AtHtml);
    }
    
    SECTION("识别复合操作符") {
        Lexer lexer("** == != && || -> &->");
        auto tokens = lexer.tokenize();
        
        REQUIRE(tokens[0].type() == TokenType::Power);
        REQUIRE(tokens[1].type() == TokenType::EqualEqual);
        REQUIRE(tokens[2].type() == TokenType::NotEqual);
        REQUIRE(tokens[3].type() == TokenType::And);
        REQUIRE(tokens[4].type() == TokenType::Or);
        REQUIRE(tokens[5].type() == TokenType::Arrow);
        REQUIRE(tokens[6].type() == TokenType::EventBind);
    }
    
    SECTION("处理转义字符") {
        Lexer lexer("\"Hello\\nWorld\\t\"");
        auto tokens = lexer.tokenize();
        
        REQUIRE(tokens[0].type() == TokenType::String);
        REQUIRE(tokens[0].value() == "Hello\nWorld\t");
    }
}
