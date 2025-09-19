#include "CHTL/CHTLParser/CHTLParser.h"
#include "CHTL/Expression/ExpressionParser.h"
#include "CHTL/Expression/ExpressionEvaluator.h"
#include <iostream>
#include <sstream>
#include <iomanip>


namespace CHTL {

CHTLParser::CHTLParser(const std::vector<Token>& tokens)
    : tokens(tokens), current_token_index(0) {}

std::unique_ptr<ElementNode> CHTLParser::parse() {
    std::unique_ptr<ElementNode> root_element = nullptr;
    while(peek().type != TokenType::END_OF_FILE) {
        if (peek().type == TokenType::LEFT_BRACKET) {
            parseTemplateDeclaration();
        } else if (peek().type == TokenType::IDENTIFIER) {
            if (root_element) {
                std::cerr << "Warning: Multiple root elements defined. Only the first one will be used." << std::endl;
                // a bit of a hack to consume the rest of the tokens for the rogue element
                parseElement();
            } else {
                root_element = parseElement();
            }
        } else {
             std::cerr << "Warning: Skipping unknown token at top level: " << peek().value << std::endl;
             advance();
        }
    }
    return root_element;
}

void CHTLParser::parseTopLevel() {
    // This function is now effectively the main parse loop body.
    // Kept the logic in parse() for simplicity for now.
}


Token CHTLParser::peek() {
    if (current_token_index >= tokens.size()) {
        return {TokenType::END_OF_FILE, "", 0, 0};
    }
    return tokens[current_token_index];
}

Token CHTLParser::advance() {
    if (current_token_index < tokens.size()) {
        current_token_index++;
    }
    return tokens[current_token_index - 1];
}

std::unique_ptr<TemplateDeclarationNode> CHTLParser::parseTemplateDeclaration() {
    auto templateNode = std::make_unique<TemplateDeclarationNode>();

    // Expect [Template]
    advance(); // consume [
    if (peek().type == TokenType::IDENTIFIER && peek().value == "Template") {
        advance();
    }
    if (peek().type == TokenType::RIGHT_BRACKET) {
        advance();
    }

    // Expect @Style
    if (peek().type == TokenType::AT_SIGN) {
        advance();
    }
    if (peek().type == TokenType::KEYWORD_STYLE || (peek().type == TokenType::IDENTIFIER && peek().value == "Style")) {
        templateNode->template_type = TemplateType::STYLE;
        advance();
    }

    // Expect template name
    if (peek().type == TokenType::IDENTIFIER) {
        templateNode->name = advance().value;
    }

    // Expect { ... } block of properties
    if (peek().type == TokenType::LEFT_BRACE) {
        advance();
        while(peek().type != TokenType::RIGHT_BRACE && peek().type != TokenType::END_OF_FILE) {
            templateNode->style_properties.push_back(parseAttribute());
        }
        if (peek().type == TokenType::RIGHT_BRACE) {
            advance();
        }
    }

    // Register it
    registry.registerStyleTemplate(templateNode->name, templateNode->style_properties);

    return templateNode;
}


Attribute CHTLParser::parseAttribute() {
    Attribute attr;
    attr.name = advance().value;

    if (peek().type != TokenType::COLON) return attr;
    advance();

    std::vector<Token> expression_tokens;
    while (peek().type != TokenType::SEMICOLON && peek().type != TokenType::RIGHT_BRACE && peek().type != TokenType::END_OF_FILE) {
        expression_tokens.push_back(advance());
    }

    if (expression_tokens.empty()) return attr;

    if (expression_tokens.size() == 1 && (expression_tokens[0].type == TokenType::IDENTIFIER || expression_tokens[0].type == TokenType::UNQUOTED_LITERAL || expression_tokens[0].type == TokenType::STRING_LITERAL)) {
        attr.value = expression_tokens[0].value;
    } else {
        try {
            ExpressionParser expr_parser(expression_tokens);
            auto expr_ast = expr_parser.parse();
            if (expr_ast) {
                EvaluationResult result = ExpressionEvaluator::evaluate(*expr_ast);
                std::stringstream ss;
                ss << std::fixed << std::setprecision(2) << result.value;
                std::string val_str = ss.str();
                val_str.erase(val_str.find_last_not_of('0') + 1, std::string::npos);
                if(val_str.back() == '.') {
                    val_str.pop_back();
                }
                attr.value = val_str + result.unit;
            }
        } catch (const std::exception& e) {
            std::cerr << "Error evaluating expression for attribute '" << attr.name << "': " << e.what() << std::endl;
            for(const auto& t : expression_tokens) {
                attr.value += t.value;
            }
        }
    }

    if (peek().type == TokenType::SEMICOLON) {
        advance();
    }
    return attr;
}

std::unique_ptr<TextNode> CHTLParser::parseText() {
    auto textNode = std::make_unique<TextNode>();
    advance();
    if (peek().type == TokenType::LEFT_BRACE) {
        advance();
        if (peek().type == TokenType::STRING_LITERAL) {
            textNode->content = advance().value;
        }
        if (peek().type == TokenType::RIGHT_BRACE) {
            advance();
        }
    }
    return textNode;
}

std::unique_ptr<StyleNode> CHTLParser::parseStyleBlock() {
    auto styleNode = std::make_unique<StyleNode>();
    advance(); // consume 'style' keyword

    if (peek().type != TokenType::LEFT_BRACE) return styleNode;
    advance(); // consume '{'

    while (peek().type != TokenType::RIGHT_BRACE && peek().type != TokenType::END_OF_FILE) {
        if (peek().type == TokenType::AT_SIGN) {
            // Handle @Style template usage
            advance(); // consume @
            if (peek().type == TokenType::KEYWORD_STYLE || (peek().type == TokenType::IDENTIFIER && peek().value == "Style")) {
                advance(); // consume Style
                if (peek().type == TokenType::IDENTIFIER) {
                    std::string template_name = advance().value;
                    const auto* template_content = registry.getStyleTemplate(template_name);
                    if (template_content) {
                        styleNode->inline_properties.insert(styleNode->inline_properties.end(), template_content->begin(), template_content->end());
                    } else {
                        std::cerr << "Warning: Style template '" << template_name << "' not found." << std::endl;
                    }
                    if (peek().type == TokenType::SEMICOLON) advance();
                }
            }
        } else if (peek().type == TokenType::DOT || peek().type == TokenType::HASH) {
            RuleNode rule;
            std::string selector;
            selector += advance().value;
            if (peek().type == TokenType::IDENTIFIER) {
                selector += advance().value;
            }
            rule.selector = selector;
            if (peek().type == TokenType::LEFT_BRACE) {
                advance();
                while (peek().type != TokenType::RIGHT_BRACE && peek().type != TokenType::END_OF_FILE) {
                    if (peek().type == TokenType::IDENTIFIER) {
                        rule.properties.push_back(parseAttribute());
                    } else {
                        advance();
                    }
                }
                if (peek().type == TokenType::RIGHT_BRACE) {
                    advance();
                }
            }
            styleNode->rules.push_back(rule);
        } else if (peek().type == TokenType::AMPERSAND) {
            RuleNode rule;
            std::string selector;
            selector += advance().value;
            while(peek().type != TokenType::LEFT_BRACE && peek().type != TokenType::END_OF_FILE) {
                selector += advance().value;
            }
            rule.selector = selector;
            if (peek().type == TokenType::LEFT_BRACE) {
                advance();
                while (peek().type != TokenType::RIGHT_BRACE && peek().type != TokenType::END_OF_FILE) {
                    if (peek().type == TokenType::IDENTIFIER) {
                        rule.properties.push_back(parseAttribute());
                    } else {
                        advance();
                    }
                }
                if (peek().type == TokenType::RIGHT_BRACE) {
                    advance();
                }
            }
            styleNode->rules.push_back(rule);
        }
        else if (peek().type == TokenType::IDENTIFIER) {
            styleNode->inline_properties.push_back(parseAttribute());
        } else {
            std::cerr << "Warning: Skipping unknown token in style block: " << peek().value << std::endl;
            advance();
        }
    }
    if (peek().type == TokenType::RIGHT_BRACE) {
        advance();
    }
    return styleNode;
}


std::unique_ptr<ElementNode> CHTLParser::parseElement() {
    auto element = std::make_unique<ElementNode>();
    element->tagName = advance().value;

    if (peek().type != TokenType::LEFT_BRACE) {
        return element;
    }
    advance();

    while (peek().type != TokenType::RIGHT_BRACE && peek().type != TokenType::END_OF_FILE) {
        if (peek().type == TokenType::KEYWORD_STYLE) {
            element->children.push_back(parseStyleBlock());
        } else if (peek().type == TokenType::KEYWORD_TEXT) {
            element->children.push_back(parseText());
        } else if (peek().type == TokenType::IDENTIFIER) {
            if (tokens[current_token_index + 1].type == TokenType::COLON) {
                element->attributes.push_back(parseAttribute());
            } else {
                element->children.push_back(parseElement());
            }
        } else {
            std::cerr << "Warning: Skipping unknown token in element body: " << peek().value << std::endl;
            advance();
        }
    }

    if (peek().type == TokenType::RIGHT_BRACE) {
        advance();
    }

    return element;
}

} // namespace CHTL
