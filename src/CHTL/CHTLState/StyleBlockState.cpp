#include "StyleBlockState.h"
#include "../CHTLParser/Parser.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/DynamicStyleNode.h"
#include "../CHTLNode/StaticStyleNode.h"
#include "../CHTLNode/ResponsiveValueNode.h"
#include "../ExpressionParser/ExpressionLexer.h"
#include "../ExpressionParser/ExpressionParser.h"
#include "../Util/StringUtil.h"
#include <stdexcept>
#include <sstream>
#include <algorithm>

namespace {
    bool isStyleValueTerminator(TokenType type) {
        switch (type) {
            case TokenType::Semicolon:
            case TokenType::CloseBrace:
            case TokenType::EndOfFile:
                return true;
            default:
                return false;
        }
    }
}

std::unique_ptr<BaseNode> StyleBlockState::handle(Parser& parser) {
    if (!parser.contextNode) {
        throw std::runtime_error("StyleBlockState requires a context node.");
    }
    parser.expectToken(TokenType::Identifier); // "style"
    parser.expectToken(TokenType::OpenBrace);
    while (parser.currentToken.type != TokenType::CloseBrace && parser.currentToken.type != TokenType::EndOfFile) {
        if (parser.currentToken.type == TokenType::Identifier) {
            parseInlineProperty(parser);
        } else if (parser.currentToken.type == TokenType::At) {
            parseStyleTemplateUsage(parser);
        } else if (parser.currentToken.type == TokenType::Dot || parser.currentToken.type == TokenType::Hash) {
            parseClassOrIdSelector(parser);
        } else if (parser.currentToken.type == TokenType::Ampersand) {
            parseAmpersandSelector(parser);
        }
        else {
            throw std::runtime_error("Unexpected token in style block: " + parser.currentToken.value);
        }
    }
    parser.expectToken(TokenType::CloseBrace);
    return nullptr;
}

void StyleBlockState::parseInlineProperty(Parser& parser) {
    std::string key = parser.currentToken.value;
    parser.expectToken(TokenType::Identifier);
    parser.expectToken(TokenType::Colon);

    auto valueNode = parseStyleExpression(parser);

    // If the value is responsive, register the binding.
    if (auto* responsive_node = dynamic_cast<ResponsiveValueNode*>(valueNode.get())) {
        if (parser.contextNode->attributes.find("id") == parser.contextNode->attributes.end()) {
            parser.contextNode->attributes["id"] = std::make_unique<StaticStyleNode>("chtl-id-" + std::to_string(parser.elementIdCounter++));
        }
        std::string elementId = parser.contextNode->attributes.at("id")->toString();

        ResponsiveBinding binding;
        binding.elementId = elementId;
        binding.property = "style." + key;
        binding.unit = responsive_node->getUnit();
        parser.sharedContext.responsiveBindings[responsive_node->getVariableName()].push_back(binding);
    }

    parser.contextNode->inlineStyles[key] = std::move(valueNode);

    if (parser.currentToken.type == TokenType::Semicolon) {
        parser.advanceTokens();
    }
}

std::unique_ptr<StyleValue> StyleBlockState::parseStyleExpression(Parser& parser) {
    // 1. Collect the raw string for the expression.
    // We read tokens from the main CHTL lexer until we hit a terminator.
    std::stringstream ss;
    size_t start_pos = parser.currentToken.start_pos;
    size_t end_pos = start_pos;

    while (!isStyleValueTerminator(parser.currentToken.type)) {
        end_pos = parser.currentToken.start_pos + parser.currentToken.value.length();
        parser.advanceTokens();
    }
    std::string expression_string = parser.lexer.getSource().substr(start_pos, end_pos - start_pos);
    trim(expression_string);

    // 2. Check for the simple case: a static value with no operators.
    // This is a temporary optimization. A more robust solution might parse all values.
    if (expression_string.find_first_of("+*-/<>?&|") == std::string::npos) {
        // Also check for responsive values
        if (expression_string.front() == '$' && expression_string.back() == '$') {
            std::string varName = expression_string.substr(1, expression_string.length() - 2);
            return std::make_unique<ResponsiveValueNode>(varName, ""); // Note: Unit handling needs refinement
        }
        return std::make_unique<StaticStyleNode>(expression_string);
    }

    // 3. If it's a complex expression, use the new ExpressionParser.
    ExpressionLexer expressionLexer(expression_string);
    std::vector<ExpressionToken> expressionTokens = expressionLexer.tokenize();

    ExpressionParser expressionParser(expressionTokens);
    std::unique_ptr<ExpressionBaseNode> expressionAst = expressionParser.parse();

    return std::make_unique<DynamicStyleNode>(std::move(expressionAst));
}

