#include "StyleBlockState.h"

#include "../CHTLParser/Parser.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/StyleValue.h" // Include the new StyleValue struct
#include "../Util/StyleUtil.h" // Include the new StyleUtil
#include "../Util/ASTUtil.h" // For findNodeBySelector
#include <stdexcept>
#include <sstream>

// The main handler for the style block. It dispatches to helpers based on the token.
std::unique_ptr<BaseNode> StyleBlockState::handle(Parser& parser) {
    if (!parser.contextNode) {
        throw std::runtime_error("StyleBlockState requires a context node.");
    }

    parser.expectToken(TokenType::Identifier); // consume 'style'
    parser.expectToken(TokenType::OpenBrace);

    while (parser.currentToken.type != TokenType::CloseBrace && parser.currentToken.type != TokenType::EndOfFile) {
        if (parser.currentToken.type == TokenType::At) {
            parseStyleTemplateUsage(parser);
        } else if (parser.currentToken.type == TokenType::Dot || parser.currentToken.type == TokenType::Hash) {
            parseClassOrIdSelector(parser);
        } else if (parser.currentToken.type == TokenType::Ampersand) {
            parseAmpersandSelector(parser);
        } else if (parser.currentToken.type == TokenType::Identifier) {
            parseInlineProperty(parser);
        } else {
            throw std::runtime_error("Unexpected token in style block: " + parser.currentToken.value);
        }
    }

    parser.expectToken(TokenType::CloseBrace);
    return nullptr;
}

// Parses an inline property like 'color: red;'.
void StyleBlockState::parseInlineProperty(Parser& parser) {
    std::string key = parser.currentToken.value;
    parser.expectToken(TokenType::Identifier);
    parser.expectToken(TokenType::Colon);

    StyleValue value = ::parseStyleExpression(parser);
    // If a conditional evaluates to false with no 'else', it returns an EMPTY value.
    // In this case, we don't add the property at all.
    if (value.type != StyleValue::EMPTY) {
        parser.contextNode->inlineStyles[key] = value;
    }

    if (parser.currentToken.type == TokenType::Semicolon) {
        parser.advanceTokens();
    }
}

// Parses a class or ID selector block, e.g., '.box { ... }'.
void StyleBlockState::parseClassOrIdSelector(Parser& parser) {
    std::string selector;
    std::string selectorName;

    if (parser.currentToken.type == TokenType::Dot) {
        selector += ".";
        parser.advanceTokens();
        if (parser.currentToken.type != TokenType::Identifier) throw std::runtime_error("Expected identifier after '.' for class selector.");
        selectorName = parser.currentToken.value;
        parser.contextNode->attributes["class"] = {StyleValue::STRING, 0.0, "", selectorName};
    } else if (parser.currentToken.type == TokenType::Hash) {
        selector += "#";
        parser.advanceTokens();
        if (parser.currentToken.type != TokenType::Identifier) throw std::runtime_error("Expected identifier after '#' for ID selector.");
        selectorName = parser.currentToken.value;
        parser.contextNode->attributes["id"] = {StyleValue::STRING, 0.0, "", selectorName};
    }
    selector += selectorName;
    parser.advanceTokens();

    std::string cssRules = parseCssRuleBlock(parser);
    parser.globalStyleContent += selector + " {\n" + cssRules + "}\n\n";
}

// Parses a selector starting with '&', e.g., '&:hover { ... }'.
void StyleBlockState::parseAmpersandSelector(Parser& parser) {
    std::string baseSelector;
    if (parser.contextNode->attributes.count("class")) {
        baseSelector = "." + parser.contextNode->attributes.at("class").string_val;
    } else if (parser.contextNode->attributes.count("id")) {
        baseSelector = "#" + parser.contextNode->attributes.at("id").string_val;
    } else {
        throw std::runtime_error("Cannot use '&' selector because the parent element has no class or id defined.");
    }

    parser.advanceTokens(); // consume '&'

    std::string selectorSuffix;
    while(parser.currentToken.type != TokenType::OpenBrace && parser.currentToken.type != TokenType::EndOfFile) {
        selectorSuffix += parser.currentToken.value;
        parser.advanceTokens();
    }

    std::string finalSelector = baseSelector + selectorSuffix;
    std::string cssRules = parseCssRuleBlock(parser);
    parser.globalStyleContent += finalSelector + " {\n" + cssRules + "}\n\n";
}

// Helper to parse the body of a CSS rule block '{...}'.
std::string StyleBlockState::parseCssRuleBlock(Parser& parser) {
    parser.expectToken(TokenType::OpenBrace);
    std::string cssRules;
    while(parser.currentToken.type != TokenType::CloseBrace) {
        if (parser.currentToken.type != TokenType::Identifier) throw std::runtime_error("Expected property name inside selector block.");
        std::string key = parser.currentToken.value;
        parser.advanceTokens();
        parser.expectToken(TokenType::Colon);
        StyleValue sv = ::parseStyleExpression(parser);
        // Use the new utility function
        std::string value = styleValueToString(sv);
        cssRules += "  " + key + ": " + value + ";\n";
        if(parser.currentToken.type == TokenType::Semicolon) parser.advanceTokens();
    }
    parser.expectToken(TokenType::CloseBrace);
    return cssRules;
}


