#include "CHTLParser.h"
#include <stdexcept>

CHTLParser::CHTLParser(const std::string& input, CHTLContext& context, bool discoveryMode)
    : lexer(input), context(context), discoveryMode(discoveryMode) {
    advance();
    advance();
}

void CHTLParser::advance() {
    currentToken = nextToken;
    nextToken = lexer.getNextToken();
}

Token CHTLParser::peek() {
    return nextToken;
}

void CHTLParser::expect(TokenType type) {
    if (currentToken.type == type) {
        advance();
    } else {
        throw std::runtime_error("Unexpected token");
    }
}

#include "../CHTLNode/TextNode.h"
#include "../CHTLNode/CommentNode.h"
#include "../CHTLNode/StyleNode.h"
#include "../CHTLNode/TemplateStyleNode.h"
#include "../CHTLNode/TemplateElementNode.h"
#include "../CHTLNode/TemplateVarNode.h"
#include "../CHTLNode/CustomStyleNode.h"
#include "../CHTLNode/CustomElementNode.h"
#include "../CHTLNode/CustomVarNode.h"
#include "../CHTLNode/ImportNode.h"
#include "../CHTLNode/NamespaceNode.h"
#include "../CHTLNode/DeclarationNode.h"
#include "../CHTLLoader/CHTLLoader.h"

std::unique_ptr<BaseNode> CHTLParser::parseStatement() {
    std::unique_ptr<BaseNode> node = nullptr;

    if (currentToken.type == TokenType::LBracket) {
        if (peek().type == TokenType::Identifier) {
            const auto& keyword = peek().value;
            if (keyword == "Template") node = parseTemplateDeclaration();
            else if (keyword == "Custom") node = parseCustomDeclaration();
            else if (keyword == "Import") node = parseImportDeclaration();
            else if (keyword == "Namespace") node = parseNamespaceDeclaration();
            else throw std::runtime_error("Unsupported declaration type inside [].");
        }
    } else if (currentToken.type == TokenType::Identifier) {
        if (currentToken.value == "text" && peek().type == TokenType::LBrace) node = parseTextNode();
        else if (currentToken.value == "style" && peek().type == TokenType::LBrace) node = parseStyleNode();
        else node = parseElement();
    } else if (currentToken.type == TokenType::Use) {
        node = parseUseStatement();
    } else if (currentToken.type == TokenType::Comment) {
        node = parseCommentNode();
    } else if (currentToken.type == TokenType::If) {
        node = parseIfStatement();
    }

    if (node) {
        const auto& global_constraints = context.getActiveGlobalConstraints();
        if (!global_constraints.empty()) {
            std::string nodeType = node->getNodeType();
            for (const auto& constraint : global_constraints) {
                if (nodeType.rfind(constraint, 0) == 0) {
                    throw std::runtime_error("Node type '" + nodeType + "' is not allowed due to a global 'except' constraint for '" + constraint + "'.");
                }
            }
        }
    }

    return node;
}

std::unique_ptr<BaseNode> CHTLParser::parseTemplateDeclaration() {
    expect(TokenType::LBracket);
    expect(TokenType::Identifier); // "Template"
    expect(TokenType::RBracket);
    expect(TokenType::At);
    std::string templateType = currentToken.value; advance();
    std::string templateName = currentToken.value; advance();

    if (templateType == "Style") {
        auto tmplNode = std::make_unique<TemplateStyleNode>(templateName);
        expect(TokenType::LBrace);
        while (currentToken.type != TokenType::RBrace) {
            std::string propName = currentToken.value; advance();
            expect(TokenType::Colon);
            std::string propValue = currentToken.value; advance();
            expect(TokenType::Semicolon);
            tmplNode->addProperty(propName, propValue);
        }
        expect(TokenType::RBrace);
        context.registerStyleTemplate(std::move(tmplNode));
    } else if (templateType == "Element") {
        auto tmplNode = std::make_unique<TemplateElementNode>(templateName);
        expect(TokenType::LBrace);
        while (currentToken.type != TokenType::RBrace) {
            auto child = parseStatement();
            if (child) tmplNode->addChild(std::move(child));
            else break;
        }
        expect(TokenType::RBrace);
        context.registerElementTemplate(std::move(tmplNode));
    } else if (templateType == "Var") {
        auto tmplNode = std::make_unique<TemplateVarNode>(templateName);
        expect(TokenType::LBrace);
        while (currentToken.type != TokenType::RBrace) {
            std::string varName = currentToken.value; advance();
            expect(TokenType::Colon);
            std::string varValue = currentToken.value; advance();
            expect(TokenType::Semicolon);
            tmplNode->addVariable(varName, varValue);
        }
        expect(TokenType::RBrace);
        context.registerVarTemplate(std::move(tmplNode));
    }
    return std::make_unique<DeclarationNode>("[Template] @" + templateType);
}

