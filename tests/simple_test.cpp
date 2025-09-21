#include <iostream>
#include <cassert>
#include "CHTLCommon.h"
#include "Token.h"
#include "ASTNode.h"
#include "CHTL/CHTLLexer/CHTLLexer.h"

using namespace CHTL;

void test_lexer() {
    std::cout << "Testing Lexer..." << std::endl;
    
    CHTLLexer lexer("div { text: \"Hello World\"; }", "test.chtl");
    auto tokens = lexer.tokenize();
    
    assert(tokens.size() > 0);
    assert(tokens[0]->getType() == TokenType::IDENTIFIER);
    assert(tokens[0]->getValue() == "div");
    
    std::cout << "  ✓ Basic tokenization passed" << std::endl;
    
    // Test string literal
    CHTLLexer lexer2("\"Hello World\"", "test.chtl");
    auto tokens2 = lexer2.tokenize();
    
    assert(tokens2.size() > 0);
    assert(tokens2[0]->getType() == TokenType::STRING_LITERAL);
    assert(tokens2[0]->getValue() == "Hello World");
    
    std::cout << "  ✓ String literal parsing passed" << std::endl;
    
    // Test operators
    CHTLLexer lexer3("+ - * / % **", "test.chtl");
    auto tokens3 = lexer3.tokenize();
    
    assert(tokens3.size() >= 6);
    assert(tokens3[0]->getType() == TokenType::PLUS);
    assert(tokens3[1]->getType() == TokenType::MINUS);
    assert(tokens3[2]->getType() == TokenType::MULTIPLY);
    assert(tokens3[3]->getType() == TokenType::DIVIDE);
    assert(tokens3[4]->getType() == TokenType::MODULO);
    assert(tokens3[5]->getType() == TokenType::POWER);
    
    std::cout << "  ✓ Operator parsing passed" << std::endl;
    
    std::cout << "All lexer tests passed!" << std::endl;
}

void test_ast_node() {
    std::cout << "Testing AST Node..." << std::endl;
    
    auto node = std::make_shared<ASTNode>(NodeType::ELEMENT, "div");
    
    assert(node->getType() == NodeType::ELEMENT);
    assert(node->getValue() == "div");
    assert(node->isLeaf());
    
    std::cout << "  ✓ Basic AST node creation passed" << std::endl;
    
    auto parent = std::make_shared<ASTNode>(NodeType::ELEMENT, "div");
    auto child = std::make_shared<ASTNode>(NodeType::TEXT, "Hello");
    
    parent->addChild(child);
    
    assert(parent->getChildCount() == 1);
    assert(!parent->isLeaf());
    assert(parent->isContainer());
    
    auto retrievedChild = parent->getChild(0);
    assert(retrievedChild->getValue() == "Hello");
    
    std::cout << "  ✓ AST node children management passed" << std::endl;
    
    std::cout << "All AST node tests passed!" << std::endl;
}

void test_string_util() {
    std::cout << "Testing String Utilities..." << std::endl;
    
    String test = "  hello world  ";
    String trimmed = StringUtil::trim(test);
    assert(trimmed == "hello world");
    
    std::cout << "  ✓ String trimming passed" << std::endl;
    
    StringVector parts = StringUtil::split("a,b,c", ',');
    assert(parts.size() == 3);
    assert(parts[0] == "a");
    assert(parts[1] == "b");
    assert(parts[2] == "c");
    
    std::cout << "  ✓ String splitting passed" << std::endl;
    
    assert(StringUtil::startsWith("hello world", "hello"));
    assert(!StringUtil::startsWith("hello world", "world"));
    
    std::cout << "  ✓ String prefix checking passed" << std::endl;
    
    assert(StringUtil::endsWith("hello world", "world"));
    assert(!StringUtil::endsWith("hello world", "hello"));
    
    std::cout << "  ✓ String suffix checking passed" << std::endl;
    
    std::cout << "All string utility tests passed!" << std::endl;
}

int main() {
    std::cout << "CHTL Simple Test Suite" << std::endl;
    std::cout << "======================" << std::endl;
    
    try {
        test_lexer();
        std::cout << std::endl;
        
        test_ast_node();
        std::cout << std::endl;
        
        test_string_util();
        std::cout << std::endl;
        
        std::cout << "All tests passed successfully!" << std::endl;
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Test failed: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Unknown test failure" << std::endl;
        return 1;
    }
}