#include "CHTLParser.h"
#include "../CHTLNode/RootNode.h"
#include <stdexcept>

namespace CHTL {

    CHTLParser::CHTLParser(CHTLLexer& lexer)
        : lexer(lexer), currentToken({TokenType::UNKNOWN, "", 0, 0}) {
        advance();
    }

    void CHTLParser::advance() {
        currentToken = lexer.getNextToken();
    }

    void CHTLParser::eat(TokenType type) {
        if (currentToken.type == type) {
            advance();
        } else {
            throw std::runtime_error("Unexpected token: '" + currentToken.value + "'. Expected a different type.");
        }
    }

    std::unique_ptr<CommentNode> CHTLParser::parseCommentNode() {
        std::string text = currentToken.value;
        eat(TokenType::GENERATOR_COMMENT);
        return std::make_unique<CommentNode>(text);
    }

    std::unique_ptr<ElementNode> CHTLParser::parseElementNode() {
        std::string tagName = currentToken.value;
        eat(TokenType::IDENTIFIER);

        auto elementNode = std::make_unique<ElementNode>(tagName);

        eat(TokenType::LEFT_BRACE);

        while (currentToken.type != TokenType::RIGHT_BRACE && currentToken.type != TokenType::END_OF_FILE) {
            auto child = parseStatement();
            if (child) {
                elementNode->addChild(std::move(child));
            }
        }

        eat(TokenType::RIGHT_BRACE);

        return elementNode;
    }

    std::unique_ptr<TextNode> CHTLParser::parseTextNode() {
        eat(TokenType::IDENTIFIER); // Consume the 'text' identifier
        eat(TokenType::LEFT_BRACE);

        std::string textContent;
        if (currentToken.type == TokenType::STRING_LITERAL) {
            textContent = currentToken.value;
            advance();
        } else {
            bool firstToken = true;
            while (currentToken.type == TokenType::IDENTIFIER || currentToken.type == TokenType::NUMERIC_LITERAL) {
                if (!firstToken) {
                    textContent += " ";
                }
                textContent += currentToken.value;
                firstToken = false;
                advance();
            }
        }

        eat(TokenType::RIGHT_BRACE);

        return std::make_unique<TextNode>(textContent);
    }

    std::unique_ptr<BaseNode> CHTLParser::parseStatement() {
        if (currentToken.type == TokenType::GENERATOR_COMMENT) {
            return parseCommentNode();
        }

        if (currentToken.type == TokenType::IDENTIFIER) {
            if (currentToken.value == "text") {
                return parseTextNode();
            }
            // Any other identifier is an element tag
            return parseElementNode();
        }

        if (currentToken.type != TokenType::END_OF_FILE) {
             throw std::runtime_error("Unexpected token at start of statement: " + currentToken.value);
        }

        return nullptr;
    }

    std::unique_ptr<BaseNode> CHTLParser::parse() {
        auto root = std::make_unique<RootNode>();
        while(currentToken.type != TokenType::END_OF_FILE) {
             try {
                auto statement = parseStatement();
                if (statement) {
                    root->children.push_back(std::move(statement));
                } else {
                    break;
                }
            } catch (const std::runtime_error& e) {
                break;
            }
        }
        return root;
    }

} // namespace CHTL