std::unique_ptr<BaseNode> CHTLParser::parseCustomDeclaration() {
    expect(TokenType::LBracket);
    expect(TokenType::Identifier); // "Custom"
    expect(TokenType::RBracket);
    expect(TokenType::At);
    std::string customType = currentToken.value; advance();
    std::string customName = currentToken.value; advance();

    if (customType == "Style") {
        auto customNode = std::make_unique<CustomStyleNode>(customName);
        expect(TokenType::LBrace);
        while (currentToken.type != TokenType::RBrace) {
            if (currentToken.value == "delete") {
                advance();
                do {
                    customNode->addDeletedProperty(currentToken.value); advance();
                    if (currentToken.type == TokenType::Comma) advance();
                    else break;
                } while (true);
                expect(TokenType::Semicolon);
            } else {
                std::string propName = currentToken.value; advance();
                expect(TokenType::Colon);
                std::string propValue = currentToken.value; advance();
                expect(TokenType::Semicolon);
                customNode->addProperty(propName, propValue);
            }
        }
        expect(TokenType::RBrace);
        context.registerCustomStyle(std::move(customNode));
    } else if (customType == "Element") {
        auto customNode = std::make_unique<CustomElementNode>(customName);
        expect(TokenType::LBrace);
        while (currentToken.type != TokenType::RBrace) {
            if (currentToken.value == "delete") {
                advance();
                do {
                    customNode->addDeletedElement(currentToken.value); advance();
                    if (currentToken.type == TokenType::Comma) advance();
                    else break;
                } while (true);
                expect(TokenType::Semicolon);
            } else {
                auto child = parseStatement();
                if (child) customNode->addChild(std::move(child));
                else break;
            }
        }
        expect(TokenType::RBrace);
        context.registerCustomElement(std::move(customNode));
    } else if (customType == "Var") {
        auto customNode = std::make_unique<CustomVarNode>(customName);
        expect(TokenType::LBrace);
        while (currentToken.type != TokenType::RBrace) {
            std::string varName = currentToken.value; advance();
            expect(TokenType::Colon);
            std::string varValue = currentToken.value; advance();
            expect(TokenType::Semicolon);
            customNode->addVariable(varName, varValue);
        }
        expect(TokenType::RBrace);
        context.registerCustomVar(std::move(customNode));
    }
    return std::make_unique<DeclarationNode>("[Custom] @" + customType);
}

std::unique_ptr<BaseNode> CHTLParser::parseStyleNode() {
    expect(TokenType::Identifier); // "style"
    expect(TokenType::LBrace);
    auto styleNode = std::make_unique<StyleNode>();

    while (currentToken.type != TokenType::RBrace) {
        if (currentToken.type == TokenType::At) {
            advance(); // Consume '@'
            expect(TokenType::Identifier); // "Style"
            std::string templateName = currentToken.value;
            advance();
            if (!discoveryMode) {
                const TemplateStyleNode* tmpl = nullptr;
                if (currentToken.type == TokenType::From) {
                    advance();
                    std::string nsPath;
                    do {
                        nsPath += currentToken.value; advance();
                        if (currentToken.type == TokenType::Dot) {
                            nsPath += "."; advance();
                        } else break;
                    } while (true);
                    tmpl = context.getStyleTemplate(templateName, nsPath);
                } else {
                    tmpl = context.getStyleTemplate(templateName);
                }
                if (!tmpl) throw std::runtime_error("Style template not found: " + templateName);
                for (const auto& prop : tmpl->getProperties()) {
                    styleNode->addProperty(prop.first, prop.second);
                }
            } else {
                if (currentToken.type == TokenType::From) {
                    advance();
                    while (currentToken.type == TokenType::Identifier || currentToken.type == TokenType::Dot) {
                        advance();
                    }
                }
            }
            expect(TokenType::Semicolon);
        } else if (peek().type == TokenType::LBrace) {
            // This is a nested CSS rule, e.g., ".class { ... }"
            std::string selector = currentToken.value;
            advance(); // Consume selector
            auto ruleNode = std::make_unique<CssRuleNode>(selector);

            expect(TokenType::LBrace);
            while (currentToken.type != TokenType::RBrace) {
                std::string propName = currentToken.value;
                advance();
                expect(TokenType::Colon);
                std::string propValue = currentToken.value;
                advance();
                expect(TokenType::Semicolon);
                ruleNode->addProperty(propName, propValue);
            }
            expect(TokenType::RBrace);
            styleNode->addRule(std::move(ruleNode));
        } else {
            // This is an inline property
            std::string propName = currentToken.value;
            advance();
            expect(TokenType::Colon);
            auto propValue = parsePropertyValue();
            styleNode->addProperty(propName, std::move(propValue));
            expect(TokenType::Semicolon);
        }
    }
    expect(TokenType::RBrace);
    return styleNode;
}