void StyleBlockState::applyStyleTemplateRecursive(
    Parser& parser,
    const std::string& ns,
    const std::string& templateName,
    std::map<std::string, std::string>& finalStyles,
    const std::vector<std::string>& deletedTemplates,
    std::vector<std::string>& visitedTemplates)
{
    // Check for circular dependencies.
    for(const auto& visited : visitedTemplates) {
        if (visited == templateName) {
            throw std::runtime_error("Circular dependency detected in style template inheritance involving '" + templateName + "'.");
        }
    }
    visitedTemplates.push_back(templateName);

    // Check if this template was deleted in the specialization block.
    for(const auto& deleted : deletedTemplates) {
        if (deleted == templateName) {
            return; // Do not apply this template or its parents.
        }
    }

    StyleTemplateNode* tmpl = parser.templateManager.getStyleTemplate(ns, templateName);
    if (!tmpl) {
        throw std::runtime_error("Style template '" + templateName + "' not found in namespace '" + ns + "'.");
    }

    // Recursively apply parent styles first.
    for (const auto& parentName : tmpl->parentNames) {
        applyStyleTemplateRecursive(parser, ns, parentName, finalStyles, deletedTemplates, visitedTemplates);
    }

    // Apply this template's own styles, overwriting any parent styles.
    for (const auto& pair : tmpl->styles) {
        finalStyles[pair.first] = pair.second;
    }

    visitedTemplates.pop_back(); // Backtrack
}


void StyleBlockState::parseStyleTemplateUsage(Parser& parser) {
    parser.expectToken(TokenType::At);
    if (parser.currentToken.value != "Style") throw std::runtime_error("Expected 'Style' after '@' for template usage.");
    parser.expectToken(TokenType::Identifier); // consume "Style"

    std::string templateName = parser.currentToken.value;
    parser.expectToken(TokenType::Identifier);

    std::string ns = parser.getCurrentNamespace();
    if (parser.currentToken.type == TokenType::From) {
        parser.advanceTokens(); // consume 'from'

        std::stringstream ns_builder;
        ns_builder << parser.currentToken.value;
        parser.expectToken(TokenType::Identifier);

        while(parser.currentToken.type == TokenType::Dot) {
            parser.advanceTokens(); // consume '.'
            ns_builder << "." << parser.currentToken.value;
            parser.expectToken(TokenType::Identifier);
        }
        ns = ns_builder.str();
    }

    StyleTemplateNode* tmpl = parser.templateManager.getStyleTemplate(ns, templateName);
    if (!tmpl) throw std::runtime_error("Style template not found: " + templateName);

    std::map<std::string, std::string> finalStyles;
    std::vector<std::string> deletedTemplates;
    std::map<std::string, StyleValue> specializedStyles;
    std::vector<std::string> completedProperties;


    // If the template is custom and a specialization block is provided, parse it first.
    if (tmpl->isCustom && parser.currentToken.type == TokenType::OpenBrace) {
        parser.expectToken(TokenType::OpenBrace);
        while (parser.currentToken.type != TokenType::CloseBrace) {
            if (parser.currentToken.type == TokenType::Delete) {
                parser.advanceTokens(); // consume 'delete'

                while(parser.currentToken.type != TokenType::Semicolon && parser.currentToken.type != TokenType::CloseBrace) {
                    if (parser.currentToken.type == TokenType::At) {
                        parser.advanceTokens(); // consume '@'
                        if (parser.currentToken.value != "Style") throw std::runtime_error("Can only delete @Style templates.");
                        parser.advanceTokens(); // consume 'Style'
                        deletedTemplates.push_back(parser.currentToken.value);
                        parser.advanceTokens();
                    } else {
                        // This is a property deletion, store it to apply later.
                        specializedStyles[parser.currentToken.value] = { StyleValue::DELETED };
                        parser.advanceTokens();
                    }

                    if (parser.currentToken.type == TokenType::Comma) {
                        parser.advanceTokens(); // Consume comma and continue loop
                    } else {
                        break; // No comma, so the list of items to delete is done.
                    }
                }
                parser.expectToken(TokenType::Semicolon);

            } else { // Handle property completion/override
                std::string key = parser.currentToken.value;
                parser.expectToken(TokenType::Identifier);
                parser.expectToken(TokenType::Colon);
                specializedStyles[key] = ::parseStyleExpression(parser);
                completedProperties.push_back(key);
                if(parser.currentToken.type == TokenType::Semicolon) parser.advanceTokens();
            }
        }
        parser.expectToken(TokenType::CloseBrace);
    } else {
        if (tmpl->isCustom && !tmpl->valuelessProperties.empty()) {
            throw std::runtime_error("Custom style template with valueless properties requires a specialization block '{...}'.");
        }
        parser.expectToken(TokenType::Semicolon);
    }

    // --- Apply styles ---
    // 1. Recursively apply all parent and template styles.
    std::vector<std::string> visited;
    applyStyleTemplateRecursive(parser, ns, templateName, finalStyles, deletedTemplates, visited);

    // 2. Apply specialized styles from the { ... } block.
    for (const auto& pair : specializedStyles) {
        if (pair.second.type == StyleValue::DELETED) {
            finalStyles.erase(pair.first);
        } else {
            finalStyles[pair.first] = styleValueToString(pair.second);
        }
    }

    // 3. Check if all valueless properties were completed
    for (const auto& valueless : tmpl->valuelessProperties) {
        bool found = false;
        for (const auto& completed : completedProperties) {
            if (valueless == completed) {
                found = true;
                break;
            }
        }
        if (!found) {
            throw std::runtime_error("Valueless property '" + valueless + "' was not assigned a value in custom template usage.");
        }
    }

    // 4. Apply the final, specialized styles to the element's inlineStyles map.
    for (const auto& pair : finalStyles) {
        StyleValue sv;
        sv.type = StyleValue::STRING; // For now, treat all template results as strings
        sv.string_val = pair.second;
        parser.contextNode->inlineStyles[pair.first] = sv;
    }
}
