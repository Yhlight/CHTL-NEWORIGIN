#include <iostream>
#include <cassert>
#include "../../CHTL/CHTLLexer/Token.h"
#include "../../CHTL/CHTLLexer/GlobalMap.h"

using namespace CHTL;

int main() {
    std::cout << "Running CHTL Token Tests..." << std::endl;
    
    // Test Token creation
    Token token(TokenType::IDENTIFIER, "test", 1, 5);
    assert(token.getType() == TokenType::IDENTIFIER);
    assert(token.getValue() == "test");
    assert(token.getLine() == 1);
    assert(token.getColumn() == 5);
    std::cout << "✓ Token creation test passed" << std::endl;
    
    // Test Token classification
    assert(token.isLiteral());
    assert(!token.isOperator());
    assert(!token.isDelimiter());
    std::cout << "✓ Token classification test passed" << std::endl;
    
    // Test Token utilities
    assert(TokenUtils::isValidIdentifier("validIdentifier"));
    assert(!TokenUtils::isValidIdentifier("123invalid"));
    assert(TokenUtils::isValidStringLiteral("\"valid string\""));
    assert(!TokenUtils::isValidStringLiteral("invalid"));
    std::cout << "✓ TokenUtils validation test passed" << std::endl;
    
    // Test TokenFactory
    Token identifierToken = TokenFactory::createIdentifier("test", 1, 1);
    assert(identifierToken.getType() == TokenType::IDENTIFIER);
    assert(identifierToken.getValue() == "test");
    
    Token stringToken = TokenFactory::createStringLiteral("hello", 1, 1);
    assert(stringToken.getType() == TokenType::STRING_LITERAL);
    assert(stringToken.getValue() == "hello");
    std::cout << "✓ TokenFactory test passed" << std::endl;
    
    // Test GlobalMap
    GlobalMap& globalMap = GlobalMap::getInstance();
    TokenType type = globalMap.getTokenType("use");
    assert(type == TokenType::KEYWORD_USE);
    
    assert(globalMap.isKeyword("use"));
    assert(!globalMap.isKeyword("unknown"));
    std::cout << "✓ GlobalMap test passed" << std::endl;
    
    // Test TokenStream
    std::vector<Token> tokens = {
        TokenFactory::createIdentifier("test", 1, 1),
        TokenFactory::createOperator(TokenType::OPERATOR_EQUALS, "=", 1, 6),
        TokenFactory::createStringLiteral("value", 1, 8)
    };
    
    TokenStream stream(tokens);
    assert(stream.hasMore());
    
    Token firstToken = stream.consume();
    assert(firstToken.getValue() == "test");
    
    assert(stream.match(TokenType::OPERATOR_EQUALS));
    assert(stream.match("="));
    std::cout << "✓ TokenStream test passed" << std::endl;
    
    std::cout << "All token tests passed!" << std::endl;
    return 0;
}