PropertyValue CHTLParser::parsePropertyValue() {
    std::vector<Token> valueTokens;
    while (currentToken.type != TokenType::Semicolon && currentToken.type != TokenType::EndOfFile) {
        valueTokens.push_back(currentToken);
        advance();
    }

    bool isConditional = false;
    for (const auto& token : valueTokens) {
        if (token.type == TokenType::Question) {
            isConditional = true;
            break;
        }
    }

    if (!isConditional) {
        std::string simpleValue;
        for (size_t i = 0; i < valueTokens.size(); ++i) {
            simpleValue += valueTokens[i].value;
            if (i < valueTokens.size() - 1 && valueTokens[i+1].type != TokenType::Semicolon) {
                simpleValue += " ";
            }
        }
        return simpleValue;
    } else {
        ConditionalPropertyValue conditionalValue;
        size_t currentPos = 0;

        while (currentPos < valueTokens.size()) {
            ConditionalBranch branch;

            auto q_it = std::find_if(valueTokens.begin() + currentPos, valueTokens.end(),
                                     [](const Token& t) { return t.type == TokenType::Question; });

            if (q_it == valueTokens.end()) {
                break;
            }

            branch.condition = {valueTokens.begin() + currentPos, q_it};
            currentPos = std::distance(valueTokens.begin(), q_it) + 1;

            if (currentPos >= valueTokens.size() || valueTokens[currentPos].type != TokenType::String) {
                throw std::runtime_error("Invalid conditional expression: missing string value after '?'");
            }
            branch.trueValue = valueTokens[currentPos].value;
            currentPos++;

            conditionalValue.branches.push_back(branch);

            if (currentPos >= valueTokens.size()) {
                break;
            }

            if (valueTokens[currentPos].type == TokenType::Colon) {
                currentPos++;
                if (currentPos >= valueTokens.size() || valueTokens[currentPos].type != TokenType::String) {
                    throw std::runtime_error("Invalid conditional expression: missing string value after ':'");
                }
                conditionalValue.elseValue = valueTokens[currentPos].value;
                currentPos++;

                if (currentPos < valueTokens.size()) {
                    throw std::runtime_error("Unexpected tokens after final 'else' value in conditional property.");
                }
                break;
            } else if (valueTokens[currentPos].type == TokenType::Comma) {
                currentPos++;
            } else {
                break;
            }
        }
        return conditionalValue;
    }
}

void CHTLParser::parseAttribute(ElementNode& node) {
    std::string attrName = currentToken.value; expect(TokenType::Identifier);
    if (currentToken.type == TokenType::Colon || currentToken.type == TokenType::Assign) advance();
    else throw std::runtime_error("Expected ':' or '=' after attribute name.");
    std::string attrValue = currentToken.value; advance();
    expect(TokenType::Semicolon);
    node.setAttribute(attrName, attrValue);
}

std::unique_ptr<BaseNode> CHTLParser::parseTextNode() {
    expect(TokenType::Identifier);
    expect(TokenType::LBrace);
    auto node = std::make_unique<TextNode>(currentToken.value);
    advance();
    expect(TokenType::RBrace);
    return node;
}

std::unique_ptr<BaseNode> CHTLParser::parseCommentNode() {
    auto node = std::make_unique<CommentNode>(currentToken.value);
    advance();
    return node;
}

std::unique_ptr<BaseNode> CHTLParser::parseTextAttribute() {
    expect(TokenType::Identifier);
    if (currentToken.type == TokenType::Colon || currentToken.type == TokenType::Assign) advance();
    else throw std::runtime_error("Expected ':' or '=' after 'text' attribute.");
    auto node = std::make_unique<TextNode>(currentToken.value);
    advance();
    expect(TokenType::Semicolon);
    return node;
}

