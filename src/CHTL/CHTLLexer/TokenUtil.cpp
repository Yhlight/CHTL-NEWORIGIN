#include "../../../include/Token.h"
#include <unordered_map>

namespace CHTL {

TokenType TokenUtil::stringToTokenType(const String& str) {
    static const std::unordered_map<String, TokenType> tokenMap = {
        // Basic tokens
        {"IDENTIFIER", TokenType::IDENTIFIER},
        {"STRING_LITERAL", TokenType::STRING_LITERAL},
        {"NUMBER_LITERAL", TokenType::NUMBER_LITERAL},
        {"COMMENT", TokenType::COMMENT},
        {"GENERATOR_COMMENT", TokenType::GENERATOR_COMMENT},
        
        // Operators
        {"COLON", TokenType::COLON},
        {"EQUAL", TokenType::EQUAL},
        {"SEMICOLON", TokenType::SEMICOLON},
        {"COMMA", TokenType::COMMA},
        {"DOT", TokenType::DOT},
        {"ARROW", TokenType::ARROW},
        
        // Brackets
        {"LEFT_BRACE", TokenType::LEFT_BRACE},
        {"RIGHT_BRACE", TokenType::RIGHT_BRACE},
        {"LEFT_PAREN", TokenType::LEFT_PAREN},
        {"RIGHT_PAREN", TokenType::RIGHT_PAREN},
        {"LEFT_BRACKET", TokenType::LEFT_BRACKET},
        {"RIGHT_BRACKET", TokenType::RIGHT_BRACKET},
        
        // Keywords
        {"TEXT", TokenType::TEXT},
        {"STYLE", TokenType::STYLE},
        {"SCRIPT", TokenType::SCRIPT},
        {"TEMPLATE", TokenType::TEMPLATE},
        {"CUSTOM", TokenType::CUSTOM},
        {"ORIGIN", TokenType::ORIGIN},
        {"IMPORT", TokenType::IMPORT},
        {"NAMESPACE", TokenType::NAMESPACE},
        {"CONFIGURATION", TokenType::CONFIGURATION},
        {"INHERIT", TokenType::INHERIT},
        {"DELETE", TokenType::DELETE},
        {"INSERT", TokenType::INSERT},
        {"FROM", TokenType::FROM},
        {"AS", TokenType::AS},
        {"EXCEPT", TokenType::EXCEPT},
        {"USE", TokenType::USE},
        {"HTML5", TokenType::HTML5},
        
        // Special
        {"AT_SYMBOL", TokenType::AT_SYMBOL},
        {"HASH_SYMBOL", TokenType::HASH_SYMBOL},
        {"DOLLAR_SYMBOL", TokenType::DOLLAR_SYMBOL},
        {"AMPERSAND_SYMBOL", TokenType::AMPERSAND_SYMBOL},
        
        // Arithmetic operators
        {"PLUS", TokenType::PLUS},
        {"MINUS", TokenType::MINUS},
        {"MULTIPLY", TokenType::MULTIPLY},
        {"DIVIDE", TokenType::DIVIDE},
        {"MODULO", TokenType::MODULO},
        {"POWER", TokenType::POWER},
        
        // Comparison operators
        {"EQUAL_TO", TokenType::EQUAL_TO},
        {"NOT_EQUAL", TokenType::NOT_EQUAL},
        {"LESS_THAN", TokenType::LESS_THAN},
        {"GREATER_THAN", TokenType::GREATER_THAN},
        {"LESS_EQUAL", TokenType::LESS_EQUAL},
        {"GREATER_EQUAL", TokenType::GREATER_EQUAL},
        
        // Logical operators
        {"AND", TokenType::AND},
        {"OR", TokenType::OR},
        {"NOT", TokenType::NOT},
        
        // Conditional
        {"QUESTION_MARK", TokenType::QUESTION_MARK},
        
        // End of file
        {"EOF_TOKEN", TokenType::EOF_TOKEN},
        
        // Unknown
        {"UNKNOWN", TokenType::UNKNOWN}
    };
    
    auto it = tokenMap.find(str);
    return (it != tokenMap.end()) ? it->second : TokenType::UNKNOWN;
}

String TokenUtil::tokenTypeToString(TokenType type) {
    static const std::unordered_map<TokenType, String> typeMap = {
        // Basic tokens
        {TokenType::IDENTIFIER, "IDENTIFIER"},
        {TokenType::STRING_LITERAL, "STRING_LITERAL"},
        {TokenType::NUMBER_LITERAL, "NUMBER_LITERAL"},
        {TokenType::COMMENT, "COMMENT"},
        {TokenType::GENERATOR_COMMENT, "GENERATOR_COMMENT"},
        
        // Operators
        {TokenType::COLON, "COLON"},
        {TokenType::EQUAL, "EQUAL"},
        {TokenType::SEMICOLON, "SEMICOLON"},
        {TokenType::COMMA, "COMMA"},
        {TokenType::DOT, "DOT"},
        {TokenType::ARROW, "ARROW"},
        
        // Brackets
        {TokenType::LEFT_BRACE, "LEFT_BRACE"},
        {TokenType::RIGHT_BRACE, "RIGHT_BRACE"},
        {TokenType::LEFT_PAREN, "LEFT_PAREN"},
        {TokenType::RIGHT_PAREN, "RIGHT_PAREN"},
        {TokenType::LEFT_BRACKET, "LEFT_BRACKET"},
        {TokenType::RIGHT_BRACKET, "RIGHT_BRACKET"},
        
        // Keywords
        {TokenType::TEXT, "TEXT"},
        {TokenType::STYLE, "STYLE"},
        {TokenType::SCRIPT, "SCRIPT"},
        {TokenType::TEMPLATE, "TEMPLATE"},
        {TokenType::CUSTOM, "CUSTOM"},
        {TokenType::ORIGIN, "ORIGIN"},
        {TokenType::IMPORT, "IMPORT"},
        {TokenType::NAMESPACE, "NAMESPACE"},
        {TokenType::CONFIGURATION, "CONFIGURATION"},
        {TokenType::INHERIT, "INHERIT"},
        {TokenType::DELETE, "DELETE"},
        {TokenType::INSERT, "INSERT"},
        {TokenType::FROM, "FROM"},
        {TokenType::AS, "AS"},
        {TokenType::EXCEPT, "EXCEPT"},
        {TokenType::USE, "USE"},
        {TokenType::HTML5, "HTML5"},
        
        // Special
        {TokenType::AT_SYMBOL, "AT_SYMBOL"},
        {TokenType::HASH_SYMBOL, "HASH_SYMBOL"},
        {TokenType::DOLLAR_SYMBOL, "DOLLAR_SYMBOL"},
        {TokenType::AMPERSAND_SYMBOL, "AMPERSAND_SYMBOL"},
        
        // Arithmetic operators
        {TokenType::PLUS, "PLUS"},
        {TokenType::MINUS, "MINUS"},
        {TokenType::MULTIPLY, "MULTIPLY"},
        {TokenType::DIVIDE, "DIVIDE"},
        {TokenType::MODULO, "MODULO"},
        {TokenType::POWER, "POWER"},
        
        // Comparison operators
        {TokenType::EQUAL_TO, "EQUAL_TO"},
        {TokenType::NOT_EQUAL, "NOT_EQUAL"},
        {TokenType::LESS_THAN, "LESS_THAN"},
        {TokenType::GREATER_THAN, "GREATER_THAN"},
        {TokenType::LESS_EQUAL, "LESS_EQUAL"},
        {TokenType::GREATER_EQUAL, "GREATER_EQUAL"},
        
        // Logical operators
        {TokenType::AND, "AND"},
        {TokenType::OR, "OR"},
        {TokenType::NOT, "NOT"},
        
        // Conditional
        {TokenType::QUESTION_MARK, "QUESTION_MARK"},
        
        // End of file
        {TokenType::EOF_TOKEN, "EOF_TOKEN"},
        
        // Unknown
        {TokenType::UNKNOWN, "UNKNOWN"}
    };
    
    auto it = typeMap.find(type);
    return (it != typeMap.end()) ? it->second : "UNKNOWN";
}

bool TokenUtil::isKeyword(const String& str) {
    static const std::unordered_set<String> keywords = {
        "text", "style", "script", "template", "custom", "origin",
        "import", "namespace", "configuration", "inherit", "delete",
        "insert", "from", "as", "except", "use", "html5",
        "true", "false", "null"
    };
    return keywords.find(str) != keywords.end();
}

bool TokenUtil::isOperator(const String& str) {
    static const std::unordered_set<String> operators = {
        ":", "=", "+", "-", "*", "/", "%", "**",
        "==", "!=", "<", ">", "<=", ">=",
        "&&", "||", "!", "?", ";", ",", ".", "->"
    };
    return operators.find(str) != operators.end();
}

bool TokenUtil::isBracket(const String& str) {
    static const std::unordered_set<String> brackets = {
        "{", "}", "(", ")", "[", "]"
    };
    return brackets.find(str) != brackets.end();
}

TokenType TokenUtil::getOperatorType(const String& op) {
    static const std::unordered_map<String, TokenType> operatorMap = {
        {":", TokenType::COLON},
        {"=", TokenType::EQUAL},
        {"+", TokenType::PLUS},
        {"-", TokenType::MINUS},
        {"*", TokenType::MULTIPLY},
        {"/", TokenType::DIVIDE},
        {"%", TokenType::MODULO},
        {"**", TokenType::POWER},
        {"==", TokenType::EQUAL_TO},
        {"!=", TokenType::NOT_EQUAL},
        {"<", TokenType::LESS_THAN},
        {">", TokenType::GREATER_THAN},
        {"<=", TokenType::LESS_EQUAL},
        {">=", TokenType::GREATER_EQUAL},
        {"&&", TokenType::AND},
        {"||", TokenType::OR},
        {"!", TokenType::NOT},
        {"?", TokenType::QUESTION_MARK},
        {";", TokenType::SEMICOLON},
        {",", TokenType::COMMA},
        {".", TokenType::DOT},
        {"->", TokenType::ARROW}
    };
    
    auto it = operatorMap.find(op);
    return (it != operatorMap.end()) ? it->second : TokenType::UNKNOWN;
}

int TokenUtil::getOperatorPrecedence(TokenType type) {
    static const std::unordered_map<TokenType, int> precedence = {
        {TokenType::POWER, 15},
        {TokenType::MULTIPLY, 13},
        {TokenType::DIVIDE, 13},
        {TokenType::MODULO, 13},
        {TokenType::PLUS, 12},
        {TokenType::MINUS, 12},
        {TokenType::LESS_THAN, 10},
        {TokenType::GREATER_THAN, 10},
        {TokenType::LESS_EQUAL, 10},
        {TokenType::GREATER_EQUAL, 10},
        {TokenType::EQUAL_TO, 9},
        {TokenType::NOT_EQUAL, 9},
        {TokenType::AND, 5},
        {TokenType::OR, 4},
        {TokenType::QUESTION_MARK, 3}
    };
    
    auto it = precedence.find(type);
    return (it != precedence.end()) ? it->second : 0;
}

bool TokenUtil::isLeftAssociative(TokenType type) {
    static const std::unordered_set<TokenType> leftAssociative = {
        TokenType::PLUS, TokenType::MINUS, TokenType::MULTIPLY,
        TokenType::DIVIDE, TokenType::MODULO, TokenType::AND, TokenType::OR,
        TokenType::EQUAL_TO, TokenType::NOT_EQUAL, TokenType::LESS_THAN,
        TokenType::GREATER_THAN, TokenType::LESS_EQUAL, TokenType::GREATER_EQUAL
    };
    return leftAssociative.find(type) != leftAssociative.end();
}

} // namespace CHTL