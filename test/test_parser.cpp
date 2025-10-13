#include "../third-party/catch.hpp"
#include "../include/CHTL/Lexer.hpp"
#include "../include/CHTL/Parser.hpp"

using namespace CHTL;

TEST_CASE("Parser基础功能", "[parser]") {
    SECTION("解析简单元素") {
        String source = R"(
            div {
                text { Hello World }
            }
        )";
        
        Lexer lexer(source);
        auto tokens = lexer.tokenize();
        
        Parser parser(tokens);
        auto ast = parser.parse();
        
        REQUIRE(ast != nullptr);
        REQUIRE(ast->type() == NodeType::Element);
        
        auto element = std::static_pointer_cast<ElementNode>(ast);
        REQUIRE(element->children().size() > 0);
    }
    
    SECTION("解析带属性的元素") {
        String source = R"(
            div {
                id: box;
                class: container;
            }
        )";
        
        Lexer lexer(source);
        auto tokens = lexer.tokenize();
        
        Parser parser(tokens);
        auto ast = parser.parse();
        
        REQUIRE(ast != nullptr);
        auto root = std::static_pointer_cast<ElementNode>(ast);
        REQUIRE(root->children().size() > 0);
        
        auto div = std::static_pointer_cast<ElementNode>(root->children()[0]);
        REQUIRE(div->tagName() == "div");
        REQUIRE(div->attributes().count("id") > 0);
        REQUIRE(div->attributes().at("id") == "box");
    }
    
    SECTION("解析嵌套元素") {
        String source = R"(
            html {
                head {
                    
                }
                body {
                    div {
                        span {
                            text { Content }
                        }
                    }
                }
            }
        )";
        
        Lexer lexer(source);
        auto tokens = lexer.tokenize();
        
        Parser parser(tokens);
        auto ast = parser.parse();
        
        REQUIRE(ast != nullptr);
        REQUIRE(ast->children().size() > 0);
    }
}

TEST_CASE("Parser高级功能", "[parser]") {
    SECTION("解析样式块") {
        String source = R"(
            div {
                style {
                    width: 100px;
                    height: 200px;
                    background-color: red;
                }
            }
        )";
        
        Lexer lexer(source);
        auto tokens = lexer.tokenize();
        
        Parser parser(tokens);
        auto ast = parser.parse();
        
        REQUIRE(ast != nullptr);
    }
    
    SECTION("解析模板") {
        String source = R"(
            [Template] @Style DefaultText {
                color: black;
                line-height: 1.6;
            }
        )";
        
        Lexer lexer(source);
        auto tokens = lexer.tokenize();
        
        Parser parser(tokens);
        auto ast = parser.parse();
        
        REQUIRE(ast != nullptr);
    }
    
    SECTION("解析导入") {
        String source = R"(
            [Import] @Style from styles.css as myStyles;
        )";
        
        Lexer lexer(source);
        auto tokens = lexer.tokenize();
        
        Parser parser(tokens);
        auto ast = parser.parse();
        
        REQUIRE(ast != nullptr);
    }
}