std::unique_ptr<ElementNode> CHTLParser::parseElement() {
    if (currentToken.type != TokenType::Identifier) return nullptr;
    auto node = std::make_unique<ElementNode>(currentToken.value);
    advance();
    if (currentToken.type == TokenType::LBrace) {
        advance();
        while (currentToken.type != TokenType::RBrace) {
            if (currentToken.type == TokenType::Except) {
                advance(); // consume 'except'
                do {
                    std::string constraint_value;
                    bool is_type_constraint = false;

                    if (currentToken.type == TokenType::LBracket) {
                        is_type_constraint = true;
                        constraint_value += "["; advance();
                        constraint_value += currentToken.value; advance(); // e.g., "Template"
                        constraint_value += "]"; expect(TokenType::RBracket);

                        if (currentToken.type == TokenType::At) { // for cases like [Template] @Var
                            constraint_value += " "; // Add space for readability
                            constraint_value += "@"; advance();
                            constraint_value += currentToken.value; advance();
                        }
                    } else if (currentToken.type == TokenType::At) {
                        is_type_constraint = true;
                        constraint_value += "@"; advance();
                        constraint_value += currentToken.value; advance(); // e.g., "Html"
                    } else if (currentToken.type == TokenType::Identifier) {
                        constraint_value = currentToken.value;
                        advance();
                    } else {
                        throw std::runtime_error("Invalid token in 'except' clause. Expected identifier, '@', or '['.");
                    }

                    if (is_type_constraint) {
                        node->addTypeConstraint(constraint_value);
                    } else {
                        node->addConstraint(constraint_value);
                    }

                    if (currentToken.type == TokenType::Comma) {
                        advance();
                    } else {
                        break;
                    }
                } while (true);
                expect(TokenType::Semicolon);
                continue;
            }

            if (currentToken.type == TokenType::Identifier && (peek().type == TokenType::Colon || peek().type == TokenType::Assign)) {
                if (currentToken.value == "text") node->addChild(parseTextAttribute());
                else parseAttribute(*node);
            } else {
                auto pre_parse_token = currentToken;
                auto child = parseStatement();
                if (child) {
                    // Check tag name constraints
                    if (auto* childElement = dynamic_cast<ElementNode*>(child.get())) {
                        const auto& constraints = node->getConstraints();
                        const auto& tagName = childElement->getTagName();
                        for (const auto& constraint : constraints) {
                            if (tagName == constraint) {
                                throw std::runtime_error("Element '" + tagName + "' is not allowed in <" + node->getTagName() + "> due to an 'except' constraint.");
                            }
                        }
                    }
                    // Check type constraints
                    const auto& typeConstraints = node->getTypeConstraints();
                    if (!typeConstraints.empty()) {
                        std::string childType = child->getNodeType();
                        for (const auto& typeConstraint : typeConstraints) {
                            if (childType.rfind(typeConstraint, 0) == 0) { // checks if childType starts with typeConstraint
                                throw std::runtime_error("Node type '" + childType + "' is not allowed in <" + node->getTagName() + "> due to an 'except' type constraint for '" + typeConstraint + "'.");
                            }
                        }
                    }

                    // Do not add declarations to the children list
                    if (dynamic_cast<DeclarationNode*>(child.get()) == nullptr) {
                        node->addChild(std::move(child));
                    }
                }
                else if (pre_parse_token == currentToken) break;
            }
        }
        expect(TokenType::RBrace);
    }
    return node;
}

std::unique_ptr<DocumentNode> CHTLParser::parse() {
    auto documentNode = std::make_unique<DocumentNode>();
    while (currentToken.type != TokenType::EndOfFile) {
        auto pre_parse_token = currentToken;
        auto statement = parseStatement();
        if (statement) {
            bool isEmittingNode = true;
            if (auto useNode = dynamic_cast<UseNode*>(statement.get())) {
                if (useNode->getDirective() == "html5") {
                    documentNode->setHasHtml5Doctype(true);
                }
                isEmittingNode = false;
            } else if (dynamic_cast<DeclarationNode*>(statement.get()) != nullptr) {
                isEmittingNode = false;
            }

            if (isEmittingNode) {
                documentNode->addChild(std::move(statement));
            }
        } else if (pre_parse_token == currentToken) {
            break;
        }
    }
    return documentNode;
}

