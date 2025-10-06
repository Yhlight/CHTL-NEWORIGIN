#include "../third-party/catch.hpp"
#include "CHTL/CHTLParser/CHTLParser.h"
#include "CHTL/CHTLGenerator/CHTLGenerator.h"

using namespace CHTL;

TEST_CASE("CSS Rules - Expression Evaluation", "[css-rules][expression]") {
    SECTION("Arithmetic in CSS rules") {
        String source = R"(
            div {
                class: box;
                style {
                    .box {
                        width: 100px + 50px;
                        height: 200px - 50px;
                    }
                }
            }
        )";
        
        CHTLParser parser(source);
        auto ast = parser.parse();
        
        REQUIRE(!parser.hasErrors());
        
        String css = CssGenerator::generate(ast);
        
        // Expressions should be evaluated in CSS rules
        REQUIRE(css.find("150px") != String::npos);
    }
    
    SECTION("Complex expressions in CSS rules") {
        String source = R"(
            div {
                style {
                    .box {
                        width: (100px + 50px) * 2;
                        margin: 10px * 4 / 2;
                    }
                }
            }
        )";
        
        CHTLParser parser(source);
        auto ast = parser.parse();
        
        REQUIRE(!parser.hasErrors());
        
        String css = CssGenerator::generate(ast);
        
        REQUIRE(css.find("300px") != String::npos);  // (100px + 50px) * 2
        REQUIRE(css.find("20px") != String::npos);   // 10px * 4 / 2 = 40px / 2 = 20px
    }
    
    SECTION("Multiple properties with expressions") {
        String source = R"(
            div {
                style {
                    .box {
                        width: 50px * 2;
                        height: 200px / 4;
                        margin: 5px + 10px;
                        padding: 100px - 75px;
                    }
                }
            }
        )";
        
        CHTLParser parser(source);
        auto ast = parser.parse();
        
        REQUIRE(!parser.hasErrors());
        
        String css = CssGenerator::generate(ast);
        
        REQUIRE(css.find("100px") != String::npos);  // 50px * 2
        REQUIRE(css.find("50px") != String::npos);   // 200px / 4
        REQUIRE(css.find("15px") != String::npos);   // 5px + 10px
        REQUIRE(css.find("25px") != String::npos);   // 100px - 75px
    }
}

TEST_CASE("CSS Rules - calc() for different units", "[css-rules][expression]") {
    SECTION("Mixed units") {
        String source = R"(
            div {
                style {
                    .box {
                        width: 100px + 50%;
                    }
                }
            }
        )";
        
        CHTLParser parser(source);
        auto ast = parser.parse();
        
        REQUIRE(!parser.hasErrors());
        
        String css = CssGenerator::generate(ast);
        
        // Different units should use calc()
        REQUIRE(css.find("calc(") != String::npos);
    }
}

TEST_CASE("CSS Rules - Power operator", "[css-rules][expression]") {
    SECTION("Power operation") {
        String source = R"(
            div {
                style {
                    .box {
                        z-index: 2 ** 3;
                    }
                }
            }
        )";
        
        CHTLParser parser(source);
        auto ast = parser.parse();
        
        REQUIRE(!parser.hasErrors());
        
        String css = CssGenerator::generate(ast);
        
        // 2 ** 3 = 8
        REQUIRE(css.find("8") != String::npos);
    }
}
