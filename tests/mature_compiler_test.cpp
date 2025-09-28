#define CATCH_CONFIG_MAIN
#include "../catch.hpp"
#include "../src/Scanner/Tokenizer.h"
#include "../src/Core/CHTLJSPreprocessor.h"
#include "../src/CHTLJS/CHTLJSCompiler.h"
#include "../src/Util/TokenUtils.h"
#include <string>
#include <algorithm>

// Helper to remove all whitespace for easy comparison of generated code
std::string remove_whitespace(std::string str) {
    str.erase(std::remove(str.begin(), str.end(), ' '), str.end());
    str.erase(std::remove(str.begin(), str.end(), '\n'), str.end());
    str.erase(std::remove(str.begin(), str.end(), '\t'), str.end());
    str.erase(std::remove(str.begin(), str.end(), '\r'), str.end());
    return str;
}

TEST_CASE("Mature Tokenizer", "[tokenizer]") {
    Tokenizer tokenizer;
    std::string source = "Vir Test = iNeverAway { Void<A>: func };";
    std::vector<Token> tokens = tokenizer.tokenize(source);

    // This test now asserts the correct, granular tokenization.
    // "Vir", " ", "Test", " ", "=", " ", "iNeverAway", " ", "{", " ", "Void", "<", "A", ">", ":", " ", "func", " ", "}", ";"
    REQUIRE(tokens.size() == 20);
    REQUIRE(tokens[0].type == TokenType::VIR_KEYWORD);
    REQUIRE(tokens[2].type == TokenType::IDENTIFIER);
    REQUIRE(tokens[6].type == TokenType::CHTL_KEYWORD);
    REQUIRE(tokens[10].type == TokenType::IDENTIFIER); // Void
    REQUIRE(tokens[11].type == TokenType::SYMBOL);    // <
    REQUIRE(tokens[12].type == TokenType::IDENTIFIER); // A
    REQUIRE(tokens[13].type == TokenType::SYMBOL);    // >
    REQUIRE(tokens[14].type == TokenType::SYMBOL);    // :
}

TEST_CASE("Mature Preprocessor (iNeverAway)", "[preprocessor]") {
    CHTLJSPreprocessor preprocessor;
    CHTLJSCompiler compiler;
    Tokenizer tokenizer;
    std::string source = "Vir Test = iNeverAway { Run: function() { Animate{ target: {{.box}}, duration: 500 }; } }; Test->Run();";
    std::vector<Token> tokens = tokenizer.tokenize(source);

    preprocessor.process(tokens, compiler);

    std::string result;
    for(const auto& token : tokens) {
        result += token.text;
    }

    std::string expected_prefix = "function__chtl_global_func_0(){consttarget_elem=document.querySelector('.box');if(target_elem){constanim=target_elem.animate([],{duration:500,});}}";
    std::string expected_suffix = "__chtl_global_func_0();";

    REQUIRE(remove_whitespace(result) == remove_whitespace(expected_prefix + expected_suffix));
}

TEST_CASE("Mature Compiler", "[compiler]") {
    CHTLJSCompiler compiler;
    Tokenizer tokenizer;

    SECTION("Router History Mode") {
        std::string source = "Router { mode: 'history', url: '/', page:{{#home}} };";
        std::vector<Token> tokens = tokenizer.tokenize(source);
        std::string result = compiler.compile(tokens);
        std::string expected_start = "constroutes={'/':document.querySelector('#home'),};";
        std::string expected_history_part = "window.history.pushState";
        REQUIRE(remove_whitespace(result).rfind(expected_start, 0) == 0);
        REQUIRE(result.find(expected_history_part) != std::string::npos);
    }

    SECTION("Animate with Keyframes") {
        std::string source = "Animate { target: {{.box}}, duration: 1000, begin: { opacity: 1 }, end: { opacity: 0 } };";
        std::vector<Token> tokens = tokenizer.tokenize(source);
        std::string result = compiler.compile(tokens);
        std::string expected = "consttarget_elem=document.querySelector('.box');if(target_elem){constanim=target_elem.animate([{opacity:1,},{opacity:0,}],{duration:1000,});}";
        REQUIRE(remove_whitespace(result) == expected);
    }

    SECTION("Delegate with inline function") {
        std::string source = "{{#app}}->Delegate{target:'.btn',click: function(e){ console.log(e); }};";
        std::vector<Token> tokens = tokenizer.tokenize(source);
        std::string result = compiler.compile(tokens);
        std::string expected = "constparent_elem=document.querySelector('#app');if(parent_elem){parent_elem.addEventListener('click',(e)=>{if(e.target.matches(\".btn\")){(function(e){console.log(e);})(e);}});}";
        REQUIRE(remove_whitespace(result) == expected);
    }
}