std::unique_ptr<BaseNode> CHTLParser::parseImportDeclaration() {
    expect(TokenType::LBracket);
    expect(TokenType::Identifier); // "Import"
    expect(TokenType::RBracket);
    std::string fullType;
    if (currentToken.type == TokenType::LBracket) {
        fullType += "["; advance();
        fullType += currentToken.value; advance();
        fullType += "]"; expect(TokenType::RBracket);
    }
    expect(TokenType::At);
    fullType += "@" + currentToken.value;
    advance();
    std::string entityName;
    if (currentToken.type == TokenType::Identifier) {
        entityName = currentToken.value;
        advance();
    }
    expect(TokenType::From);
    std::string filePath = currentToken.value;
    advance();
    std::string alias;
    if (currentToken.type == TokenType::As) {
        advance();
        alias = currentToken.value;
        advance();
    }
    return std::make_unique<ImportNode>(fullType, entityName, filePath, alias);
}

std::unique_ptr<BaseNode> CHTLParser::parseUseStatement() {
    expect(TokenType::Use);
    std::string directive = currentToken.value;
    advance();
    expect(TokenType::Semicolon);
    return std::make_unique<UseNode>(directive);
}

std::unique_ptr<BaseNode> CHTLParser::parseNamespaceDeclaration() {
    expect(TokenType::LBracket);
    expect(TokenType::Identifier); // "Namespace"
    expect(TokenType::RBracket);
    std::string namespaceName = currentToken.value;
    advance();
    context.enterNamespace(namespaceName);
    auto namespaceNode = std::make_unique<NamespaceNode>(namespaceName);

    if (currentToken.type == TokenType::LBrace) {
        advance();
        while (currentToken.type != TokenType::RBrace) {
            if (currentToken.type == TokenType::Except) {
                advance(); // consume 'except'
                do {
                    std::string constraint_value;
                    if (currentToken.type == TokenType::LBracket) {
                        constraint_value += "["; advance();
                        constraint_value += currentToken.value; advance(); // e.g., "Template"
                        constraint_value += "]"; expect(TokenType::RBracket);
                    } else {
                        throw std::runtime_error("Global 'except' only supports type constraints like [Template].");
                    }
                    context.addGlobalConstraint(constraint_value);
                    if (currentToken.type == TokenType::Comma) {
                        advance();
                    } else {
                        break;
                    }
                } while (true);
                expect(TokenType::Semicolon);
                continue;
            }

            auto pre_parse_token = currentToken;
            auto child = parseStatement();
            if (child) {
                // Do not add declarations to the children list
                if (dynamic_cast<DeclarationNode*>(child.get()) == nullptr) {
                    namespaceNode->addChild(std::move(child));
                }
            } else if (pre_parse_token == currentToken) {
                break;
            }
        }
        expect(TokenType::RBrace);
    }
    context.leaveNamespace();
    return namespaceNode;
}

std::unique_ptr<IfNode> CHTLParser::parseIfStatement() {
    expect(TokenType::If);
    expect(TokenType::LBrace);

    if (currentToken.type != TokenType::Condition) {
        throw std::runtime_error("Expected 'condition' keyword in if block.");
    }
    advance(); // consume 'condition'
    expect(TokenType::Colon);

    std::vector<Token> conditionTokens;
    bool isDynamic = false;
    while(currentToken.type != TokenType::Comma && currentToken.type != TokenType::RBrace) {
        if (currentToken.type == TokenType::DynamicSelectorStart) {
            isDynamic = true;
        }
        conditionTokens.push_back(currentToken);
        advance();
    }
     if (currentToken.type == TokenType::Comma) {
        advance();
    }

    auto ifNode = std::make_unique<IfNode>(conditionTokens, isDynamic);

    while (currentToken.type != TokenType::RBrace) {
        std::string propName = currentToken.value;
        advance();
        expect(TokenType::Colon);
        auto propValue = parsePropertyValue();
        ifNode->addProperty(propName, std::move(propValue));
        if (currentToken.type == TokenType::Semicolon) {
            advance();
        }
    }
    expect(TokenType::RBrace);

    if (currentToken.type == TokenType::Else) {
        advance();
        if (currentToken.type == TokenType::If) {
            ifNode->setElseBranch(parseIfStatement());
        } else {
            expect(TokenType::LBrace);
            auto elseNode = std::make_unique<IfNode>(std::vector<Token>{}); // Empty condition for else
            while (currentToken.type != TokenType::RBrace) {
                std::string propName = currentToken.value;
                advance();
                expect(TokenType::Colon);
                auto propValue = parsePropertyValue();
                elseNode->addProperty(propName, std::move(propValue));
                 if (currentToken.type == TokenType::Semicolon) {
                    advance();
                }
            }
            expect(TokenType::RBrace);
            ifNode->setElseBranch(std::move(elseNode));
        }
    }

    return ifNode;
}