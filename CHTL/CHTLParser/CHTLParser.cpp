#include "CHTLParser.h"
#include "../CHTLNode/DocumentNode.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
#include "../CHTLNode/StyleNode.h"
#include "../CHTLNode/ScriptNode.h"
#include "../CHTLNode/StylePropertyNode.h"
#include "../CHTLNode/SelectorBlockNode.h"
#include "../CHTLNode/LiteralNode.h"
#include "../CHTLNode/BinaryOpNode.h"
#include "../CHTLNode/ReferenceNode.h"
#include "../CHTLNode/TemplateDefinitionNode.h"
#include "../CHTLNode/TemplateUsageNode.h"
#include "../CHTLNode/VariableAccessNode.h"
#include "../CHTLNode/CustomDefinitionNode.h"
#include "../CHTLNode/DeleteNode.h"
#include "../CHTLNode/InsertNode.h"
#include "../CHTLManager/TemplateManager.h"
#include <stdexcept>
#include <vector>

namespace CHTL {

CHTLParser::CHTLParser(const std::vector<Token>& tokens) : tokens(tokens) {}

// --- Core Parsing Logic ---

std::unique_ptr<DocumentNode> CHTLParser::parse() {
    auto docNode = std::make_unique<DocumentNode>();
    parseUseDirective(docNode.get());

    while (!isAtEnd()) {
        auto statement = parseStatement();
        if (statement) {
            if (auto* def_node = dynamic_cast<TemplateDefinitionNode*>(statement.get())) {
                TemplateManager::getInstance().registerTemplate(def_node->getName(), std::unique_ptr<TemplateDefinitionNode>(static_cast<TemplateDefinitionNode*>(statement.release())));
            } else if (auto* custom_def_node = dynamic_cast<CustomDefinitionNode*>(statement.get())) {
                 TemplateManager::getInstance().registerTemplate(custom_def_node->getName(), std::unique_ptr<CustomDefinitionNode>(static_cast<CustomDefinitionNode*>(statement.release())));
            }else {
                docNode->children.push_back(std::move(statement));
            }
        }
    }

    return docNode;
}


// --- Token Manipulation Helpers ---

bool CHTLParser::isAtEnd() const {
    return current >= tokens.size() || peek().type == TokenType::EndOfFile;
}

Token CHTLParser::peek() const {
    if (current >= tokens.size()) return {TokenType::EndOfFile, ""};
    return tokens[current];
}

Token CHTLParser::advance() {
    if (!isAtEnd()) {
        current++;
    }
    return tokens[current - 1];
}

bool CHTLParser::check(TokenType type) const {
    if (isAtEnd()) return false;
    return peek().type == type;
}

bool CHTLParser::match(TokenType type) {
    if (check(type)) {
        advance();
        return true;
    }
    return false;
}

Token CHTLParser::consume(TokenType type, const std::string& message) {
    if (check(type)) {
        return advance();
    }
    throw std::runtime_error(message);
}

// --- Directive Parsing ---

void CHTLParser::parseUseDirective(DocumentNode* doc) {
    if (match(TokenType::USE_KEYWORD)) {
        if (match(TokenType::HTML5_KEYWORD)) {
            consume(TokenType::SEMICOLON, "Expect ';' after 'use html5' directive.");
            doc->useHtml5 = true;
        } else {
            throw std::runtime_error("Unknown 'use' directive.");
        }
    }
}

// --- Statement-Level Parsing ---

std::unique_ptr<BaseNode> CHTLParser::parseStatement() {
    while (match(TokenType::COMMENT)) {}
    if(isAtEnd()) return nullptr;

    if (match(TokenType::L_BRACKET)) {
        if (peek().type == TokenType::CUSTOM_KEYWORD) {
            return parseCustomDefinition();
        }
        return parseTemplateDefinition();
    }
    if (check(TokenType::IDENTIFIER)) {
        return parseElementNode();
    }
    if (match(TokenType::TEXT_KEYWORD)) {
        return parseTextNode();
    }
    if (match(TokenType::AT_SIGN)) {
        return parseTemplateUsage();
    }
    return nullptr;
}

std::unique_ptr<ElementNode> CHTLParser::parseElementNode() {
    Token tag = consume(TokenType::IDENTIFIER, "Expect element tag name.");
    auto element = std::make_unique<ElementNode>(tag.value);
    consume(TokenType::L_BRACE, "Expect '{' after element tag name.");

    while (!check(TokenType::R_BRACE) && !isAtEnd()) {
        if (match(TokenType::STYLE_KEYWORD)) {
            element->setStyle(parseStyleNode(element.get()));
        } else if (match(TokenType::SCRIPT_KEYWORD)) {
            element->setScript(parseScriptNode());
        } else if (match(TokenType::TEXT_KEYWORD)) {
            element->addChild(parseTextNode());
        } else if (match(TokenType::AT_SIGN)) {
            element->addChild(parseTemplateUsage());
        }
        else if (check(TokenType::IDENTIFIER)) {
            if (current + 1 < tokens.size() && (tokens[current + 1].type == TokenType::COLON || tokens[current + 1].type == TokenType::EQUAL)) {
                parseAttribute(element.get());
            } else {
                element->addChild(parseElementNode());
            }
        } else {
            throw std::runtime_error("Unexpected token in element block: " + peek().value);
        }
    }

    consume(TokenType::R_BRACE, "Expect '}' after element block.");
    return element;
}

void CHTLParser::parseAttribute(ElementNode* element) {
    Token key = consume(TokenType::IDENTIFIER, "Expect attribute key.");
    if (match(TokenType::COLON) || match(TokenType::EQUAL)) {
        std::string value_str;
        while (!isAtEnd() && peek().type != TokenType::SEMICOLON) {
            value_str += advance().value;
        }
        if (value_str.empty()) throw std::runtime_error("Expect attribute value.");
        element->addAttribute(key.value, value_str);
        consume(TokenType::SEMICOLON, "Expect ';' after attribute.");
    } else {
        throw std::runtime_error("Expect ':' or '=' after attribute key.");
    }
}

std::unique_ptr<BaseNode> CHTLParser::parseTextNode() {
    consume(TokenType::L_BRACE, "Expect '{' after 'text' keyword.");
    Token text = consume(TokenType::STRING_LITERAL, "Expect string literal inside text block.");
    consume(TokenType::R_BRACE, "Expect '}' after text block.");
    return std::make_unique<TextNode>(text.value);
}

std::unique_ptr<StyleNode> CHTLParser::parseStyleNode(ElementNode* parent) {
    auto styleNode = std::make_unique<StyleNode>();
    consume(TokenType::L_BRACE, "Expect '{' after 'style' keyword.");

    std::string last_selector;

    while (!check(TokenType::R_BRACE) && !isAtEnd()) {
        if (match(TokenType::AT_SIGN)) {
             styleNode->addTemplateUsage(parseTemplateUsage());
        }
        else if (match(TokenType::DOT)) {
            Token name = consume(TokenType::IDENTIFIER, "Expect class name after '.'.");
            last_selector = "." + name.value;
            parent->addAttribute("class", name.value);
            auto selectorBlock = std::make_unique<SelectorBlockNode>(last_selector);
            consume(TokenType::L_BRACE, "Expect '{' after selector name.");
            while (!check(TokenType::R_BRACE) && !isAtEnd()) {
                Token key = consume(TokenType::IDENTIFIER, "Expect style property key.");
                consume(TokenType::COLON, "Expect ':' after style property key.");
                selectorBlock->addProperty(std::make_unique<StylePropertyNode>(key.value, parseExpression()));
                consume(TokenType::SEMICOLON, "Expect ';' after style property value.");
            }
            consume(TokenType::R_BRACE, "Expect '}' after selector block.");
            styleNode->addSelectorBlock(std::move(selectorBlock));
        } else if (match(TokenType::HASH)) {
            Token name = consume(TokenType::IDENTIFIER, "Expect id name after '#'.");
            last_selector = "#" + name.value;
            parent->addAttribute("id", name.value);
            auto selectorBlock = std::make_unique<SelectorBlockNode>(last_selector);
            consume(TokenType::L_BRACE, "Expect '{' after selector name.");
            while (!check(TokenType::R_BRACE) && !isAtEnd()) {
                Token key = consume(TokenType::IDENTIFIER, "Expect style property key.");
                consume(TokenType::COLON, "Expect ':' after style property key.");
                selectorBlock->addProperty(std::make_unique<StylePropertyNode>(key.value, parseExpression()));
                consume(TokenType::SEMICOLON, "Expect ';' after style property value.");
            }
            consume(TokenType::R_BRACE, "Expect '}' after selector block.");
            styleNode->addSelectorBlock(std::move(selectorBlock));
        } else if (match(TokenType::AMPERSAND)) {
            if (last_selector.empty()) throw std::runtime_error("Found '&' without a preceding selector context.");
            std::string pseudo_selector_part;
            while (!check(TokenType::L_BRACE) && !isAtEnd()) {
                pseudo_selector_part += advance().value;
            }
            std::string full_selector = last_selector + pseudo_selector_part;
            auto selectorBlock = std::make_unique<SelectorBlockNode>(full_selector);
            consume(TokenType::L_BRACE, "Expect '{' after selector.");
            while (!check(TokenType::R_BRACE) && !isAtEnd()) {
                Token key = consume(TokenType::IDENTIFIER, "Expect style property key.");
                consume(TokenType::COLON, "Expect ':' after style property key.");
                selectorBlock->addProperty(std::make_unique<StylePropertyNode>(key.value, parseExpression()));
                consume(TokenType::SEMICOLON, "Expect ';' after style property value.");
            }
            consume(TokenType::R_BRACE, "Expect '}' after selector block.");
            styleNode->addSelectorBlock(std::move(selectorBlock));
        }
        else if (check(TokenType::IDENTIFIER)) {
            Token key = consume(TokenType::IDENTIFIER, "Expect style property key.");
            consume(TokenType::COLON, "Expect ':' after style property key.");
            auto value_expr = parseExpression();
            styleNode->addProperty(std::make_unique<StylePropertyNode>(key.value, std::move(value_expr)));
            consume(TokenType::SEMICOLON, "Expect ';' after style property value.");
        } else {
            throw std::runtime_error("Unexpected token in style block: " + peek().value);
        }
    }

    consume(TokenType::R_BRACE, "Expect '}' after style block.");
    return styleNode;
}


std::unique_ptr<ScriptNode> CHTLParser::parseScriptNode() {
    consume(TokenType::L_BRACE, "Expect '{' after 'script' keyword.");
    Token content = consume(TokenType::STRING_LITERAL, "Expect script content.");
    consume(TokenType::R_BRACE, "Expect '}' after script block.");
    return std::make_unique<ScriptNode>(content.value);
}

std::unique_ptr<TemplateDefinitionNode> CHTLParser::parseTemplateDefinition() {
    consume(TokenType::IDENTIFIER, "Expect 'Template' keyword.");
    consume(TokenType::R_BRACKET, "Expect ']' after 'Template'.");
    consume(TokenType::AT_SIGN, "Expect '@' after '[Template]'.");

    Token typeToken = consume(TokenType::IDENTIFIER, "Expect template type (Style, Element, or Var).");
    TemplateType type;
    if (typeToken.value == "Style") type = TemplateType::STYLE;
    else if (typeToken.value == "Element") type = TemplateType::ELEMENT;
    else if (typeToken.value == "Var") type = TemplateType::VAR;
    else throw std::runtime_error("Unknown template type: " + typeToken.value);

    Token nameToken = consume(TokenType::IDENTIFIER, "Expect template name.");
    auto templateNode = std::make_unique<TemplateDefinitionNode>(type, nameToken.value);

    consume(TokenType::L_BRACE, "Expect '{' after template definition.");

    while (!check(TokenType::R_BRACE) && !isAtEnd()) {
        if (type == TemplateType::ELEMENT) {
            templateNode->addChild(parseStatement());
        } else if (type == TemplateType::STYLE || type == TemplateType::VAR) {
            if (match(TokenType::INHERIT_KEYWORD)) {
                consume(TokenType::AT_SIGN, "Expect '@' after 'inherit' keyword.");
                templateNode->addChild(parseTemplateUsage());
            } else if (match(TokenType::AT_SIGN)) {
                templateNode->addChild(parseTemplateUsage());
            } else {
                Token key = consume(TokenType::IDENTIFIER, "Expect property or variable key.");
                consume(TokenType::COLON, "Expect ':' after key.");
                templateNode->addChild(std::make_unique<StylePropertyNode>(key.value, parseExpression()));
                consume(TokenType::SEMICOLON, "Expect ';' after value.");
            }
        }
    }

    consume(TokenType::R_BRACE, "Expect '}' after template block.");
    return templateNode;
}

std::unique_ptr<CustomDefinitionNode> CHTLParser::parseCustomDefinition() {
    consume(TokenType::CUSTOM_KEYWORD, "Expect 'Custom' keyword.");
    consume(TokenType::R_BRACKET, "Expect ']' after 'Custom'.");
    consume(TokenType::AT_SIGN, "Expect '@' after '[Custom]'.");

    Token typeToken = consume(TokenType::IDENTIFIER, "Expect template type (Style, Element, or Var).");
    TemplateType type;
    if (typeToken.value == "Style") type = TemplateType::STYLE;
    else if (typeToken.value == "Element") type = TemplateType::ELEMENT;
    else if (typeToken.value == "Var") type = TemplateType::VAR;
    else throw std::runtime_error("Unknown template type: " + typeToken.value);

    Token nameToken = consume(TokenType::IDENTIFIER, "Expect template name.");
    auto customNode = std::make_unique<CustomDefinitionNode>(type, nameToken.value);

    consume(TokenType::L_BRACE, "Expect '{' after custom definition.");

    while (!check(TokenType::R_BRACE) && !isAtEnd()) {
        if (match(TokenType::DELETE_KEYWORD)) {
            std::vector<std::string> targets;
            do {
                targets.push_back(consume(TokenType::IDENTIFIER, "Expect target for delete.").value);
            } while (match(TokenType::COMMA));
            customNode->addChild(std::make_unique<DeleteNode>(targets));
            consume(TokenType::SEMICOLON, "Expect ';' after delete statement.");
        } else if (match(TokenType::INSERT_KEYWORD)) {
            InsertPosition position;
            if (match(TokenType::AFTER_KEYWORD)) position = InsertPosition::AFTER;
            else if (match(TokenType::BEFORE_KEYWORD)) position = InsertPosition::BEFORE;
            else if (match(TokenType::REPLACE_KEYWORD)) position = InsertPosition::REPLACE;
            else if (match(TokenType::AT_KEYWORD)) {
                if (match(TokenType::TOP_KEYWORD)) position = InsertPosition::AT_TOP;
                else if (match(TokenType::BOTTOM_KEYWORD)) position = InsertPosition::AT_BOTTOM;
                else throw std::runtime_error("Expected 'top' or 'bottom' after 'at'.");
            } else {
                throw std::runtime_error("Invalid insert position.");
            }

            std::string selector;
            if (position != InsertPosition::AT_TOP && position != InsertPosition::AT_BOTTOM) {
                selector = consume(TokenType::IDENTIFIER, "Expect selector for insert.").value;
            }

            consume(TokenType::L_BRACE, "Expect '{' after insert statement.");
            std::vector<std::unique_ptr<BaseNode>> children;
            while (!check(TokenType::R_BRACE) && !isAtEnd()) {
                children.push_back(parseStatement());
            }
            consume(TokenType::R_BRACE, "Expect '}' after insert block.");
            customNode->addChild(std::make_unique<InsertNode>(position, selector, std::move(children)));
        } else {
             if (type == TemplateType::ELEMENT) {
                customNode->addChild(parseStatement());
            } else if (type == TemplateType::STYLE || type == TemplateType::VAR) {
                if (match(TokenType::INHERIT_KEYWORD)) {
                    consume(TokenType::AT_SIGN, "Expect '@' after 'inherit' keyword.");
                    customNode->addChild(parseTemplateUsage());
                } else if (match(TokenType::AT_SIGN)) {
                    customNode->addChild(parseTemplateUsage());
                } else {
                    Token key = consume(TokenType::IDENTIFIER, "Expect property or variable key.");
                    consume(TokenType::COLON, "Expect ':' after key.");
                    customNode->addChild(std::make_unique<StylePropertyNode>(key.value, parseExpression()));
                    consume(TokenType::SEMICOLON, "Expect ';' after value.");
                }
            }
        }
    }

    consume(TokenType::R_BRACE, "Expect '}' after custom block.");
    return customNode;
}

std::unique_ptr<TemplateUsageNode> CHTLParser::parseTemplateUsage() {
    Token typeToken = consume(TokenType::IDENTIFIER, "Expect template type (Style, Element, or Var).");
    TemplateType type;
    if (typeToken.value == "Style") type = TemplateType::STYLE;
    else if (typeToken.value == "Element") type = TemplateType::ELEMENT;
    else if (typeToken.value == "Var") type = TemplateType::VAR;
    else throw std::runtime_error("Unsupported template usage type: " + typeToken.value);

    Token nameToken = consume(TokenType::IDENTIFIER, "Expect template name.");

    if (match(TokenType::L_BRACE)) {
        std::vector<std::unique_ptr<BaseNode>> specializations;
        while(!check(TokenType::R_BRACE) && !isAtEnd()) {
            if (match(TokenType::DELETE_KEYWORD)) {
                std::vector<std::string> targets;
                do {
                    targets.push_back(consume(TokenType::IDENTIFIER, "Expect target for delete.").value);
                } while (match(TokenType::COMMA));
                specializations.push_back(std::make_unique<DeleteNode>(targets));
                consume(TokenType::SEMICOLON, "Expect ';' after delete statement.");
            } else if (match(TokenType::INSERT_KEYWORD)) {
                 InsertPosition position;
                if (match(TokenType::AFTER_KEYWORD)) position = InsertPosition::AFTER;
                else if (match(TokenType::BEFORE_KEYWORD)) position = InsertPosition::BEFORE;
                else if (match(TokenType::REPLACE_KEYWORD)) position = InsertPosition::REPLACE;
                else if (match(TokenType::AT_KEYWORD)) {
                    if (match(TokenType::TOP_KEYWORD)) position = InsertPosition::AT_TOP;
                    else if (match(TokenType::BOTTOM_KEYWORD)) position = InsertPosition::AT_BOTTOM;
                    else throw std::runtime_error("Expected 'top' or 'bottom' after 'at'.");
                } else {
                    throw std::runtime_error("Invalid insert position.");
                }

                std::string selector;
                if (position != InsertPosition::AT_TOP && position != InsertPosition::AT_BOTTOM) {
                    selector = consume(TokenType::IDENTIFIER, "Expect selector for insert.").value;
                }

                consume(TokenType::L_BRACE, "Expect '{' after insert statement.");
                std::vector<std::unique_ptr<BaseNode>> children;
                while (!check(TokenType::R_BRACE) && !isAtEnd()) {
                    children.push_back(parseStatement());
                }
                consume(TokenType::R_BRACE, "Expect '}' after insert block.");
                specializations.push_back(std::make_unique<InsertNode>(position, selector, std::move(children)));
            }
        }
        consume(TokenType::R_BRACE, "Expect '}' after specialization block.");
        return std::make_unique<TemplateUsageNode>(type, nameToken.value, std::move(specializations));
    }


    consume(TokenType::SEMICOLON, "Expect ';' after template usage.");
    return std::make_unique<TemplateUsageNode>(type, nameToken.value);
}


// --- Expression Parsing (Precedence Climbing) ---

std::unique_ptr<ExpressionNode> CHTLParser::parseExpression() {
    return parseTernary();
}

std::unique_ptr<ExpressionNode> CHTLParser::parseTernary() {
    auto expr = parseLogicalOr();
    if (match(TokenType::QUESTION_MARK)) {
        auto true_expr = parseExpression();
        consume(TokenType::COLON, "Expect ':' after true expression in ternary operator.");
        auto false_expr = parseExpression();
        return std::make_unique<ConditionalExpressionNode>(std::move(expr), std::move(true_expr), std::move(false_expr));
    }
    return expr;
}

std::unique_ptr<ExpressionNode> CHTLParser::parseLogicalOr() {
    auto expr = parseLogicalAnd();
    while (match(TokenType::PIPE_PIPE)) {
        Token op = tokens[current - 1];
        auto right = parseLogicalAnd();
        expr = std::make_unique<BinaryOpNode>(std::move(expr), op, std::move(right));
    }
    return expr;
}

std::unique_ptr<ExpressionNode> CHTLParser::parseLogicalAnd() {
    auto expr = parseEquality();
    while (match(TokenType::AMPERSAND_AMPERSAND)) {
        Token op = tokens[current - 1];
        auto right = parseEquality();
        expr = std::make_unique<BinaryOpNode>(std::move(expr), op, std::move(right));
    }
    return expr;
}

std::unique_ptr<ExpressionNode> CHTLParser::parseEquality() {
    auto expr = parseComparison();
    while (match(TokenType::EQUAL_EQUAL) || match(TokenType::BANG_EQUAL)) {
        Token op = tokens[current - 1];
        auto right = parseComparison();
        expr = std::make_unique<BinaryOpNode>(std::move(expr), op, std::move(right));
    }
    return expr;
}

std::unique_ptr<ExpressionNode> CHTLParser::parseComparison() {
    auto expr = parseTerm();
    while (match(TokenType::GREATER) || match(TokenType::GREATER_EQUAL) || match(TokenType::LESS) || match(TokenType::LESS_EQUAL)) {
        Token op = tokens[current - 1];
        auto right = parseTerm();
        expr = std::make_unique<BinaryOpNode>(std::move(expr), op, std::move(right));
    }
    return expr;
}

std::unique_ptr<ExpressionNode> CHTLParser::parseTerm() {
    auto expr = parseFactor();
    while (match(TokenType::PLUS) || match(TokenType::MINUS)) {
        Token op = tokens[current - 1];
        auto right = parseFactor();
        expr = std::make_unique<BinaryOpNode>(std::move(expr), op, std::move(right));
    }
    return expr;
}

std::unique_ptr<ExpressionNode> CHTLParser::parseFactor() {
    auto expr = parsePower();
    while (match(TokenType::STAR) || match(TokenType::SLASH) || match(TokenType::PERCENT)) {
        Token op = tokens[current - 1];
        auto right = parsePower();
        expr = std::make_unique<BinaryOpNode>(std::move(expr), op, std::move(right));
    }
    return expr;
}

std::unique_ptr<ExpressionNode> CHTLParser::parsePower() {
    auto expr = parsePrimary();
    if (match(TokenType::STAR_STAR)) {
        Token op = tokens[current - 1];
        auto right = parsePower();
        expr = std::make_unique<BinaryOpNode>(std::move(expr), op, std::move(right));
    }
    return expr;
}

std::unique_ptr<ExpressionNode> CHTLParser::parsePrimary() {
    if (match(TokenType::L_PAREN)) {
        auto expr = parseExpression();
        consume(TokenType::R_PAREN, "Expect ')' after expression.");
        return expr;
    }

    if (match(TokenType::HEX_LITERAL)) {
        return std::make_unique<LiteralNode>(tokens[current - 1], "");
    }

    if (match(TokenType::NUMBER)) {
        Token num_token = tokens[current - 1];
        std::string unit;
        if (check(TokenType::IDENTIFIER)) {
            unit = advance().value;
        } else if (check(TokenType::PERCENT)) {
            size_t next_token_idx = current + 1;
            if (next_token_idx >= tokens.size() ||
                (tokens[next_token_idx].type != TokenType::NUMBER &&
                 tokens[next_token_idx].type != TokenType::IDENTIFIER &&
                 tokens[next_token_idx].type != TokenType::L_PAREN)) {
                unit = advance().value;
            }
        }
        return std::make_unique<LiteralNode>(num_token, unit);
    }

    if (match(TokenType::HASH)) {
        Token name = consume(TokenType::IDENTIFIER, "Expect id name.");
        consume(TokenType::DOT, "Expect '.' after id name.");
        Token prop = consume(TokenType::IDENTIFIER, "Expect property name.");
        return std::make_unique<ReferenceNode>("#" + name.value, prop.value);
    }

    if (check(TokenType::DOT)) {
         if (current + 2 < tokens.size() && tokens[current+1].type == TokenType::IDENTIFIER && tokens[current+2].type == TokenType::DOT) {
            advance(); // consume dot
            Token name = consume(TokenType::IDENTIFIER, "Expect class name.");
            consume(TokenType::DOT, "Expect '.' after class name.");
            Token prop = consume(TokenType::IDENTIFIER, "Expect property name.");
            return std::make_unique<ReferenceNode>("." + name.value, prop.value);
        }
    }

    if (check(TokenType::IDENTIFIER)) {
        if (current + 1 < tokens.size() && tokens[current + 1].type == TokenType::L_PAREN) {
            Token groupName = consume(TokenType::IDENTIFIER, "Expect variable group name.");
            consume(TokenType::L_PAREN, "Expect '(' after variable group name.");
            Token variableName = consume(TokenType::IDENTIFIER, "Expect variable name.");
            consume(TokenType::R_PAREN, "Expect ')' after variable name.");
            return std::make_unique<VariableAccessNode>(groupName.value, variableName.value);
        }
        if (current + 1 < tokens.size() && tokens[current + 1].type == TokenType::DOT) {
            Token selector = advance();
            consume(TokenType::DOT, "Expect '.' after tag name.");
            Token prop = consume(TokenType::IDENTIFIER, "Expect property name.");
            return std::make_unique<ReferenceNode>(selector.value, prop.value);
        }
    }

    if (match(TokenType::IDENTIFIER) || match(TokenType::STRING_LITERAL)) {
        return std::make_unique<LiteralNode>(tokens[current - 1], "");
    }

    throw std::runtime_error("Expected expression, but found token: " + peek().value);
}

} // namespace CHTL