#include "CHTLParser.h"
#include "../CHTLNode/CommentNode.h"
#include "../CHTLNode/Expression/LiteralExpression.h"
#include "../CHTLNode/Expression/BinaryOpExpression.h"
#include <stdexcept>
#include <iostream>
#include <map>

namespace CHTL {

// Operator precedence mapping for expression parsing.
enum Precedence {
    LOWEST,
    SUM,     // + or -
    PRODUCT, // * or /
    PREFIX   // -X or !X
};

static const std::map<TokenType, Precedence> precedences = {
    {TokenType::PLUS, SUM},
    {TokenType::MINUS, SUM},
    {TokenType::STAR, PRODUCT},
    {TokenType::SLASH, PRODUCT},
};

static Precedence getPrecedence(const Token& token) {
    if (precedences.count(token.type)) {
        return precedences.at(token.type);
    }
    return LOWEST;
}


CHTLParser::CHTLParser(std::vector<Token> tokens) : m_tokens(std::move(tokens)) {
    m_eofToken.type = TokenType::END_OF_FILE;
    if (!m_tokens.empty()) {
        m_eofToken.line = m_tokens.back().line;
        m_eofToken.column = m_tokens.back().column + 1;
    }
}

const Token& CHTLParser::currentToken() const {
    if (m_position >= m_tokens.size()) return m_eofToken;
    return m_tokens[m_position];
}

const Token& CHTLParser::peekToken() const {
    if (m_position + 1 >= m_tokens.size()) return m_eofToken;
    return m_tokens[m_position + 1];
}

void CHTLParser::advance() {
    if (m_position < m_tokens.size()) m_position++;
}

bool CHTLParser::match(TokenType type) {
    if (currentToken().type == type) {
        advance();
        return true;
    }
    return false;
}

const Token& CHTLParser::consume(TokenType type, const std::string& errorMessage) {
    const auto& token = currentToken();
    if (token.type == type) {
        advance();
        return m_tokens[m_position - 1];
    }
    throw std::runtime_error(errorMessage + " Got '" + token.value + "' instead. Line: " + std::to_string(token.line) + ", Col: " + std::to_string(token.column));
}

std::unique_ptr<ElementNode> CHTLParser::parse() {
    auto root = std::make_unique<ElementNode>();
    root->tagName = "document_root";
    while (currentToken().type != TokenType::END_OF_FILE) {
        root->children.push_back(parseNode());
    }
    return root;
}

std::unique_ptr<BaseNode> CHTLParser::parseNode() {
    if (currentToken().type == TokenType::GENERATOR_COMMENT) {
        auto commentNode = std::make_unique<CommentNode>();
        commentNode->type = CommentType::GENERATOR;
        commentNode->content = currentToken().value;
        advance();
        return commentNode;
    }
    if (currentToken().type == TokenType::IDENTIFIER) {
        // If the identifier is 'text' and it's followed by a brace,
        // parse it as a TextNode block, not a generic element.
        if (currentToken().value == "text" && peekToken().type == TokenType::LEFT_BRACE) {
            return parseTextNode();
        }
        return parseElement();
    }

    const auto& token = currentToken();
    throw std::runtime_error("Unexpected token while parsing node: '" + token.value + "' at Line: " + std::to_string(token.line));
}

Attribute CHTLParser::parseAttribute() {
    Attribute attr;
    attr.key = consume(TokenType::IDENTIFIER, "Expected attribute key.").value;

    if (!match(TokenType::COLON) && !match(TokenType::EQUALS)) {
        throw std::runtime_error("Expected ':' or '=' after attribute key '" + attr.key + "'.");
    }

    if (currentToken().type == TokenType::STRING_LITERAL || currentToken().type == TokenType::IDENTIFIER) {
        attr.value = currentToken().value;
        advance();
    } else {
        throw std::runtime_error("Expected attribute value for key '" + attr.key + "'.");
    }

    consume(TokenType::SEMICOLON, "Expected ';' after attribute value for key '" + attr.key + "'.");
    return attr;
}

std::unique_ptr<ExpressionNode> CHTLParser::parsePrefix() {
    // For now, we only handle literals as prefixes.
    // A more advanced parser would handle prefix operators like '-' or '!'.
    if (currentToken().type == TokenType::IDENTIFIER || currentToken().type == TokenType::STRING_LITERAL) {
        return std::make_unique<LiteralExpression>(consume(currentToken().type, "Expected literal."));
    }
    throw std::runtime_error("Could not parse prefix expression for token: " + currentToken().value);
}

std::unique_ptr<ExpressionNode> CHTLParser::parseInfix(std::unique_ptr<ExpressionNode> left) {
    Token op = consume(currentToken().type, "Expected infix operator.");
    int precedence = getPrecedence(op);
    auto right = parseExpression(precedence);
    return std::make_unique<BinaryOpExpression>(std::move(left), op, std::move(right));
}

std::unique_ptr<ExpressionNode> CHTLParser::parseExpression(int precedence) {
    auto left = parsePrefix();

    while (currentToken().type != TokenType::SEMICOLON && precedence < getPrecedence(currentToken())) {
        left = parseInfix(std::move(left));
    }

    return left;
}


void CHTLParser::parseStyleBlock(ElementNode& element) {
    consume(TokenType::KEYWORD_STYLE, "Expected 'style' keyword.");
    consume(TokenType::LEFT_BRACE, "Expected '{' after 'style' keyword.");

    while (currentToken().type != TokenType::RIGHT_BRACE && currentToken().type != TokenType::END_OF_FILE) {
        StyleProperty prop;
        prop.key = consume(TokenType::IDENTIFIER, "Expected style property key.").value;
        consume(TokenType::COLON, "Expected ':' after style property key.");

        prop.value = parseExpression();

        consume(TokenType::SEMICOLON, "Expected ';' after style property value.");
        element.styles.push_back(std::move(prop));
    }

    consume(TokenType::RIGHT_BRACE, "Expected '}' to close style block.");
}

std::unique_ptr<TextNode> CHTLParser::parseTextAttributeAsNode() {
    consume(TokenType::IDENTIFIER, "Expected 'text' keyword."); // Consume 'text'
    if (!match(TokenType::COLON) && !match(TokenType::EQUALS)) {
        throw std::runtime_error("Expected ':' or '=' after 'text' attribute.");
    }

    auto textNode = std::make_unique<TextNode>();
    if (currentToken().type == TokenType::STRING_LITERAL || currentToken().type == TokenType::IDENTIFIER) {
        textNode->content = currentToken().value;
        advance();
    } else {
        throw std::runtime_error("Expected string or unquoted literal for 'text' attribute value.");
    }

    consume(TokenType::SEMICOLON, "Expected ';' after 'text' attribute value.");
    return textNode;
}

std::unique_ptr<ElementNode> CHTLParser::parseElement() {
    auto element = std::make_unique<ElementNode>();
    element->tagName = consume(TokenType::IDENTIFIER, "Expected element tag name.").value;

    consume(TokenType::LEFT_BRACE, "Expected '{' after element name '" + element->tagName + "'.");

    while (currentToken().type != TokenType::RIGHT_BRACE && currentToken().type != TokenType::END_OF_FILE) {
        if (currentToken().type == TokenType::KEYWORD_STYLE) {
            parseStyleBlock(*element);
        } else if (currentToken().type == TokenType::GENERATOR_COMMENT) {
            element->children.push_back(parseNode());
        } else if (currentToken().type == TokenType::IDENTIFIER && (peekToken().type == TokenType::COLON || peekToken().type == TokenType::EQUALS)) {
            // Check for special 'text' attribute which is parsed as a child TextNode
            if (currentToken().value == "text") {
                element->children.push_back(parseTextAttributeAsNode());
            } else {
                element->attributes.push_back(parseAttribute());
            }
        } else {
            element->children.push_back(parseNode());
        }
    }

    consume(TokenType::RIGHT_BRACE, "Expected '}' to close element '" + element->tagName + "'.");

    return element;
}

std::unique_ptr<TextNode> CHTLParser::parseTextNode() {
    consume(TokenType::IDENTIFIER, "Expected 'text' identifier."); // Now expects IDENTIFIER, not KEYWORD_TEXT
    consume(TokenType::LEFT_BRACE, "Expected '{' after 'text' identifier.");

    auto textNode = std::make_unique<TextNode>();

    if (currentToken().type == TokenType::STRING_LITERAL || currentToken().type == TokenType::UNQUOTED_LITERAL || currentToken().type == TokenType::IDENTIFIER) {
        textNode->content = currentToken().value;
        advance();
    } else {
        throw std::runtime_error("Expected string or unquoted literal inside text block.");
    }

    consume(TokenType::RIGHT_BRACE, "Expected '}' to close text block.");

    return textNode;
}

} // namespace CHTL
