#include "Parser.h"
#include "../Node/StyleNode.h"
#include "../Node/TemplateUsageNode.h"

namespace CHTL {

Parser::Parser(Lexer& lexer, TemplateStore& store) : lexer(lexer), store(store) {
    nextToken();
    nextToken();
}

void Parser::nextToken() {
    currentToken = peekToken;
    peekToken = lexer.getNextToken();
}

bool Parser::currentTokenIs(TokenType type) { return currentToken.type == type; }
bool Parser::peekTokenIs(TokenType type) { return peekToken.type == type; }

void Parser::expectPeek(TokenType type) {
    if (peekTokenIs(type)) {
        nextToken();
    } else {
        throw std::runtime_error("Parser Error: Expected next token to be " + Token::typeToString(type) +
                                 ", got " + Token::typeToString(peekToken.type) + " instead at line " + std::to_string(peekToken.line));
    }
}

// Main parsing loop
std::unique_ptr<ElementNode> Parser::parse() {
    auto root = std::make_unique<ElementNode>("root");
    while (!currentTokenIs(TokenType::EndOfFile)) {
        if (currentTokenIs(TokenType::LeftBracket)) {
            parseTemplateDefinition();
        } else {
            auto stmt = parseStatement();
            if (stmt) {
                root->addChild(std::move(stmt));
            } else {
                nextToken();
            }
        }
    }
    return root;
}

std::unique_ptr<BaseNode> Parser::parseStatement() {
    if (currentTokenIs(TokenType::At)) return parseTemplateUsage();
    if (currentTokenIs(TokenType::TextKeyword)) return parseTextNode();
    if (currentTokenIs(TokenType::Identifier)) {
        if (currentToken.text == "style") return parseStyleNode();
        return parseElementNode();
    }
    return nullptr;
}

void Parser::parseTemplateDefinition() {
    nextToken(); // consume '['
    if (!currentTokenIs(TokenType::Identifier) || currentToken.text != "Template") throw std::runtime_error("Parser Error: Expected 'Template' keyword.");
    nextToken(); // consume 'Template'
    if (!currentTokenIs(TokenType::RightBracket)) throw std::runtime_error("Parser Error: Expected ']' after 'Template'.");
    nextToken(); // consume ']'
    if (!currentTokenIs(TokenType::At)) throw std::runtime_error("Parser Error: Expected '@' for template type.");
    nextToken(); // consume '@'
    if (!currentTokenIs(TokenType::Identifier)) throw std::runtime_error("Parser Error: Expected template type (Style, Element, Var).");
    std::string type = currentToken.text;
    nextToken(); // consume type
    if (!currentTokenIs(TokenType::Identifier)) throw std::runtime_error("Parser Error: Expected template name.");
    std::string name = currentToken.text;
    nextToken(); // consume name
    if (!currentTokenIs(TokenType::LeftBrace)) throw std::runtime_error("Parser Error: Expected '{' to define template body.");
    nextToken(); // consume '{'

    if (type == "Style") {
        StyleTemplate st;
        st.name = name;
        while (!currentTokenIs(TokenType::RightBrace)) {
            std::string key = currentToken.text; nextToken();
            if (!currentTokenIs(TokenType::Colon) && !currentTokenIs(TokenType::Equals)) throw std::runtime_error("Parser Error: Expected ':' in style template.");
            nextToken();
            std::string value;
            while(!currentTokenIs(TokenType::Semicolon) && !currentTokenIs(TokenType::RightBrace)) {
                value += currentToken.text;
                if(!peekTokenIs(TokenType::Semicolon)) value += " ";
                nextToken();
            }
            st.properties.push_back({key, value});
            if(currentTokenIs(TokenType::Semicolon)) nextToken();
        }
        store.addStyleTemplate(st);
    } else if (type == "Element") {
        ElementTemplate et;
        et.name = name;
        while (!currentTokenIs(TokenType::RightBrace)) {
            et.nodes.push_back(parseStatement());
        }
        store.addElementTemplate(std::move(et));
    }
    if (!currentTokenIs(TokenType::RightBrace)) throw std::runtime_error("Parser Error: Expected '}' to close template definition.");
    nextToken(); // consume '}'
}

std::unique_ptr<TemplateUsageNode> Parser::parseTemplateUsage() {
    nextToken(); // consume '@'
    std::string type_str = currentToken.text; nextToken();
    TemplateType type;
    if (type_str == "Style") type = TemplateType::Style;
    else if (type_str == "Element") type = TemplateType::Element;
    else throw std::runtime_error("Parser Error: Unknown template usage type '" + type_str + "'.");
    std::string name = currentToken.text; nextToken();
    if (!currentTokenIs(TokenType::Semicolon)) throw std::runtime_error("Parser Error: Expected ';' after template usage.");
    nextToken(); // consume ';'
    return std::make_unique<TemplateUsageNode>(type, name);
}

std::unique_ptr<TextNode> Parser::parseTextNode() {
    nextToken(); // consume 'text'
    if (!currentTokenIs(TokenType::LeftBrace)) throw std::runtime_error("Parser Error: Expected '{' after 'text' keyword.");
    nextToken(); // consume '{'
    if (!currentTokenIs(TokenType::StringLiteral)) throw std::runtime_error("Parser Error: Expected string literal inside 'text' block.");
    auto node = std::make_unique<TextNode>(currentToken.text);
    nextToken(); // consume string literal
    if (!currentTokenIs(TokenType::RightBrace)) throw std::runtime_error("Parser Error: Expected '}' to close 'text' block.");
    nextToken(); // consume '}'
    return node;
}

StyleRule Parser::parseStyleRule() {
    StyleRule rule;
    while(!currentTokenIs(TokenType::LeftBrace)) {
        rule.selector += currentToken.text;
        nextToken();
    }
    if (!currentTokenIs(TokenType::LeftBrace)) throw std::runtime_error("Parser Error: Expected '{' for style rule.");
    nextToken(); // consume '{'
    while (!currentTokenIs(TokenType::RightBrace)) {
        std::string key = currentToken.text; nextToken();
        if (!currentTokenIs(TokenType::Colon)) throw std::runtime_error("Parser Error: Expected ':' in style rule.");
        nextToken();
        std::string value;
        while(!currentTokenIs(TokenType::Semicolon) && !currentTokenIs(TokenType::RightBrace)) {
            value += currentToken.text;
            if(!peekTokenIs(TokenType::Semicolon)) value += " ";
            nextToken();
        }
        rule.properties.push_back({key, value});
        if (currentTokenIs(TokenType::Semicolon)) nextToken();
    }
    if (!currentTokenIs(TokenType::RightBrace)) throw std::runtime_error("Parser Error: Expected '}' to close style rule.");
    nextToken(); // consume '}'
    return rule;
}

std::unique_ptr<StyleNode> Parser::parseStyleNode() {
    nextToken(); // consume 'style'
    if (!currentTokenIs(TokenType::LeftBrace)) throw std::runtime_error("Parser Error: Expected '{' after 'style' keyword.");
    nextToken(); // consume '{'
    auto node = std::make_unique<StyleNode>();
    while (!currentTokenIs(TokenType::RightBrace)) {
        if (currentTokenIs(TokenType::Dot) || currentTokenIs(TokenType::Hash) || currentTokenIs(TokenType::Ampersand)) {
            node->addRule(parseStyleRule());
        } else if (currentTokenIs(TokenType::At)) {
            auto usage_node = parseTemplateUsage();
            if (usage_node->template_type == TemplateType::Style) {
                node->children.push_back(std::move(usage_node));
            } else {
                throw std::runtime_error("Parser Error: Only @Style templates can be used inside a style block.");
            }
        } else if (currentTokenIs(TokenType::Identifier)) {
            std::string key = currentToken.text; nextToken();
            if (!currentTokenIs(TokenType::Colon)) throw std::runtime_error("Parser Error: Expected ':' for inline style property.");
            nextToken();
            std::string value;
            while(!currentTokenIs(TokenType::Semicolon) && !currentTokenIs(TokenType::RightBrace)) {
                value += currentToken.text;
                if(!peekTokenIs(TokenType::Semicolon)) value += " ";
                nextToken();
            }
            node->addInlineProperty(key, value);
            if (currentTokenIs(TokenType::Semicolon)) nextToken();
        } else {
             nextToken();
        }
    }
    if (!currentTokenIs(TokenType::RightBrace)) throw std::runtime_error("Parser Error: Expected '}' to close 'style' block.");
    nextToken(); // consume '}'
    return node;
}

std::unique_ptr<ElementNode> Parser::parseElementNode() {
    auto node = std::make_unique<ElementNode>(currentToken.text);
    nextToken();
    if (!currentTokenIs(TokenType::LeftBrace)) {
        return node;
    }
    nextToken();
    while (!currentTokenIs(TokenType::RightBrace)) {
        if (currentTokenIs(TokenType::Identifier) && peekTokenIs(TokenType::Colon)) {
            std::string key = currentToken.text; nextToken(); nextToken();
            if (!currentTokenIs(TokenType::Identifier) && !currentTokenIs(TokenType::StringLiteral)) throw std::runtime_error("Parser Error: Expected attribute value.");
            node->addAttribute(key, currentToken.text);
            nextToken();
            if (currentTokenIs(TokenType::Semicolon)) nextToken();
        } else {
            auto stmt = parseStatement();
            if (stmt) {
                node->addChild(std::move(stmt));
            } else {
                nextToken();
            }
        }
    }
    if (!currentTokenIs(TokenType::RightBrace)) throw std::runtime_error("Parser Error: Expected '}' to close element block.");
    nextToken();
    return node;
}

} // namespace CHTL
