#include "Parser.h"
#include <iostream>

namespace CHTL
{

    Parser::Parser(const std::vector<Token> &tokens) : tokens(tokens) {}

    std::vector<std::unique_ptr<BaseNode>> Parser::parse()
    {
        std::vector<std::unique_ptr<BaseNode>> statements;
        while (!isAtEnd())
        {
            auto statement = parseStatement();
            if (statement) { // Prevent adding nullptrs to the list
                statements.push_back(std::move(statement));
            }
        }
        return statements;
    }

    std::unique_ptr<BaseNode> Parser::parseStatement()
    {
        if (peek().type == TokenType::TOKEN_GENERATOR_COMMENT)
        {
            return parseCommentNode();
        }

        if (peek().type == TokenType::TOKEN_IDENTIFIER)
        {
            if (peek().lexeme == "text")
            {
                return parseTextNode();
            }
            if (peek().lexeme == "style")
            {
                return parseStyleNode();
            }
            return parseElement();
        }
        // Other statement types like [Import], [Template] will be handled here later.

        // For now, if it's not a known statement, it's an error or EOF.
        // We advance past any unknown tokens to prevent infinite loops.
        if (peek().type != TokenType::TOKEN_EOF) {
            std::cerr << "Error: Unexpected token '" << peek().lexeme << "' at the root level." << std::endl;
            advance();
        }
        return nullptr;
    }

    std::unique_ptr<ElementNode> Parser::parseElement()
    {
        const Token& nameToken = consume(TokenType::TOKEN_IDENTIFIER, "Expected element name.");
        auto element = std::make_unique<ElementNode>(nameToken.lexeme);

        consume(TokenType::TOKEN_LEFT_BRACE, "Expected '{' after element name.");

        while (!check(TokenType::TOKEN_RIGHT_BRACE) && !isAtEnd())
        {
            // Inside an element, we can have attributes or child elements.
            // Attribute: id: "value";
            // Child:     div { ... }
            if (peek().type == TokenType::TOKEN_IDENTIFIER && (tokens[current + 1].type == TokenType::TOKEN_COLON || tokens[current + 1].type == TokenType::TOKEN_EQUAL))
            {
                // Parse attribute
                const Token& key = consume(TokenType::TOKEN_IDENTIFIER, "Expected attribute key.");

                // Consume ':' or '='
                if (!match(TokenType::TOKEN_COLON) && !match(TokenType::TOKEN_EQUAL)) {
                    throw std::runtime_error("Expected ':' or '=' after attribute key.");
                }

                const Token& value = advance(); // Can be QUOTED_STRING or IDENTIFIER
                element->addAttribute(key.lexeme, value.lexeme);
                consume(TokenType::TOKEN_SEMICOLON, "Expected ';' after attribute value.");
            }
            else
            {
                // Parse child statement/node
                element->addChild(parseStatement());
            }
        }

        consume(TokenType::TOKEN_RIGHT_BRACE, "Expected '}' after element block.");
        return element;
    }

    std::unique_ptr<TextNode> Parser::parseTextNode()
    {
        consume(TokenType::TOKEN_IDENTIFIER, "Expected 'text' keyword."); // Consume 'text'
        consume(TokenType::TOKEN_LEFT_BRACE, "Expected '{' after 'text'.");

        std::string content;
        if (match(TokenType::TOKEN_QUOTED_STRING))
        {
            content = previous().lexeme;
        }
        else
        {
            // Handle unquoted literals, which might be multiple tokens.
            while (!check(TokenType::TOKEN_RIGHT_BRACE) && !isAtEnd())
            {
                content += advance().lexeme;
                if (!check(TokenType::TOKEN_RIGHT_BRACE)) {
                    content += " "; // Add space between reconstructed tokens.
                }
            }
            // Trim trailing space if it exists
            if (!content.empty() && content.back() == ' ') {
                content.pop_back();
            }
        }

        consume(TokenType::TOKEN_RIGHT_BRACE, "Expected '}' after text block.");
        return std::make_unique<TextNode>(content);
    }

    std::unique_ptr<StyleNode> Parser::parseStyleNode()
    {
        consume(TokenType::TOKEN_IDENTIFIER, "Expected 'style' keyword."); // consume 'style'
        auto styleNode = std::make_unique<StyleNode>();
        consume(TokenType::TOKEN_LEFT_BRACE, "Expected '{' after 'style'.");

        while (!check(TokenType::TOKEN_RIGHT_BRACE) && !isAtEnd())
        {
            const Token& key = consume(TokenType::TOKEN_IDENTIFIER, "Expected style property key.");
            consume(TokenType::TOKEN_COLON, "Expected ':' after style property key.");
            const Token& value = consume(TokenType::TOKEN_IDENTIFIER, "Expected style property value.");
            styleNode->addProperty(key.lexeme, value.lexeme);
            consume(TokenType::TOKEN_SEMICOLON, "Expected ';' after style property.");
        }

        consume(TokenType::TOKEN_RIGHT_BRACE, "Expected '}' after style block.");
        return styleNode;
    }

    std::unique_ptr<CommentNode> Parser::parseCommentNode()
    {
        const Token& token = consume(TokenType::TOKEN_GENERATOR_COMMENT, "Expected comment.");
        return std::make_unique<CommentNode>(token.lexeme, CommentNode::Type::GENERATOR);
    }

    // --- Helper Methods ---

    const Token &Parser::advance()
    {
        if (!isAtEnd())
            current++;
        return previous();
    }

    const Token &Parser::peek() const
    {
        return tokens[current];
    }

    const Token &Parser::previous() const
    {
        return tokens[current - 1];
    }

    bool Parser::isAtEnd() const
    {
        return peek().type == TokenType::TOKEN_EOF;
    }

    bool Parser::check(TokenType type) const
    {
        if (isAtEnd())
            return false;
        return peek().type == type;
    }

    bool Parser::match(TokenType type)
    {
        if (check(type))
        {
            advance();
            return true;
        }
        return false;
    }

    const Token &Parser::consume(TokenType type, const std::string &message)
    {
        if (check(type))
            return advance();
        throw std::runtime_error("Parse Error on line " + std::to_string(peek().line) + ": " + message);
    }
}