void StyleBlockState::parseClassOrIdSelector(Parser& parser) {
    if (!parser.contextNode) return;

    std::string selector_char = parser.currentToken.value;
    parser.advanceTokens(); // consume . or #

    std::string selector_name = parser.currentToken.value;
    parser.expectToken(TokenType::Identifier);

    // --- Automatic Attribute Injection ---
    std::string attribute_name = (selector_char == ".") ? "class" : "id";
    if (parser.contextNode->attributes.find(attribute_name) == parser.contextNode->attributes.end()) {
        parser.contextNode->attributes[attribute_name] = std::make_unique<StaticStyleNode>(selector_name);
    }

    // --- Hoisting the Style Rule ---
    std::string rule_body = parseCssRuleBlock(parser);
    std::stringstream ss;
    ss << selector_char << selector_name << " { " << rule_body << " }\n";
    parser.globalStyleContent += ss.str();
}

void StyleBlockState::parseAmpersandSelector(Parser& parser) {
    if (!parser.contextNode) return;

    parser.advanceTokens(); // consume &

    // The rest of the selector (e.g., :hover, ::before, [disabled])
    std::string pseudo_selector;
    while(parser.currentToken.type != TokenType::OpenBrace && parser.currentToken.type != TokenType::EndOfFile) {
        pseudo_selector += parser.currentToken.value;
        parser.advanceTokens();
    }

    // --- Determine the base selector ---
    std::string base_selector;
    // Prioritize ID, then class
    if (parser.contextNode->attributes.count("id")) {
        base_selector = "#" + parser.contextNode->attributes.at("id")->toString();
    } else if (parser.contextNode->attributes.count("class")) {
        // Just use the first class name for simplicity
        std::string full_class = parser.contextNode->attributes.at("class")->toString();
        std::stringstream class_ss(full_class);
        std::string first_class;
        class_ss >> first_class;
        base_selector = "." + first_class;
    } else {
        throw std::runtime_error("Cannot use '&' selector on an element with no class or id attribute.");
    }

    // --- Hoisting the Style Rule ---
    std::string rule_body = parseCssRuleBlock(parser);
    std::stringstream ss;
    ss << base_selector << pseudo_selector << " { " << rule_body << " }\n";
    parser.globalStyleContent += ss.str();
}

std::string StyleBlockState::parseCssRuleBlock(Parser& parser) {
    parser.expectToken(TokenType::OpenBrace);
    const auto& start_token = parser.currentToken;
    if (start_token.type == TokenType::CloseBrace) {
        parser.advanceTokens(); // Consume '}'
        return "";
    }

    size_t start_pos = start_token.start_pos;
    size_t end_pos = start_pos;
    int brace_level = 1;

    while (brace_level > 0 && parser.currentToken.type != TokenType::EndOfFile) {
        end_pos = parser.currentToken.start_pos + parser.currentToken.value.length();
        if (parser.currentToken.type == TokenType::OpenBrace) {
            brace_level++;
        } else if (parser.currentToken.type == TokenType::CloseBrace) {
            brace_level--;
            if (brace_level == 0) {
                end_pos = parser.currentToken.start_pos;
                break;
            }
        }
        parser.advanceTokens();
    }

    parser.expectToken(TokenType::CloseBrace);

    return parser.lexer.getSource().substr(start_pos, end_pos - start_pos);
}

