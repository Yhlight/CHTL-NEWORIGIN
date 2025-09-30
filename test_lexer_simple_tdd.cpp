#include <iostream>
#include <vector>
#include <string>
#include "CHTL/CHTLLexer.h"

// Simple test framework for TDD
#define ASSERT_EQ(expected, actual) \
    if ((expected) != (actual)) { \
        std::cout << "FAIL: Expected '" << (expected) << "', got '" << (actual) << "'" << std::endl; \
        return false; \
    }

#define ASSERT_TRUE(condition) \
    if (!(condition)) { \
        std::cout << "FAIL: Expected true, got false" << std::endl; \
        return false; \
    }

#define TEST_CASE(name) \
    bool test_##name() { \
        std::cout << "Testing " << #name << "..." << std::endl;

#define END_TEST_CASE \
        std::cout << "PASS" << std::endl; \
        return true; \
    }

// Test cases
TEST_CASE(basic_element_tokenization)
    CHTL::CHTLLexer lexer;
    lexer.tokenize("div");
    const auto& tokens = lexer.getTokens();
    
    ASSERT_EQ(2, tokens.size()); // div + EOF
    ASSERT_EQ("div", tokens[0].value);
END_TEST_CASE

TEST_CASE(element_with_attributes)
    CHTL::CHTLLexer lexer;
    lexer.tokenize("div class=\"container\"");
    const auto& tokens = lexer.getTokens();
    
    ASSERT_EQ(4, tokens.size()); // div + class + = + "container" + EOF
    ASSERT_EQ("div", tokens[0].value);
    ASSERT_EQ("class", tokens[1].value);
    ASSERT_EQ("=", tokens[2].value);
    ASSERT_EQ("container", tokens[3].value);
END_TEST_CASE

TEST_CASE(string_literals)
    CHTL::CHTLLexer lexer;
    lexer.tokenize("\"Hello World\"");
    const auto& tokens = lexer.getTokens();
    
    ASSERT_EQ(2, tokens.size()); // "Hello World" + EOF
    ASSERT_EQ("Hello World", tokens[0].value);
END_TEST_CASE

TEST_CASE(keywords)
    CHTL::CHTLLexer lexer;
    lexer.tokenize("text style template");
    const auto& tokens = lexer.getTokens();
    
    ASSERT_EQ(4, tokens.size()); // text + style + template + EOF
    ASSERT_EQ("text", tokens[0].value);
    ASSERT_EQ("style", tokens[1].value);
    ASSERT_EQ("template", tokens[2].value);
END_TEST_CASE

TEST_CASE(comments_single_line)
    CHTL::CHTLLexer lexer;
    lexer.tokenize("// This is a comment\ndiv");
    const auto& tokens = lexer.getTokens();
    
    ASSERT_EQ(2, tokens.size()); // div + EOF (comment should be skipped)
    ASSERT_EQ("div", tokens[0].value);
END_TEST_CASE

TEST_CASE(comments_multiline)
    CHTL::CHTLLexer lexer;
    lexer.tokenize("/* This is a\nmultiline comment */\ndiv");
    const auto& tokens = lexer.getTokens();
    
    ASSERT_EQ(2, tokens.size()); // div + EOF (comment should be skipped)
    ASSERT_EQ("div", tokens[0].value);
END_TEST_CASE

TEST_CASE(operators)
    CHTL::CHTLLexer lexer;
    lexer.tokenize("+ - * / = == != < >");
    const auto& tokens = lexer.getTokens();
    
    ASSERT_EQ(10, tokens.size()); // 9 operators + EOF
    ASSERT_EQ("+", tokens[0].value);
    ASSERT_EQ("-", tokens[1].value);
    ASSERT_EQ("*", tokens[2].value);
    ASSERT_EQ("/", tokens[3].value);
    ASSERT_EQ("=", tokens[4].value);
    ASSERT_EQ("==", tokens[5].value);
    ASSERT_EQ("!=", tokens[6].value);
    ASSERT_EQ("<", tokens[7].value);
    ASSERT_EQ(">", tokens[8].value);
END_TEST_CASE

TEST_CASE(numbers)
    CHTL::CHTLLexer lexer;
    lexer.tokenize("123 45.67 0x1A");
    const auto& tokens = lexer.getTokens();
    
    ASSERT_EQ(4, tokens.size()); // 123 + 45.67 + 0x1A + EOF
    ASSERT_EQ("123", tokens[0].value);
    ASSERT_EQ("45.67", tokens[1].value);
    ASSERT_EQ("0x1A", tokens[2].value);
END_TEST_CASE

TEST_CASE(special_characters)
    CHTL::CHTLLexer lexer;
    lexer.tokenize("{}[]();");
    const auto& tokens = lexer.getTokens();
    
    ASSERT_EQ(8, tokens.size()); // 7 special chars + EOF
    ASSERT_EQ("{", tokens[0].value);
    ASSERT_EQ("}", tokens[1].value);
    ASSERT_EQ("[", tokens[2].value);
    ASSERT_EQ("]", tokens[3].value);
    ASSERT_EQ("(", tokens[4].value);
    ASSERT_EQ(")", tokens[5].value);
    ASSERT_EQ(";", tokens[6].value);
END_TEST_CASE

int main() {
    std::cout << "=== CHTL Lexer TDD Tests ===" << std::endl;
    
    bool allPassed = true;
    
    allPassed &= test_basic_element_tokenization();
    allPassed &= test_element_with_attributes();
    allPassed &= test_string_literals();
    allPassed &= test_keywords();
    allPassed &= test_comments_single_line();
    allPassed &= test_comments_multiline();
    allPassed &= test_operators();
    allPassed &= test_numbers();
    allPassed &= test_special_characters();
    
    std::cout << "\n=== Test Results ===" << std::endl;
    if (allPassed) {
        std::cout << "All tests PASSED!" << std::endl;
        return 0;
    } else {
        std::cout << "Some tests FAILED!" << std::endl;
        return 1;
    }
}