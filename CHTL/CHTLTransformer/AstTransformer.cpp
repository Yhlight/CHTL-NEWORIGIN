#include "AstTransformer.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TemplateUsageNode.h"
#include "../CHTLNode/StylePropertyNode.h"
#include "../CHTLParser/Parser.h" // Needed to re-parse template bodies
#include <iostream>

std::unique_ptr<RootNode> AstTransformer::transform(std::unique_ptr<RootNode> root) {
    // Pass 1: Collect all template definitions
    collectTemplates(root.get());

    // Pass 2: Expand all template usages
    expandTemplates(root.get());

    return root;
}

void AstTransformer::collectTemplates(Node* node) {
    if (!node) return;

    // Is this a template definition node?
    if (auto defNode = dynamic_cast<TemplateDefinitionNode*>(node)) {
        if (defNode->getTemplateType() == TemplateType::Style) {
            if (styleTemplates.count(defNode->getName())) {
                std::cerr << "Warning: Duplicate style template definition for '" << defNode->getName() << "'. Using the last one." << std::endl;
            }
            styleTemplates[defNode->getName()] = defNode;
        }
        // Handle other template types here in the future
    }

    // Recursively traverse the tree
    if (auto rootNode = dynamic_cast<RootNode*>(node)) {
        for (const auto& child : rootNode->getChildren()) {
            collectTemplates(child.get());
        }
    } else if (auto elementNode = dynamic_cast<ElementNode*>(node)) {
        for (const auto& child : elementNode->getChildren()) {
            collectTemplates(child.get());
        }
    }
}

void AstTransformer::expandTemplates(Node* node) {
    if (!node) return;

    // Recursively traverse the tree first (post-order traversal)
    if (auto rootNode = dynamic_cast<RootNode*>(node)) {
        for (const auto& child : rootNode->getChildren()) {
            expandTemplates(child.get());
        }
    } else if (auto elementNode = dynamic_cast<ElementNode*>(node)) {
        for (const auto& child : elementNode->getChildren()) {
            expandTemplates(child.get());
        }

        // Now process the style statements for the current element
        StyleStatementList& statements = const_cast<StyleStatementList&>(elementNode->getStyleStatements());
        StyleStatementList expandedStatements;

        for (auto& stmt : statements) {
            // Is this a style template usage?
            if (auto usageNode = dynamic_cast<TemplateUsageNode*>(stmt.get())) {
                if (usageNode->getTemplateType() == TemplateType::Style) {
                    auto it = styleTemplates.find(usageNode->getName());
                    if (it == styleTemplates.end()) {
                        std::cerr << "Error: Undefined style template '" << usageNode->getName() << "'." << std::endl;
                        continue;
                    }

                    // The body of the template definition contains raw tokens.
                    // We need to parse these tokens as if they were in a style block.
                    const RawBodyNode* body = it->second->getBody();
                    if (body) {
                        // This is a simplified re-parsing. We assume the body only contains
                        // style property declarations.
                        const auto& tokens = body->getTokens();
                        // This is a bit of a hack. A better way would be a parser that can start
                        // from a specific rule. For now, we create a dummy ElementNode and
                        // use the existing parseStyleBlock logic.
                        ElementNode dummy("dummy");
                        Parser styleParser(tokens);
                        // This parseStyleBlock needs to be public or we need a friend class.
                        // For now, let's assume we can call a helper that does this.
                        // This reveals a need to refactor the Parser to support parsing snippets.

                        // Let's manually parse the tokens for now as a simplification.
                        for(size_t i = 0; i < tokens.size(); ++i) {
                            if (tokens[i].type == TokenType::Identifier && i + 1 < tokens.size() && tokens[i+1].type == TokenType::Colon) {
                                std::string propName = tokens[i].value;
                                i += 2; // skip name and colon
                                std::string propValue;
                                while(i < tokens.size() && tokens[i].type != TokenType::Semicolon) {
                                    propValue += tokens[i].value;
                                    i++;
                                }
                                expandedStatements.push_back(std::make_unique<StylePropertyNode>(propName, propValue));
                            }
                        }
                    }
                }
            } else {
                // Not a usage node, so just move it to the expanded list
                expandedStatements.push_back(std::move(stmt));
            }
        }
        // Replace the old statements with the expanded ones
        statements = std::move(expandedStatements);
    }
}