void StyleBlockState::parseStyleTemplateUsage(Parser& parser) {
    parser.expectToken(TokenType::At);
    parser.expectKeyword(TokenType::Identifier, "KEYWORD_STYLE", "Style");

    std::string templateName = parser.currentToken.value;
    parser.expectToken(TokenType::Identifier);

    // --- Specialization Block Parsing ---
    std::map<std::string, std::unique_ptr<StyleValue>> specializedStyles;
    std::vector<std::string> deletedProperties;
    std::vector<std::string> deletedTemplates;

    if (parser.currentToken.type == TokenType::OpenBrace) {
        parser.advanceTokens(); // Consume '{'

        while (parser.currentToken.type != TokenType::CloseBrace && parser.currentToken.type != TokenType::EndOfFile) {
            if (parser.tryExpectKeyword(TokenType::Delete, "KEYWORD_DELETE", "delete")) {
                // Handle deletion
                do {
                    if (parser.currentToken.type == TokenType::At) {
                        // Deleting an inherited template, e.g., delete @Style Parent;
                        parser.advanceTokens(); // consume @
                        parser.expectKeyword(TokenType::Identifier, "KEYWORD_STYLE", "Style");
                        deletedTemplates.push_back(parser.currentToken.value);
                        parser.expectToken(TokenType::Identifier);
                    } else {
                        // Deleting a property, e.g., delete font-size;
                        deletedProperties.push_back(parser.currentToken.value);
                        parser.expectToken(TokenType::Identifier);
                    }
                } while (parser.currentToken.type == TokenType::Comma && (parser.advanceTokens(), true));
                parser.expectToken(TokenType::Semicolon);
            } else if (parser.currentToken.type == TokenType::Identifier) {
                // Handle overriding/adding a property
                std::string key = parser.currentToken.value;
                parser.advanceTokens();
                parser.expectToken(TokenType::Colon);
                specializedStyles[key] = parseStyleExpression(parser);
                if (parser.currentToken.type == TokenType::Semicolon) {
                    parser.advanceTokens();
                }
            } else {
                throw std::runtime_error("Unexpected token in style specialization block: " + parser.currentToken.value);
            }
        }
        parser.expectToken(TokenType::CloseBrace);
    } else {
        parser.expectToken(TokenType::Semicolon);
    }

    // --- Style Resolution ---
    std::map<std::string, std::unique_ptr<StyleValue>> finalStyles;
    std::vector<std::string> visitedTemplates;

    applyStyleTemplateRecursive(parser, parser.getCurrentNamespace(), templateName, finalStyles, deletedTemplates, visitedTemplates);

    // Apply specialized styles (overrides)
    for (auto& pair : specializedStyles) {
        finalStyles[pair.first] = std::move(pair.second);
    }

    // Remove deleted properties
    for (const auto& prop : deletedProperties) {
        finalStyles.erase(prop);
    }

    // Apply the resolved styles to the context node.
    for (const auto& pair : finalStyles) {
        parser.contextNode->inlineStyles[pair.first] = pair.second->clone();
    }
}

void StyleBlockState::applyStyleTemplateRecursive(
        Parser& parser,
        const std::string& ns,
        const std::string& templateName,
        std::map<std::string, std::unique_ptr<StyleValue>>& finalStyles,
        const std::vector<std::string>& deletedTemplates,
        std::vector<std::string>& visitedTemplates
    ) {

    // 0. Check if this template itself was deleted by a child.
    if (std::find(deletedTemplates.begin(), deletedTemplates.end(), templateName) != deletedTemplates.end()) {
        return;
    }

    // 1. Prevent circular inheritance.
    if (std::find(visitedTemplates.begin(), visitedTemplates.end(), templateName) != visitedTemplates.end()) {
        throw std::runtime_error("Circular inheritance detected in style templates involving '" + templateName + "'.");
    }
    visitedTemplates.push_back(templateName);

    // 2. Get the template from the manager.
    StyleTemplateNode* styleTemplate = parser.templateManager.getStyleTemplate(ns, templateName);
    if (!styleTemplate) {
        throw std::runtime_error("Style template '" + templateName + "' not found in namespace '" + ns + "'.");
    }

    // 3. Recursively apply parent templates first.
    for (const auto& parentName : styleTemplate->parentNames) {
        applyStyleTemplateRecursive(parser, ns, parentName, finalStyles, deletedTemplates, visitedTemplates);
    }

    // 4. Apply the styles from the current template.
    // This ensures that child styles overwrite parent styles.
    for (const auto& stylePair : styleTemplate->styles) {
        finalStyles[stylePair.first] = stylePair.second->clone();
    }
}