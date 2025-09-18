#include "CHTLParser.h"
#include "../CHTLLexer/CHTLLexer.h"
#include "../CHTLLoader/CHTLLoader.h"
#include "../CHTLNode/CommentNode.h"
#include "../CHTLNode/Expression/LiteralExpression.h"
#include "../CHTLNode/Expression/BinaryOpExpression.h"
#include <stdexcept>
#include <iostream>
#include <map>
#include <algorithm>

namespace CHTL {

enum Precedence { LOWEST, SUM, PRODUCT, PREFIX };

static const std::map<TokenType, Precedence> precedences = {
    {TokenType::PLUS, SUM}, {TokenType::MINUS, SUM},
    {TokenType::STAR, PRODUCT}, {TokenType::SLASH, PRODUCT},
};

static Precedence getPrecedence(const Token& token) {
    if (precedences.count(token.type)) return precedences.at(token.type);
    return LOWEST;
}

CHTLParser::CHTLParser(std::vector<Token> tokens, std::shared_ptr<CHTLContext> context)
    : m_tokens(std::move(tokens)), m_context(std::move(context)) {
    m_eofToken.type = TokenType::END_OF_FILE;
    if (!m_tokens.empty()) {
        m_eofToken.line = m_tokens.back().line;
        m_eofToken.column = m_tokens.back().column + 1;
    }
}

// --- Token Manipulation ---
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

// --- Main Parsing Logic ---
std::unique_ptr<ElementNode> CHTLParser::parse() {
    auto root = std::make_unique<ElementNode>();
    root->tagName = "document_root";
    while (currentToken().type != TokenType::END_OF_FILE) {
        if (currentToken().type == TokenType::LEFT_BRACKET) {
            if (peekToken().type == TokenType::KEYWORD_IMPORT) {
                parseImportStatement();
            } else {
                parseTemplateDefinition();
            }
        } else {
            root->children.push_back(parseNode());
        }
    }
    return root;
}

std::unique_ptr<BaseNode> CHTLParser::parseNode() {
    switch(currentToken().type) {
        case TokenType::GENERATOR_COMMENT: {
            auto commentNode = std::make_unique<CommentNode>();
            commentNode->type = CommentType::GENERATOR;
            commentNode->content = currentToken().value;
            advance();
            return commentNode;
        }
        case TokenType::IDENTIFIER: {
            if (currentToken().value == "text" && peekToken().type == TokenType::LEFT_BRACE) {
                return parseTextNode();
            }
            return parseElement();
        }
        default:
            throw std::runtime_error("Unexpected token while parsing node: '" + currentToken().value + "' at Line: " + std::to_string(currentToken().line));
    }
}

std::unique_ptr<ElementNode> CHTLParser::parseElement() {
    auto element = std::make_unique<ElementNode>();
    element->tagName = consume(TokenType::IDENTIFIER, "Expected element tag name.").value;
    consume(TokenType::LEFT_BRACE, "Expected '{' after element name '" + element->tagName + "'.");

    while (currentToken().type != TokenType::RIGHT_BRACE && currentToken().type != TokenType::END_OF_FILE) {
        if (currentToken().type == TokenType::AT) {
            parseTemplateUsage(*element);
        } else if (currentToken().type == TokenType::KEYWORD_STYLE) {
            parseStyleBlock(*element);
        } else if (currentToken().type == TokenType::IDENTIFIER && (peekToken().type == TokenType::COLON || peekToken().type == TokenType::EQUALS)) {
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

void CHTLParser::parseStyleBlock(ElementNode& element) {
    consume(TokenType::KEYWORD_STYLE, "Expected 'style' keyword.");
    consume(TokenType::LEFT_BRACE, "Expected '{' after 'style' keyword.");

    while (currentToken().type != TokenType::RIGHT_BRACE && currentToken().type != TokenType::END_OF_FILE) {
        if (currentToken().type == TokenType::AT) {
            parseTemplateUsage(element);
        } else {
            StyleProperty prop;
            prop.key = consume(TokenType::IDENTIFIER, "Expected style property key.").value;
            consume(TokenType::COLON, "Expected ':' after style property key.");
            if (peekContainsOperator()) {
                prop.value = parseExpression();
            } else {
                std::string value_str;
                while(currentToken().type != TokenType::SEMICOLON && currentToken().type != TokenType::END_OF_FILE) {
                    if(!value_str.empty()) value_str += " ";
                    value_str += currentToken().value;
                    advance();
                }
                prop.value = std::make_unique<LiteralExpression>(Token{TokenType::IDENTIFIER, value_str});
            }
            consume(TokenType::SEMICOLON, "Expected ';' after style property value.");
            element.styles.push_back(std::move(prop));
        }
    }
    consume(TokenType::RIGHT_BRACE, "Expected '}' to close style block.");
}

// --- Helper & Sub-Parsers ---

Attribute CHTLParser::parseAttribute() {
    Attribute attr;
    attr.key = consume(TokenType::IDENTIFIER, "Expected attribute key.").value;
    if (match(TokenType::COLON) || match(TokenType::EQUALS)) {
        if (currentToken().type == TokenType::STRING_LITERAL || currentToken().type == TokenType::IDENTIFIER) {
            attr.value = currentToken().value;
            advance();
        } else {
            throw std::runtime_error("Expected attribute value for key '" + attr.key + "'.");
        }
    } else {
        throw std::runtime_error("Expected ':' or '=' after attribute key '" + attr.key + "'.");
    }
    consume(TokenType::SEMICOLON, "Expected ';' after attribute value.");
    return attr;
}

std::unique_ptr<TextNode> CHTLParser::parseTextNode() {
    consume(TokenType::IDENTIFIER, "Expected 'text' identifier.");
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

std::unique_ptr<TextNode> CHTLParser::parseTextAttributeAsNode() {
    consume(TokenType::IDENTIFIER, "Expected 'text' keyword.");
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

CHTLParser::ElementSelector CHTLParser::parseElementSelector() {
    ElementSelector selector;
    selector.tagName = consume(TokenType::IDENTIFIER, "Expected tag name for selector.").value;
    if (match(TokenType::LEFT_BRACKET)) {
        Token indexToken = consume(TokenType::IDENTIFIER, "Expected index in brackets.");
        try {
            selector.index = std::stoi(indexToken.value);
        } catch (const std::exception& e) {
            throw std::runtime_error("Invalid index in selector: " + indexToken.value);
        }
        consume(TokenType::RIGHT_BRACKET, "Expected ']' to close index selector.");
    }
    return selector;
}

void CHTLParser::parseElementSpecialization(std::vector<std::unique_ptr<BaseNode>>& nodes) {
    consume(TokenType::LEFT_BRACE, "Expected '{' for specialization block.");
    while(currentToken().type != TokenType::RIGHT_BRACE) {
        if (match(TokenType::KEYWORD_DELETE)) {
            do {
                ElementSelector selector = parseElementSelector();
                int match_count = 0;
                nodes.erase(
                    std::remove_if(nodes.begin(), nodes.end(),
                        [&](const std::unique_ptr<BaseNode>& node){
                            if (auto* el = dynamic_cast<ElementNode*>(node.get())) {
                                if (el->tagName == selector.tagName) {
                                    if (selector.index == -1 || selector.index == match_count) {
                                        match_count++;
                                        return true;
                                    }
                                    match_count++;
                                }
                            }
                            return false;
                        }),
                    nodes.end()
                );
            } while (match(TokenType::COMMA));
            consume(TokenType::SEMICOLON, "Expected ';' after delete statement.");
        } else {
            throw std::runtime_error("Only 'delete' is supported in element specialization for now.");
        }
    }
    consume(TokenType::RIGHT_BRACE, "Expected '}' to close specialization block.");
}

void CHTLParser::parseTemplateUsage(ElementNode& element) {
    consume(TokenType::AT, "Expected '@' for template usage.");
    Token type = consume(TokenType::IDENTIFIER, "Expected template type, e.g., 'Style' or 'Element'.");
    Token name = consume(TokenType::IDENTIFIER, "Expected template name.");

    if (type.value == "Style") {
        if (m_context->styleTemplates.count(name.value)) {
            auto& tmpl = m_context->styleTemplates.at(name.value);
            for(const auto& style_prop : tmpl->styles) {
                element.styles.push_back(StyleProperty{style_prop.key, style_prop.value->clone()});
            }
        } else {
            throw std::runtime_error("Undefined style template: " + name.value);
        }
    } else if (type.value == "Element") {
        if (m_context->elementTemplates.count(name.value)) {
            auto& tmpl = m_context->elementTemplates.at(name.value);
            std::vector<std::unique_ptr<BaseNode>> cloned_children;
            for (const auto& child : tmpl->children) {
                cloned_children.push_back(child->clone());
            }

            if (currentToken().type == TokenType::LEFT_BRACE) {
                parseElementSpecialization(cloned_children);
            }

            for(auto& child : cloned_children) {
                element.children.push_back(std::move(child));
            }
        } else {
            throw std::runtime_error("Undefined element template: " + name.value);
        }
    } else {
        throw std::runtime_error("Unknown template type: " + type.value);
    }

    if (currentToken().type == TokenType::SEMICOLON) {
        advance();
    }
}

void CHTLParser::parseTemplateDefinition() {
    consume(TokenType::LEFT_BRACKET, "Expected '[' to start template definition.");
    bool isCustom = match(TokenType::KEYWORD_CUSTOM);
    if (!isCustom) {
        consume(TokenType::KEYWORD_TEMPLATE, "Expected 'Template' or 'Custom' keyword.");
    }
    consume(TokenType::RIGHT_BRACKET, "Expected ']' to close template definition tag.");

    consume(TokenType::AT, "Expected '@' for template type.");
    Token type = consume(TokenType::IDENTIFIER, "Expected template type, e.g., 'Style' or 'Element'.");
    Token name = consume(TokenType::IDENTIFIER, "Expected template name.");
    consume(TokenType::LEFT_BRACE, "Expected '{' to open template body.");

    if (type.value == "Style") {
        auto styleTmpl = std::make_shared<StyleTemplateNode>(name.value);
        while(currentToken().type != TokenType::RIGHT_BRACE) {
            // Not handling style inheritance in this version
            StyleProperty prop;
            prop.key = consume(TokenType::IDENTIFIER, "Expected style property key.").value;
            consume(TokenType::COLON, "Expected ':' after style property key.");
            if (peekContainsOperator()) {
                prop.value = parseExpression();
            } else {
                std::string value_str;
                while(currentToken().type != TokenType::SEMICOLON && currentToken().type != TokenType::END_OF_FILE) {
                    if(!value_str.empty()) value_str += " ";
                    value_str += currentToken().value;
                    advance();
                }
                prop.value = std::make_unique<LiteralExpression>(Token{TokenType::IDENTIFIER, value_str});
            }
            consume(TokenType::SEMICOLON, "Expected ';' after style property value.");
            styleTmpl->styles.push_back(std::move(prop));
        }
        m_context->styleTemplates[name.value] = styleTmpl;
    } else if (type.value == "Element") {
        auto elementTmpl = std::make_shared<ElementTemplateNode>(name.value);
        while(currentToken().type != TokenType::RIGHT_BRACE) {
            elementTmpl->children.push_back(parseNode());
        }
        m_context->elementTemplates[name.value] = elementTmpl;
    } else {
        throw std::runtime_error("Unknown template type for definition: " + type.value);
    }
    consume(TokenType::RIGHT_BRACE, "Expected '}' to close template body.");
}

void CHTLParser::parseImportStatement() {
    consume(TokenType::LEFT_BRACKET, "Expected '[' for import statement.");
    consume(TokenType::KEYWORD_IMPORT, "Expected 'Import' keyword.");
    consume(TokenType::RIGHT_BRACKET, "Expected ']' to close import tag.");
    consume(TokenType::AT, "Expected '@' for import type.");
    Token type = consume(TokenType::IDENTIFIER, "Expected import type.");
    if (type.value != "Chtl") {
        throw std::runtime_error("Only '@Chtl' imports are supported for now.");
    }
    consume(TokenType::KEYWORD_FROM, "Expected 'from' keyword.");
    Token path = consume(TokenType::STRING_LITERAL, "Expected file path string.");
    match(TokenType::SEMICOLON);
    try {
        std::string source = CHTLLoader::readFile(path.value);
        CHTLLexer importedLexer(source);
        CHTLParser importedParser(importedLexer.getAllTokens(), m_context);
        importedParser.parse();
    } catch (const std::runtime_error& e) {
        throw std::runtime_error("Failed to import file '" + path.value + "': " + e.what());
    }
}

// --- Expression Parsing ---
std::unique_ptr<ExpressionNode> CHTLParser::parsePrefix() {
    if (currentToken().type == TokenType::IDENTIFIER || currentToken().type == TokenType::STRING_LITERAL) {
        return std::make_unique<LiteralExpression>(consume(currentToken().type, "Expected literal."));
    }
    throw std::runtime_error("Could not parse prefix expression for token: " + currentToken().value);
}

std::unique_ptr<ExpressionNode> CHTLParser::parseInfix(std::unique_ptr<ExpressionNode> left) {
    Token op = currentToken();
    advance();
    int precedence = getPrecedence(op);
    auto right = parseExpression(precedence);
    return std::make_unique<BinaryOpExpression>(std::move(left), op, std::move(right));
}

std::unique_ptr<ExpressionNode> CHTLParser::parseExpression(int precedence) {
    auto left = parsePrefix();
    while (currentToken().type != TokenType::SEMICOLON && currentToken().type != TokenType::RIGHT_BRACE && precedence < getPrecedence(currentToken())) {
        left = parseInfix(std::move(left));
    }
    return left;
}

bool CHTLParser::peekContainsOperator() const {
    size_t temp_pos = m_position;
    while (temp_pos < m_tokens.size() && m_tokens[temp_pos].type != TokenType::SEMICOLON) {
        TokenType type = m_tokens[temp_pos].type;
        if (type == TokenType::PLUS || type == TokenType::MINUS || type == TokenType::STAR || type == TokenType::SLASH) {
            return true;
        }
        temp_pos++;
    }
    return false;
}

} // namespace CHTL
