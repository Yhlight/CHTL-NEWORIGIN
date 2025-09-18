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


CHTLParser::CHTLParser(std::vector<Token> tokens, std::shared_ptr<CHTLContext> context)
    : m_tokens(std::move(tokens)), m_context(std::move(context)) {
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

void CHTLParser::parseImportStatement() {
    consume(TokenType::LEFT_BRACKET, "Expected '[' for import statement.");
    consume(TokenType::KEYWORD_IMPORT, "Expected 'Import' keyword.");
    consume(TokenType::RIGHT_BRACKET, "Expected ']' to close import tag.");

    // For now, we only support the simple form: [Import] @Chtl from "filepath";
    consume(TokenType::AT, "Expected '@' for import type.");
    Token type = consume(TokenType::IDENTIFIER, "Expected import type.");
    if (type.value != "Chtl") {
        throw std::runtime_error("Only '@Chtl' imports are supported for now.");
    }

    consume(TokenType::KEYWORD_FROM, "Expected 'from' keyword.");
    Token path = consume(TokenType::STRING_LITERAL, "Expected file path string.");

    match(TokenType::SEMICOLON); // Semicolon is optional for now.

    // --- Recursive Parsing ---
    try {
        std::string source = CHTLLoader::readFile(path.value);
        CHTLLexer importedLexer(source);
        // NOTE: We use the *same context* to populate it with the imported templates.
        CHTLParser importedParser(importedLexer.getAllTokens(), m_context);
        importedParser.parse(); // This will parse and populate the context.
    } catch (const std::runtime_error& e) {
        throw std::runtime_error("Failed to import file '" + path.value + "': " + e.what());
    }
}

std::unique_ptr<ElementNode> CHTLParser::parse() {
    auto root = std::make_unique<ElementNode>();
    root->tagName = "document_root";
    while (currentToken().type != TokenType::END_OF_FILE) {
        if (currentToken().type == TokenType::LEFT_BRACKET) {
            // It could be [Template], [Custom], or [Import]
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
    if (currentToken().type == TokenType::GENERATOR_COMMENT) {
        auto commentNode = std::make_unique<CommentNode>();
        commentNode->type = CommentType::GENERATOR;
        commentNode->content = currentToken().value;
        advance();
        return commentNode;
    }
    if (currentToken().type == TokenType::IDENTIFIER) {
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

void CHTLParser::parseDeletion(std::vector<StyleProperty>& styleList) {
    consume(TokenType::KEYWORD_DELETE, "Expected 'delete' keyword.");
    do {
        Token prop_to_delete = consume(TokenType::IDENTIFIER, "Expected property name to delete.");
        styleList.erase(
            std::remove_if(styleList.begin(), styleList.end(),
                [&](const StyleProperty& prop){ return prop.key == prop_to_delete.value; }),
            styleList.end()
        );
    } while (match(TokenType::COMMA));
    consume(TokenType::SEMICOLON, "Expected ';' after delete statement.");
}

void CHTLParser::parseTemplateUsage(ElementNode& element, std::vector<StyleProperty>& styleList) {
    consume(TokenType::AT, "Expected '@' for template usage.");
    Token type = consume(TokenType::IDENTIFIER, "Expected template type, e.g., 'Style' or 'Element'.");
    Token name = consume(TokenType::IDENTIFIER, "Expected template name.");

    if (type.value == "Style") {
        if (!m_context->styleTemplates.count(name.value)) {
            throw std::runtime_error("Undefined style template: " + name.value);
        }
        auto& tmpl = m_context->styleTemplates.at(name.value);
        // Deep copy the properties to avoid ownership issues.
        for(const auto& style_prop : tmpl->styles) {
            styleList.push_back(StyleProperty{style_prop.key, style_prop.value->clone()});
        }
    } else if (type.value == "Element") {
        throw std::runtime_error("@Element template expansion is not yet implemented.");
    } else {
        throw std::runtime_error("Unknown template type: " + type.value);
    }

    // Handle optional specialization block
    if (match(TokenType::LEFT_BRACE)) {
        while(currentToken().type != TokenType::RIGHT_BRACE) {
            if (currentToken().type == TokenType::KEYWORD_DELETE) {
                parseDeletion(styleList);
            } else {
                throw std::runtime_error("Only 'delete' is allowed inside a template specialization block for now.");
            }
        }
        consume(TokenType::RIGHT_BRACE, "Expected '}' to close specialization block.");
    } else {
        consume(TokenType::SEMICOLON, "Expected ';' after template usage.");
    }
}

void CHTLParser::parseStyleBlock(ElementNode& element) {
    consume(TokenType::KEYWORD_STYLE, "Expected 'style' keyword.");
    consume(TokenType::LEFT_BRACE, "Expected '{' after 'style' keyword.");

    while (currentToken().type != TokenType::RIGHT_BRACE && currentToken().type != TokenType::END_OF_FILE) {
        if (currentToken().type == TokenType::AT) {
             parseTemplateUsage(element, element.styles);
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
            if (currentToken().type == TokenType::AT) {
                // This is an inheritance case
                parseTemplateUsage(*std::make_unique<ElementNode>(), styleTmpl->styles);
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
                styleTmpl->styles.push_back(std::move(prop));
            }
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

std::unique_ptr<ElementNode> CHTLParser::parseElement() {
    auto element = std::make_unique<ElementNode>();
    element->tagName = consume(TokenType::IDENTIFIER, "Expected element tag name.").value;

    consume(TokenType::LEFT_BRACE, "Expected '{' after element name '" + element->tagName + "'.");

    while (currentToken().type != TokenType::RIGHT_BRACE && currentToken().type != TokenType::END_OF_FILE) {
        if (currentToken().type == TokenType::AT) {
             throw std::runtime_error("Element-level template usage is not yet supported here.");
        } else if (currentToken().type == TokenType::KEYWORD_STYLE) {
            parseStyleBlock(*element);
        } else if (currentToken().type == TokenType::GENERATOR_COMMENT) {
            element->children.push_back(parseNode());
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

} // namespace CHTL
