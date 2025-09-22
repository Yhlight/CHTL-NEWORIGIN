#include "AstTransformer.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/StyleTemplateUsageNode.h"
#include "../CHTLNode/ElementTemplateUsageNode.h"
#include "../CHTLNode/DeletePropertyNode.h"
#include "../CHTLNode/StylePropertyNode.h"
#include "../CHTLParser/Parser.h"
#include <iostream>
#include <vector>
#include <map>
#include <regex>

std::unique_ptr<RootNode> AstTransformer::transform(std::unique_ptr<RootNode> root) {
    collectTemplates(root.get());
    expandTemplates(root.get());
    return root;
}

void AstTransformer::collectTemplates(Node* node) {
    if (!node) return;

    if (auto defNode = dynamic_cast<TemplateDefinitionNode*>(node)) {
        if (defNode->getTemplateType() == TemplateType::Style) {
            styleTemplates[defNode->getName()] = defNode;
        } else if (defNode->getTemplateType() == TemplateType::Element) {
            elementTemplates[defNode->getName()] = defNode;
        } else if (defNode->getTemplateType() == TemplateType::Var) {
            const RawBodyNode* body = defNode->getBody();
            if (body) {
                const auto& tokens = body->getTokens();
                for(size_t i = 0; i < tokens.size(); ++i) {
                    if (tokens[i].type == TokenType::Identifier && i + 1 < tokens.size() && tokens[i+1].type == TokenType::Colon) {
                        std::string varName = tokens[i].value;
                        i += 2; // skip name and colon
                        if (i < tokens.size() && tokens[i].type == TokenType::String) {
                            varTemplates[defNode->getName()][varName] = tokens[i].value;
                        }
                        while(i < tokens.size() && tokens[i].type != TokenType::Semicolon) i++;
                    }
                }
            }
        }
    }

    if (auto rootNode = dynamic_cast<RootNode*>(node)) {
        for (const auto& child : rootNode->getChildren()) collectTemplates(child.get());
    } else if (auto elementNode = dynamic_cast<ElementNode*>(node)) {
        for (const auto& child : elementNode->getChildren()) collectTemplates(child.get());
    }
}

void AstTransformer::expandTemplates(Node* node) {
    if (!node) return;

    if (auto elementNode = dynamic_cast<ElementNode*>(node)) {
        expandTemplatesForChildren(elementNode);
    } else if (auto rootNode = dynamic_cast<RootNode*>(node)) {
        expandTemplatesForChildren(rootNode);
    }

    if (auto elementNode = dynamic_cast<ElementNode*>(node)) {
        expandStyleTemplates(elementNode);
    }
}

void AstTransformer::expandTemplatesForChildren(Node* containerNode) {
    ElementNode* elementNode = dynamic_cast<ElementNode*>(containerNode);
    RootNode* rootNode = dynamic_cast<RootNode*>(containerNode);

    NodeList oldChildren = elementNode ? elementNode->takeChildren() : rootNode->takeChildren();
    NodeList newChildren;

    for (auto& child : oldChildren) {
        expandTemplates(child.get());

        if (auto usageNode = dynamic_cast<ElementTemplateUsageNode*>(child.get())) {
            auto it = elementTemplates.find(usageNode->getName());
            if (it != elementTemplates.end()) {
                const RawBodyNode* body = it->second->getBody();
                if (body) {
                    Parser elementParser(body->getTokens());
                    auto templateRoot = elementParser.parse();
                    NodeList templateChildren = templateRoot->takeChildren();
                    for (auto& templateChild : templateChildren) {
                        newChildren.push_back(std::move(templateChild));
                    }
                }
            } else {
                std::cerr << "Error: Undefined element template '" << usageNode->getName() << "'." << std::endl;
            }
        } else {
            newChildren.push_back(std::move(child));
        }
    }

    if (elementNode) {
        elementNode->setChildren(std::move(newChildren));
    } else {
        rootNode->setChildren(std::move(newChildren));
    }
}

void AstTransformer::expandStyleTemplates(ElementNode* elementNode) {
    StyleStatementList& statements = const_cast<StyleStatementList&>(elementNode->getStyleStatements());
    StyleStatementList expandedStatements;

    // First pass: expand style templates
    for (auto& stmt : statements) {
        if (auto usageNode = dynamic_cast<StyleTemplateUsageNode*>(stmt.get())) {
            auto it = styleTemplates.find(usageNode->getName());
            if (it != styleTemplates.end()) {
                TemplateDefinitionNode* defNode = it->second;
                StyleStatementList templateProps = Parser::parseStyleSnippet(defNode->getBody()->getTokens());

                if(defNode->getIsCustom()) {
                    const StyleStatementList& usageBody = usageNode->getBody();
                    std::map<std::string, std::string> finalValues;
                    for(auto& prop : templateProps) {
                        auto* p = dynamic_cast<StylePropertyNode*>(prop.get());
                        finalValues[p->getProperty()] = p->getValue();
                    }
                    for(const auto& usagePropStmt : usageBody) {
                        if (auto* deleteNode = dynamic_cast<DeletePropertyNode*>(usagePropStmt.get())) {
                            for (const auto& propToDelete : deleteNode->getProperties()) {
                                finalValues.erase(propToDelete);
                            }
                        }
                    }
                    for(const auto& usagePropStmt : usageBody) {
                         if (auto* usp = dynamic_cast<StylePropertyNode*>(usagePropStmt.get())) {
                            if (finalValues.count(usp->getProperty())) {
                                finalValues[usp->getProperty()] = usp->getValue();
                            }
                        }
                    }
                    for(auto const& [prop, val] : finalValues) {
                        expandedStatements.push_back(std::make_unique<StylePropertyNode>(prop, val));
                    }
                } else {
                    for (auto& prop : templateProps) {
                        expandedStatements.push_back(std::move(prop));
                    }
                }
            } else {
                std::cerr << "Error: Undefined style template '" << usageNode->getName() << "'." << std::endl;
            }
        } else {
            expandedStatements.push_back(std::move(stmt));
        }
    }

    // Second pass: expand variables in all resulting properties
    for (auto& stmt : expandedStatements) {
        if (auto* propNode = dynamic_cast<StylePropertyNode*>(stmt.get())) {
            std::string value = propNode->getValue();
            std::regex varRegex("([a-zA-Z0-9_]+)\\(([a-zA-Z0-9_]+)\\)");
            std::smatch match;

            // Keep replacing until no more matches are found
            while (std::regex_search(value, match, varRegex)) {
                std::string groupName = match[1].str();
                std::string varName = match[2].str();

                if (varTemplates.count(groupName) && varTemplates[groupName].count(varName)) {
                    value = std::regex_replace(value, varRegex, varTemplates[groupName][varName], std::regex_constants::format_first_only);
                } else {
                    break;
                }
            }
            propNode->setValue(value);
        }
    }

    statements = std::move(expandedStatements);
}
