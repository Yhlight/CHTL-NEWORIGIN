/**
 * @file test_element_parsing.cpp
 * @brief 测试完整的元素解析和HTML生成
 * 
 * TDD方式：先写测试，再实现功能
 */

#include "catch.hpp"
#include "CHTL/CHTLLexer/Lexer.h"
#include "CHTL/CHTLParser/Parser.h"
#include "CHTL/CHTLGenerator/Generator.h"
#include "CHTL/CHTLContext.h"

using namespace CHTL;

TEST_CASE("Simple element parsing", "[element][parser]") {
    SECTION("Empty div element") {
        std::string input = "div { }";
        
        CHTLContext context;
        Lexer lexer(input);
        auto tokens = lexer.tokenize();
        
        Parser parser(tokens, context);
        auto ast = parser.parse();
        
        REQUIRE(ast != nullptr);
        REQUIRE(ast->getType() == NodeType::ELEMENT);
        
        Generator generator(context);
        std::string html = generator.generateHTML(ast);
        
        REQUIRE(html == "<div></div>");
    }
    
    SECTION("Element with id attribute") {
        std::string input = R"(
            div {
                id: container;
            }
        )";
        
        CHTLContext context;
        Lexer lexer(input);
        auto tokens = lexer.tokenize();
        
        Parser parser(tokens, context);
        auto ast = parser.parse();
        
        REQUIRE(ast != nullptr);
        
        Generator generator(context);
        std::string html = generator.generateHTML(ast);
        
        REQUIRE(html.find("<div") != std::string::npos);
        REQUIRE(html.find("id=\"container\"") != std::string::npos);
        REQUIRE(html.find("</div>") != std::string::npos);
    }
    
    SECTION("Element with multiple attributes") {
        std::string input = R"(
            div {
                id: box;
                class: main-container;
            }
        )";
        
        CHTLContext context;
        Lexer lexer(input);
        auto tokens = lexer.tokenize();
        
        Parser parser(tokens, context);
        auto ast = parser.parse();
        
        REQUIRE(ast != nullptr);
        
        Generator generator(context);
        std::string html = generator.generateHTML(ast);
        
        REQUIRE(html.find("id=\"box\"") != std::string::npos);
        REQUIRE(html.find("class=\"main-container\"") != std::string::npos);
    }
}

TEST_CASE("Nested elements", "[element][parser]") {
    SECTION("Simple nesting") {
        std::string input = R"(
            div {
                span { }
            }
        )";
        
        CHTLContext context;
        Lexer lexer(input);
        auto tokens = lexer.tokenize();
        
        Parser parser(tokens, context);
        auto ast = parser.parse();
        
        REQUIRE(ast != nullptr);
        REQUIRE(ast->getChildren().size() == 1);
        
        Generator generator(context);
        std::string html = generator.generateHTML(ast);
        
        REQUIRE(html == "<div><span></span></div>");
    }
    
    SECTION("Multiple children") {
        std::string input = R"(
            div {
                span { }
                p { }
            }
        )";
        
        CHTLContext context;
        Lexer lexer(input);
        auto tokens = lexer.tokenize();
        
        Parser parser(tokens, context);
        auto ast = parser.parse();
        
        REQUIRE(ast != nullptr);
        REQUIRE(ast->getChildren().size() == 2);
        
        Generator generator(context);
        std::string html = generator.generateHTML(ast);
        
        REQUIRE(html.find("<span></span>") != std::string::npos);
        REQUIRE(html.find("<p></p>") != std::string::npos);
    }
}

TEST_CASE("Element with text content", "[element][text][parser]") {
    SECTION("Text block in element") {
        std::string input = R"(
            div {
                text {
                    "Hello World"
                }
            }
        )";
        
        CHTLContext context;
        Lexer lexer(input);
        auto tokens = lexer.tokenize();
        
        Parser parser(tokens, context);
        auto ast = parser.parse();
        
        REQUIRE(ast != nullptr);
        
        Generator generator(context);
        std::string html = generator.generateHTML(ast);
        
        REQUIRE(html == "<div>Hello World</div>");
    }
    
    SECTION("Text attribute in element") {
        std::string input = R"(
            div {
                text: "Hello World";
            }
        )";
        
        CHTLContext context;
        Lexer lexer(input);
        auto tokens = lexer.tokenize();
        
        Parser parser(tokens, context);
        auto ast = parser.parse();
        
        REQUIRE(ast != nullptr);
        
        Generator generator(context);
        std::string html = generator.generateHTML(ast);
        
        REQUIRE(html == "<div>Hello World</div>");
    }
}

TEST_CASE("Self-closing elements", "[element][parser]") {
    SECTION("BR element") {
        std::string input = "br { }";
        
        CHTLContext context;
        Lexer lexer(input);
        auto tokens = lexer.tokenize();
        
        Parser parser(tokens, context);
        auto ast = parser.parse();
        
        REQUIRE(ast != nullptr);
        
        Generator generator(context);
        std::string html = generator.generateHTML(ast);
        
        REQUIRE(html == "<br />");
    }
    
    SECTION("IMG element with attributes") {
        std::string input = R"(
            img {
                src: "image.png";
                alt: "An image";
            }
        )";
        
        CHTLContext context;
        Lexer lexer(input);
        auto tokens = lexer.tokenize();
        
        Parser parser(tokens, context);
        auto ast = parser.parse();
        
        REQUIRE(ast != nullptr);
        
        Generator generator(context);
        std::string html = generator.generateHTML(ast);
        
        REQUIRE(html.find("<img") != std::string::npos);
        REQUIRE(html.find("src=\"image.png\"") != std::string::npos);
        REQUIRE(html.find("alt=\"An image\"") != std::string::npos);
        REQUIRE(html.find("/>") != std::string::npos);
    }
}

TEST_CASE("Complete HTML document", "[element][parser][integration]") {
    std::string input = R"(
        html {
            head {
                title {
                    text: "My Page";
                }
            }
            
            body {
                div {
                    id: container;
                    class: main;
                    
                    h1 {
                        text: "Welcome";
                    }
                    
                    p {
                        text: "This is a paragraph.";
                    }
                }
            }
        }
    )";
    
    CHTLContext context;
    Lexer lexer(input);
    auto tokens = lexer.tokenize();
    
    Parser parser(tokens, context);
    auto ast = parser.parse();
    
    REQUIRE(ast != nullptr);
    
    Generator generator(context);
    std::string html = generator.generateHTML(ast);
    
    // 验证结构
    REQUIRE(html.find("<html>") != std::string::npos);
    REQUIRE(html.find("<head>") != std::string::npos);
    REQUIRE(html.find("<title>My Page</title>") != std::string::npos);
    REQUIRE(html.find("<body>") != std::string::npos);
    REQUIRE(html.find("id=\"container\"") != std::string::npos);
    REQUIRE(html.find("class=\"main\"") != std::string::npos);
    REQUIRE(html.find("<h1>Welcome</h1>") != std::string::npos);
    REQUIRE(html.find("<p>This is a paragraph.</p>") != std::string::npos);
    REQUIRE(html.find("</html>") != std::string::npos);
}
