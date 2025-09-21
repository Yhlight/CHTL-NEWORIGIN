#include "Parser.h"
#include <stdexcept>

namespace CHTL {

Parser::Parser(const std::vector<Token>& tokens, Context& context)
    : m_tokens(tokens), m_context(context) {}

std::unique_ptr<ElementNode> Parser::parse() {
    // At the top level, we can have template declarations or a single root element.
    while (peek().type == TokenType::OpenBracket) {
        auto template_decl = parseTemplateDeclaration();
        m_context.addTemplate(std::move(template_decl));
    }

    if (isAtEnd() || peek().type != TokenType::Identifier) {
        return nullptr; // Or throw error if a root element is always required
    }

    auto node = parseNode(nullptr); // No parent for the root
    if(node->getType() != NodeType::Element) {
        throw std::runtime_error("Root of the document must be an element.");
    }
    return std::unique_ptr<ElementNode>(static_cast<ElementNode*>(node.release()));
}

std::shared_ptr<TemplateDeclarationNode> Parser::parseTemplateDeclaration() {
    consume(TokenType::OpenBracket, "Expected '[' for template declaration.");
    if (consume(TokenType::Identifier, "Expected 'Template' keyword.").value != "Template") {
        throw std::runtime_error("Expected 'Template' keyword.");
    }
    consume(TokenType::CloseBracket, "Expected ']' after 'Template' keyword.");

    consume(TokenType::At, "Expected '@' for template type.");

    auto decl = std::make_shared<TemplateDeclarationNode>();
    std::string type_str = consume(TokenType::Identifier, "Expected template type (Style, Element, Var).").value;
    if (type_str == "Style") decl->templateType = TemplateType::Style;
    else if (type_str == "Element") decl->templateType = TemplateType::Element;
    else if (type_str == "Var") decl->templateType = TemplateType::Var;
    else throw std::runtime_error("Unknown template type: " + type_str);

    decl->name = consume(TokenType::Identifier, "Expected template name.").value;
    consume(TokenType::OpenBrace, "Expected '{' for template body.");

    while(!check(TokenType::CloseBrace)) {
        if (decl->templateType == TemplateType::Style) {
            decl->body.push_back(parseStyleProperty());
        } else {
            decl->body.push_back(parseNode(nullptr)); // No parent context inside templates for now
        }
    }

    consume(TokenType::CloseBrace, "Expected '}' to close template body.");
    return decl;
}

std::unique_ptr<StylePropertyNode> Parser::parseStyleProperty() {
    auto prop = std::make_unique<StylePropertyNode>();
    prop->key = consume(TokenType::Identifier, "Expected style property key.").value;
    consume(TokenType::Colon, "Expected ':' after property key.");
    prop->value = consume(TokenType::StringLiteral, "Expected string literal for property value.").value;
    consume(TokenType::Semicolon, "Expected ';' after property value.");
    return prop;
}

std::unique_ptr<TemplateInstantiationNode> Parser::parseTemplateInstantiation(ElementNode* parent) {
    consume(TokenType::At, "Expected '@'.");
    auto inst = std::make_unique<TemplateInstantiationNode>();

    std::string type_str = consume(TokenType::Identifier, "Expected template type.").value;
    if (type_str == "Style") inst->templateType = TemplateType::Style;
    else if (type_str == "Element") inst->templateType = TemplateType::Element;
    else if (type_str == "Var") inst->templateType = TemplateType::Var;
    else throw std::runtime_error("Unknown template type: " + type_str);

    inst->name = consume(TokenType::Identifier, "Expected template name.").value;
    consume(TokenType::Semicolon, "Expected ';' after template instantiation.");
    return inst;
}


std::unique_ptr<Node> Parser::parseNode(ElementNode* parent) {
    if (peek().type == TokenType::At) {
        return parseTemplateInstantiation(parent);
    }
    if (peek().type == TokenType::Identifier) {
        if (peek().value == "text") {
            return parseText();
        }
        if (peek().value == "style") {
            if (parent && parent->tagName == "head") {
                consume(TokenType::Identifier, ".");
                consume(TokenType::OpenBrace, ".");
                auto styleNode = std::make_unique<StyleBlockNode>();
                std::string css_content;
                int brace_level = 1;
                while(!isAtEnd() && brace_level > 0) {
                    if(peek().type == TokenType::OpenBrace) brace_level++;
                    if(peek().type == TokenType::CloseBrace) brace_level--;
                    if(brace_level > 0) css_content += advance().value + " ";
                    else advance();
                }
                styleNode->css_content = css_content;
                return styleNode;
            }
        }
        return parseElement(parent);
    }
    throw std::runtime_error("Unexpected token while parsing a node: " + peek().value);
}

std::unique_ptr<ElementNode> Parser::parseElement(ElementNode* parent) {
    auto element = std::make_unique<ElementNode>();
    element->tagName = consume(TokenType::Identifier, "Expected element tag name.").value;

    consume(TokenType::OpenBrace, "Expected '{' after element tag name.");

    // This is a local style block
    if(element->tagName == "style") {
         while (!check(TokenType::CloseBrace) && !isAtEnd()) {
            if (peek().type == TokenType::At) {
                element->children.push_back(parseTemplateInstantiation(element.get()));
            } else {
                // For now, just consume other properties in a local style block
                advance();
            }
        }
    } else {
        while (!check(TokenType::CloseBrace) && !isAtEnd()) {
            if (peek().type == TokenType::Identifier && m_tokens[m_current + 1].type == TokenType::Colon) {
                parseAttributes(*element);
            } else {
                element->children.push_back(parseNode(element.get()));
            }
        }
    }

    consume(TokenType::CloseBrace, "Expected '}' after element content.");
    return element;
}

std::unique_ptr<TextNode> Parser::parseText() {
    consume(TokenType::Identifier, "Expected 'text' keyword.");
    consume(TokenType::OpenBrace, "Expected '{' after 'text' keyword.");
    auto textNode = std::make_unique<TextNode>();
    textNode->content = consume(TokenType::StringLiteral, "Expected string literal for text content.").value;
    consume(TokenType::CloseBrace, "Expected '}' after text content.");
    if(match(TokenType::Semicolon)) {}
    return textNode;
}

void Parser::parseAttributes(ElementNode& element) {
    std::string key = consume(TokenType::Identifier, "Expected attribute name.").value;
    consume(TokenType::Colon, "Expected ':' after attribute name.");
    Token valueToken = advance();
    if (valueToken.type != TokenType::Identifier && valueToken.type != TokenType::StringLiteral) {
        throw std::runtime_error("Expected identifier or string literal as attribute value.");
    }
    element.attributes[key] = valueToken.value;
    consume(TokenType::Semicolon, "Expected ';' after attribute value.");
}

Token& Parser::peek() { return m_tokens[m_current]; }
Token& Parser::previous() { return m_tokens[m_current - 1]; }
bool Parser::isAtEnd() { return m_current >= m_tokens.size() || peek().type == TokenType::EndOfFile; }
Token& Parser::advance() { if (!isAtEnd()) m_current++; return previous(); }
bool Parser::check(TokenType type) { if (isAtEnd()) return false; return peek().type == type; }

bool Parser::match(TokenType type) {
    if (check(type)) {
        advance();
        return true;
    }
    return false;
}

Token& Parser::consume(TokenType type, const std::string& message) {
    if (check(type)) return advance();
    throw std::runtime_error(message + " Got " + peek().value + " instead.");
}

} // namespace CHTL
