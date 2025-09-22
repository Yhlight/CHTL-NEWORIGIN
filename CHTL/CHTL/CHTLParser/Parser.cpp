#include "Parser.h"
#include <iostream>

namespace CHTL {

// Helper to construct a qualified name from the namespace stack
std::string getCurrentNamespacePrefix(const std::vector<std::string>& stack) {
    if (stack.empty()) {
        return "";
    }
    std::string result;
    for (const auto& ns : stack) {
        result += ns + ".";
    }
    return result;
}


Parser::Parser(Lexer& lexer) : m_lexer(lexer) {
    nextToken();
    nextToken();
}

void Parser::nextToken() {
    m_currentToken = m_peekToken;
    m_peekToken = m_lexer.nextToken();
}

bool Parser::currentTokenIs(TokenType type) const {
    return m_currentToken.type == type;
}

bool Parser::peekTokenIs(TokenType type) const {
    return m_peekToken.type == type;
}

bool Parser::expectPeek(TokenType type) {
    if (peekTokenIs(type)) {
        nextToken();
        return true;
    }
    m_errors.push_back("Expected next token to be " + TokenTypeToString(type) +
                       ", but got " + TokenTypeToString(m_peekToken.type) + " instead.");
    return false;
}

std::unique_ptr<Program> Parser::parseProgram() {
    auto program = std::make_unique<Program>();
    while (!currentTokenIs(TokenType::END_OF_FILE)) {
        auto stmt = parseStatement();
        if (stmt) {
            program->statements.push_back(std::move(stmt));
        }
        nextToken();
    }
    return program;
}

std::unique_ptr<Statement> Parser::parseStatement() {
    if (currentTokenIs(TokenType::IDENTIFIER)) {
        if (m_currentToken.literal == "text") {
            return parseTextNode();
        }
        return parseElementNode();
    } else if (currentTokenIs(TokenType::LEFT_BRACKET)) {
        if (peekTokenIs(TokenType::IDENTIFIER)) {
            if (m_peekToken.literal == "Template") {
                return parseTemplateDefinition(false);
            } else if (m_peekToken.literal == "Custom") {
                return parseTemplateDefinition(true);
            } else if (m_peekToken.literal == "Import") {
                return parseImportStatement();
            } else if (m_peekToken.literal == "Namespace") {
                return parseNamespaceNode();
            }
        }
    }
    return nullptr;
}

std::unique_ptr<NamespaceNode> Parser::parseNamespaceNode() {
    auto node = std::make_unique<NamespaceNode>();
    // Consume [Namespace]
    nextToken();
    nextToken();

    if (!expectPeek(TokenType::IDENTIFIER)) {
        return nullptr;
    }
    node->name = m_currentToken;
    m_namespaceStack.push_back(node->name.literal);

    if (!expectPeek(TokenType::LEFT_BRACE)) {
        m_namespaceStack.pop_back(); // Clean up if no brace
        return nullptr;
    }
    nextToken();

    while (!currentTokenIs(TokenType::RIGHT_BRACE) && !currentTokenIs(TokenType::END_OF_FILE)) {
        auto stmt = parseStatement();
        if (stmt) {
            node->body.push_back(std::move(stmt));
        }
        nextToken();
    }

    m_namespaceStack.pop_back();
    return node;
}


std::unique_ptr<Statement> Parser::parseTemplateDefinition(bool isCustom) {
    // Expect [Template] or [Custom]
    nextToken();
    nextToken();

    if (!expectPeek(TokenType::AT_SIGN)) {
        return nullptr;
    }
    if (!expectPeek(TokenType::IDENTIFIER)) {
        return nullptr;
    }

    if (m_currentToken.literal == "Style") {
        auto node = parseStyleTemplateDefinition();
        if (node) {
            std::string prefix = getCurrentNamespacePrefix(m_namespaceStack);
            styleTemplates[prefix + node->name.literal] = std::move(node);
        }
        return nullptr;
    } else if (m_currentToken.literal == "Element") {
        auto node = parseElementTemplateDefinition();
        if (node) {
            std::string prefix = getCurrentNamespacePrefix(m_namespaceStack);
            elementTemplates[prefix + node->name.literal] = std::move(node);
        }
        return nullptr;
    }
    return nullptr;
}

std::unique_ptr<ImportNode> Parser::parseImportStatement() {
    nextToken();
    nextToken();
    if (!expectPeek(TokenType::AT_SIGN)) return nullptr;
    if (!expectPeek(TokenType::IDENTIFIER) || m_currentToken.literal != "Chtl") return nullptr;
    if (!expectPeek(TokenType::FROM)) return nullptr;
    if (!expectPeek(TokenType::STRING_LITERAL)) return nullptr;

    std::string path = m_currentToken.literal;
    auto file_content = m_loader.loadFile(path);

    if (!file_content) {
        m_errors.push_back("Could not load file: " + path);
        return nullptr;
    }

    Lexer imported_lexer(*file_content);
    Parser imported_parser(imported_lexer);
    imported_parser.parseProgram();

    if (!imported_parser.errors().empty()) {
        for (const auto& err : imported_parser.errors()) {
            m_errors.push_back("Error in imported file '" + path + "': " + err);
        }
        return nullptr;
    }

    // This is where default namespacing would happen.
    // For now, we just merge globally.
    styleTemplates.insert(std::make_move_iterator(imported_parser.styleTemplates.begin()),
                          std::make_move_iterator(imported_parser.styleTemplates.end()));
    elementTemplates.insert(std::make_move_iterator(imported_parser.elementTemplates.begin()),
                            std::make_move_iterator(imported_parser.elementTemplates.end()));

    return nullptr;
}

std::unique_ptr<ElementTemplateNode> Parser::parseElementTemplateDefinition() {
    auto templateNode = std::make_unique<ElementTemplateNode>();
    if (!expectPeek(TokenType::IDENTIFIER)) return nullptr;
    templateNode->name = m_currentToken;
    if (!expectPeek(TokenType::LEFT_BRACE)) return nullptr;
    nextToken();
    while (!currentTokenIs(TokenType::RIGHT_BRACE) && !currentTokenIs(TokenType::END_OF_FILE)) {
        auto stmt = parseStatement();
        if (stmt) templateNode->body.push_back(std::move(stmt));
        nextToken();
    }
    return templateNode;
}

std::unique_ptr<ElementTemplateUsageNode> Parser::parseElementTemplateUsage() {
    auto usageNode = std::make_unique<ElementTemplateUsageNode>();
    if (!expectPeek(TokenType::IDENTIFIER)) return nullptr;
    usageNode->name = m_currentToken;
    if (peekTokenIs(TokenType::FROM)) {
        nextToken();
        if (!expectPeek(TokenType::IDENTIFIER)) return nullptr;
        usageNode->fromNamespace = m_currentToken;
    }
    if (!expectPeek(TokenType::SEMICOLON)) return nullptr;
    return usageNode;
}

std::unique_ptr<StyleTemplateNode> Parser::parseStyleTemplateDefinition() {
    auto templateNode = std::make_unique<StyleTemplateNode>();
    if (!expectPeek(TokenType::IDENTIFIER)) return nullptr;
    templateNode->name = m_currentToken;
    if (!expectPeek(TokenType::LEFT_BRACE)) return nullptr;
    nextToken();
    while (!currentTokenIs(TokenType::RIGHT_BRACE) && !currentTokenIs(TokenType::END_OF_FILE)) {
        if (currentTokenIs(TokenType::IDENTIFIER)) {
            auto propNode = parseStylePropertyNode();
            if (propNode) templateNode->properties.push_back(std::move(propNode));
        }
        nextToken();
    }
    return templateNode;
}

std::unique_ptr<DeleteStatementNode> Parser::parseDeleteStatement() {
    auto deleteNode = std::make_unique<DeleteStatementNode>();
    if (!expectPeek(TokenType::IDENTIFIER)) return nullptr;
    deleteNode->target = m_currentToken;
    if (peekTokenIs(TokenType::SEMICOLON)) nextToken();
    return deleteNode;
}

std::unique_ptr<StyleTemplateUsageNode> Parser::parseStyleTemplateUsage() {
    auto usageNode = std::make_unique<StyleTemplateUsageNode>();
    if (!expectPeek(TokenType::IDENTIFIER)) return nullptr;
    usageNode->name = m_currentToken;

    if (peekTokenIs(TokenType::FROM)) {
        nextToken();
        if (!expectPeek(TokenType::IDENTIFIER)) return nullptr;
        usageNode->fromNamespace = m_currentToken;
    }

    if (peekTokenIs(TokenType::LEFT_BRACE)) {
        nextToken();
        nextToken();
        while (!currentTokenIs(TokenType::RIGHT_BRACE) && !currentTokenIs(TokenType::END_OF_FILE)) {
            if (currentTokenIs(TokenType::IDENTIFIER)) {
                auto propNode = parseStylePropertyNode();
                if (propNode) usageNode->body.push_back(std::move(propNode));
            } else if (currentTokenIs(TokenType::DELETE)) {
                auto deleteNode = parseDeleteStatement();
                if (deleteNode) usageNode->body.push_back(std::move(deleteNode));
            }
            nextToken();
        }
    } else if (peekTokenIs(TokenType::SEMICOLON)) {
        nextToken();
    }
    return usageNode;
}

std::unique_ptr<ElementNode> Parser::parseElementNode() {
    auto node = std::make_unique<ElementNode>();
    node->token = m_currentToken;
    if (!expectPeek(TokenType::LEFT_BRACE)) return nullptr;
    nextToken();
    while (!currentTokenIs(TokenType::RIGHT_BRACE) && !currentTokenIs(TokenType::END_OF_FILE)) {
        if (currentTokenIs(TokenType::IDENTIFIER)) {
            if (m_currentToken.literal == "style" && peekTokenIs(TokenType::LEFT_BRACE)) {
                node->styleBlock = parseStyleBlockNode();
            } else if (peekTokenIs(TokenType::COLON) || peekTokenIs(TokenType::EQUALS)) {
                if (m_currentToken.literal == "text") {
                    nextToken();
                    nextToken();
                    if (!currentTokenIs(TokenType::STRING_LITERAL)) return nullptr;
                    auto textNode = std::make_unique<TextNode>();
                    textNode->value = m_currentToken;
                    node->children.push_back(std::move(textNode));
                    if (peekTokenIs(TokenType::SEMICOLON)) nextToken();
                } else {
                    auto attr = parseAttributeNode();
                    if (attr) node->attributes.push_back(std::move(attr));
                }
            } else {
                auto stmt = parseStatement();
                if (stmt) node->children.push_back(std::move(stmt));
            }
        } else if (currentTokenIs(TokenType::AT_SIGN)) {
            if (peekTokenIs(TokenType::IDENTIFIER) && m_peekToken.literal == "Element") {
                nextToken();
                auto usageNode = parseElementTemplateUsage();
                if (usageNode) node->children.push_back(std::move(usageNode));
            }
        } else if (currentTokenIs(TokenType::COMMENT)) {
            // Ignore
        }
        nextToken();
    }
    return node;
}

std::unique_ptr<StyleBlockNode> Parser::parseStyleBlockNode() {
    auto styleNode = std::make_unique<StyleBlockNode>();
    if (!expectPeek(TokenType::LEFT_BRACE)) return nullptr;
    nextToken();
    while (!currentTokenIs(TokenType::RIGHT_BRACE) && !currentTokenIs(TokenType::END_OF_FILE)) {
        if (currentTokenIs(TokenType::IDENTIFIER)) {
            auto propNode = parseStylePropertyNode();
            if (propNode) styleNode->body.push_back(std::move(propNode));
        } else if (currentTokenIs(TokenType::DOT) || currentTokenIs(TokenType::HASH)) {
            auto ruleNode = parseSelectorRuleNode();
            if (ruleNode) styleNode->body.push_back(std::move(ruleNode));
        } else if (currentTokenIs(TokenType::AT_SIGN)) {
            if (peekTokenIs(TokenType::IDENTIFIER) && m_peekToken.literal == "Style") {
                nextToken();
                auto usageNode = parseStyleTemplateUsage();
                if (usageNode) styleNode->body.push_back(std::move(usageNode));
            }
        }
        nextToken();
    }
    return styleNode;
}

std::unique_ptr<SelectorRuleNode> Parser::parseSelectorRuleNode() {
    auto ruleNode = std::make_unique<SelectorRuleNode>();
    ruleNode->token = m_currentToken;
    if (!expectPeek(TokenType::IDENTIFIER)) return nullptr;
    ruleNode->name = m_currentToken;
    if (!expectPeek(TokenType::LEFT_BRACE)) return nullptr;
    nextToken();
    while (!currentTokenIs(TokenType::RIGHT_BRACE) && !currentTokenIs(TokenType::END_OF_FILE)) {
        if (currentTokenIs(TokenType::IDENTIFIER)) {
            auto propNode = parseStylePropertyNode();
            if (propNode) ruleNode->properties.push_back(std::move(propNode));
        }
        nextToken();
    }
    return ruleNode;
}

std::unique_ptr<StylePropertyNode> Parser::parseStylePropertyNode() {
    auto propNode = std::make_unique<StylePropertyNode>();
    propNode->property = m_currentToken;
    if (peekTokenIs(TokenType::SEMICOLON)) {
        propNode->value = "";
        nextToken();
        return propNode;
    }
    if (!expectPeek(TokenType::COLON)) return nullptr;
    nextToken();
    std::stringstream value_ss;
    while (!currentTokenIs(TokenType::SEMICOLON) && !currentTokenIs(TokenType::RIGHT_BRACE) && !currentTokenIs(TokenType::END_OF_FILE)) {
        value_ss << m_currentToken.literal;
        TokenType peekType = m_peekToken.type;
        if (peekType == TokenType::IDENTIFIER || peekType == TokenType::STRING_LITERAL || peekType == TokenType::UNQUOTED_LITERAL) {
             value_ss << " ";
        }
        nextToken();
    }
    propNode->value = value_ss.str();
    if (!propNode->value.empty() && propNode->value.back() == ' ') {
        propNode->value.pop_back();
    }
    if (currentTokenIs(TokenType::SEMICOLON)) {
       // consumed by outer loop
    }
    return propNode;
}

std::unique_ptr<AttributeNode> Parser::parseAttributeNode() {
    auto attrNode = std::make_unique<AttributeNode>();
    attrNode->token = m_currentToken;
    nextToken();
    nextToken();
    if (!currentTokenIs(TokenType::STRING_LITERAL)) return nullptr;
    attrNode->value = m_currentToken;
    if (peekTokenIs(TokenType::SEMICOLON)) nextToken();
    return attrNode;
}

std::unique_ptr<TextNode> Parser::parseTextNode() {
    auto node = std::make_unique<TextNode>();
    if (!expectPeek(TokenType::LEFT_BRACE)) return nullptr;
    if (!expectPeek(TokenType::STRING_LITERAL)) return nullptr;
    node->value = m_currentToken;
    if (!expectPeek(TokenType::RIGHT_BRACE)) return nullptr;
    return node;
}

} // namespace CHTL
