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
    if (parser.currentToken.value != "Style") {
        throw std::runtime_error("Expected '@Style' for template usage.");
    }
    parser.advanceTokens(); // Consume 'Style'

    std::string templateName = parser.currentToken.value;
    parser.expectToken(TokenType::Identifier);
    parser.expectToken(TokenType::Semicolon);

    // Get the template from the manager
    const StyleTemplateNode* styleTemplate = parser.templateManager.getStyleTemplate(parser.getCurrentNamespace(), templateName);

    if (!styleTemplate) {
        throw std::runtime_error("Style template '" + templateName + "' not found in namespace '" + parser.getCurrentNamespace() + "'.");
    }

    // Apply the styles to the context node
    for (const auto& pair : styleTemplate->styles) {
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
        // Placeholder
}