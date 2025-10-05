#include "CHTLParser.h"
#include "../CHTLNode/CommentNode.h"
#include "../CHTLNode/TextNode.h"
#include "../CHTLNode/ElementNode.h"

CHTLParser::CHTLParser(std::vector<Token> tokens) : tokens(std::move(tokens)) {}

std::vector<std::unique_ptr<Node>> CHTLParser::parse() {
    std::vector<std::unique_ptr<Node>> nodes;
    while (position < tokens.size()) {
        auto node = parseNode();
        if (node) {
            nodes.push_back(std::move(node));
        } else {
            position++;
        }
    }
    return nodes;
}

std::unique_ptr<Node> CHTLParser::parseNode() {
    if (position >= tokens.size()) return nullptr;

    const auto& token = tokens[position];
    switch (token.type) {
        case TokenType::SINGLE_LINE_COMMENT:
        case TokenType::MULTI_LINE_COMMENT:
        case TokenType::GENERATOR_COMMENT:
            position++;
            return std::make_unique<CommentNode>(token.value);

        case TokenType::TEXT_KEYWORD:
            if (position + 1 < tokens.size() && tokens[position + 1].type == TokenType::LEFT_BRACE) {
                 return parseTextNode();
            }
            return nullptr;

        case TokenType::UNQUOTED_LITERAL:
            if (position + 1 < tokens.size() && tokens[position + 1].type == TokenType::LEFT_BRACE) {
                return parseElementNode();
            }
            return nullptr;
        default:
            return nullptr;
    }
}

std::unique_ptr<Node> CHTLParser::parseElementNode() {
    auto element = std::make_unique<ElementNode>(tokens[position].value);
    position++;
    position++;

    while (position < tokens.size() && tokens[position].type != TokenType::RIGHT_BRACE) {
        const auto& token = tokens[position];
        if ((token.type == TokenType::UNQUOTED_LITERAL || token.type == TokenType::TEXT_KEYWORD) &&
            position + 1 < tokens.size() &&
            tokens[position + 1].type == TokenType::COLON)
        {
            std::string key = token.value;
            position += 2;

            std::string value;
            if (position < tokens.size()) {
                value = tokens[position].value;
                position++;
            }

            if (position < tokens.size() && tokens[position].type == TokenType::SEMICOLON) {
                position++;
            }
            element->setAttribute(key, value);
        } else {
            auto childNode = parseNode();
            if (childNode) {
                element->addChild(std::move(childNode));
            } else {
                position++;
            }
        }
    }

    if (position < tokens.size() && tokens[position].type == TokenType::RIGHT_BRACE) {
        position++;
    }

    return element;
}

std::unique_ptr<Node> CHTLParser::parseTextNode() {
    position++;
    position++;

    if (position < tokens.size()) {
        if (tokens[position].type == TokenType::STRING_LITERAL) {
            auto node = std::make_unique<TextNode>(tokens[position].value);
            position++;
            if (position < tokens.size() && tokens[position].type == TokenType::RIGHT_BRACE) {
                position++;
            }
            return node;
        } else {
            std::string text_content;
            while (position < tokens.size() && tokens[position].type != TokenType::RIGHT_BRACE) {
                text_content += tokens[position].value;
                if (position + 1 < tokens.size() && tokens[position + 1].type != TokenType::RIGHT_BRACE) {
                    text_content += " ";
                }
                position++;
            }

            if (position < tokens.size() && tokens[position].type == TokenType::RIGHT_BRACE) {
                position++;
            }

            if (!text_content.empty()) {
                return std::make_unique<TextNode>(text_content);
            }
        }
    }

    return std::make_unique<TextNode>("");
}