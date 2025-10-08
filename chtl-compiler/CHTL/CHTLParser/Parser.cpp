#include "chtl-compiler/CHTL/CHTLParser/Parser.h"
#include <stdexcept>
#include <iostream>
#include <sstream>

Parser::Parser(Lexer& lexer, Loader& loader, const std::string& currentFile)
    : lexer(lexer), loader(loader), currentFile(currentFile) {
    consume();
    consume(); // Initialize both currentToken and peekToken
}

void Parser::consume() {
    currentToken = peekToken;
    peekToken = lexer.getNextToken();
}

void Parser::eat(TokenType type) {
    if (currentToken.type == type) {
        consume();
    } else {
        std::string err = "Unexpected token: expected " + std::to_string((int)type) +
                          ", got " + std::to_string((int)currentToken.type) +
                          " ('" + currentToken.value + "') at line " + std::to_string(currentToken.line) +
                          ", col " + std::to_string(currentToken.column);
        throw std::runtime_error(err);
    }
}

NodeList Parser::parse() {
    NodeList nodes;
    auto& config = ConfigurationManager::getInstance();

    while(currentToken.type != TokenType::EndOfFile) {
        while(currentToken.type == TokenType::LineComment || currentToken.type == TokenType::BlockComment) {
            consume();
        }
        if (currentToken.type == TokenType::EndOfFile) break;

        if (currentToken.type == TokenType::LeftBracket) {
            if (peekToken.value == config.getKeyword("KEYWORD_IMPORT")) {
                parseImportStatement();
            } else if (peekToken.value == config.getKeyword("KEYWORD_CONFIGURATION")) {
                parseConfigurationBlock();
            } else {
                parseTemplateDefinition();
            }
        } else {
            NodeList stmts = parseStatement();
            if (!stmts.empty()) {
                nodes.insert(nodes.end(), stmts.begin(), stmts.end());
            } else if (currentToken.type != TokenType::EndOfFile) {
                throw std::runtime_error("Unexpected top-level token: " + currentToken.value);
            }
        }
    }
    loader.finishFile(currentFile);
    return nodes;
}

NodeList Parser::parseStatement() {
    auto& config = ConfigurationManager::getInstance();
    if (currentToken.type == TokenType::Identifier) {
        if (currentToken.value == config.getKeyword("KEYWORD_TEXT")) {
            return {parseText()};
        }
        return {parseElement()};
    } else if (currentToken.type == TokenType::GeneratorComment) {
        return {parseComment()};
    } else if (currentToken.type == TokenType::At) {
        return parseElementUsage();
    }
    return {};
}

NodePtr Parser::parseElement() {
    std::string tagName = currentToken.value;
    eat(TokenType::Identifier);

    auto element = std::make_shared<ElementNode>(tagName);

    eat(TokenType::LeftBrace);
    auto& config = ConfigurationManager::getInstance();

    while (currentToken.type != TokenType::RightBrace && currentToken.type != TokenType::EndOfFile) {
        while(currentToken.type == TokenType::LineComment || currentToken.type == TokenType::BlockComment) {
            consume();
        }
        if (currentToken.type == TokenType::RightBrace || currentToken.type == TokenType::EndOfFile) break;

        if (currentToken.type == TokenType::Identifier && (peekToken.type == TokenType::Colon || peekToken.type == TokenType::Equal)) {
            parseAttributes(element);
        } else if (currentToken.type == TokenType::Identifier && currentToken.value == config.getKeyword("KEYWORD_STYLE") && peekToken.type == TokenType::LeftBrace) {
            parseStyleBlock(element);
        } else {
            NodeList children = parseStatement();
            if (!children.empty()) {
                element->children.insert(element->children.end(), children.begin(), children.end());
            } else {
                 throw std::runtime_error("Unexpected token in element body: " + currentToken.value);
            }
        }
    }

    eat(TokenType::RightBrace);

    return element;
}

void Parser::parseAttributes(std::shared_ptr<ElementNode> element) {
    std::string key = currentToken.value;
    eat(TokenType::Identifier);

    if (currentToken.type == TokenType::Colon || currentToken.type == TokenType::Equal) {
        eat(currentToken.type);
    } else {
        throw std::runtime_error("Expected ':' or '=' for attribute.");
    }

    std::string value;
    if (currentToken.type == TokenType::Identifier || currentToken.type == TokenType::StringLiteral) {
        value = currentToken.value;
        eat(currentToken.type);
    } else {
        throw std::runtime_error("Attribute value must be an identifier or a string literal.");
    }

    element->attributes[key] = value;
    eat(TokenType::Semicolon);
}

NodePtr Parser::parseText() {
    eat(TokenType::Identifier); // consume 'text'
    eat(TokenType::LeftBrace);
    std::string content = currentToken.value;
    eat(TokenType::StringLiteral);
    eat(TokenType::RightBrace);

    return std::make_shared<TextNode>(content);
}

NodePtr Parser::parseComment() {
    std::string content = currentToken.value;
    eat(TokenType::GeneratorComment);
    return std::make_shared<CommentNode>(content);
}

void Parser::parseStyleBlock(std::shared_ptr<ElementNode> element) {
    auto& config = ConfigurationManager::getInstance();
    eat(TokenType::Identifier); // consume 'style'
    eat(TokenType::LeftBrace);

    std::map<std::string, std::string> evaluated_styles;

    while(currentToken.type != TokenType::RightBrace && currentToken.type != TokenType::EndOfFile) {
        if (currentToken.type == TokenType::At) {
            eat(TokenType::At);
            if (currentToken.value != config.getKeyword("TYPE_STYLE")) throw std::runtime_error("Expected 'Style' keyword.");
            eat(TokenType::Identifier);

            std::string templateName = currentToken.value;
            eat(TokenType::Identifier);
            StyleTemplate final_template = TemplateRegistry::getStyleTemplate(templateName);

            if (currentToken.type == TokenType::LeftBrace) {
                eat(TokenType::LeftBrace);
                while(currentToken.type != TokenType::RightBrace) {
                    if (currentToken.type == TokenType::DeleteKeyword) {
                        eat(TokenType::DeleteKeyword);
                        final_template.erase(currentToken.value);
                        eat(TokenType::Identifier);
                    } else {
                        std::string prop_key = currentToken.value;
                        eat(TokenType::Identifier);
                        eat(TokenType::Colon);
                        std::vector<Token> value_tokens;
                        while(currentToken.type != TokenType::Semicolon) {
                            value_tokens.push_back(currentToken);
                            consume();
                        }
                        ExpressionParser expr_parser(value_tokens);
                        final_template[prop_key] = expr_parser.parse()->evaluate({}); // No context for specialization
                    }
                    eat(TokenType::Semicolon);
                }
                eat(TokenType::RightBrace);
            } else {
                eat(TokenType::Semicolon);
            }

            for(const auto& pair : final_template) {
                if (pair.second.empty()) throw std::runtime_error("Valueless property not specialized: " + pair.first);
                evaluated_styles[pair.first] = pair.second;
            }
        } else {
            std::string property = currentToken.value;
            eat(TokenType::Identifier);
            eat(TokenType::Colon);

            std::vector<Token> value_tokens;
            while(currentToken.type != TokenType::Semicolon) {
                value_tokens.push_back(currentToken);
                consume();
            }

            bool has_operator = false;
            for(const auto& t : value_tokens) {
                if (t.type >= TokenType::Plus && t.type <= TokenType::Question) {
                    has_operator = true;
                    break;
                }
            }

            std::string value;
            if (has_operator) {
                ExpressionParser expr_parser(value_tokens);
                value = expr_parser.parse()->evaluate(evaluated_styles);
            } else {
                std::stringstream ss;
                for (size_t i = 0; i < value_tokens.size(); ++i) {
                    ss << value_tokens[i].value << (i < value_tokens.size() - 1 ? " " : "");
                }
                value = ss.str();
            }

            evaluated_styles[property] = value;
            eat(TokenType::Semicolon);
        }
    }
    eat(TokenType::RightBrace);

    std::stringstream final_style_string;
    for(const auto& pair : evaluated_styles) {
        final_style_string << pair.first << ":" << pair.second << ";";
    }

    if (element->attributes.count("style")) {
        element->attributes["style"] += final_style_string.str();
    } else {
        element->attributes["style"] = final_style_string.str();
    }
}

NodePtr Parser::parseTemplateDefinition() {
    auto& config = ConfigurationManager::getInstance();
    eat(TokenType::LeftBracket);
    std::string keyword = currentToken.value;
    if (keyword != config.getKeyword("KEYWORD_TEMPLATE") && keyword != config.getKeyword("KEYWORD_CUSTOM")) {
        throw std::runtime_error("Expected 'Template' or 'Custom' keyword in definition.");
    }
    eat(TokenType::Identifier);
    eat(TokenType::RightBracket);

    eat(TokenType::At);
    std::string templateType = currentToken.value;
    eat(TokenType::Identifier); // Consume "Style" or "Element"

    std::string templateName = currentToken.value;
    eat(TokenType::Identifier);

    eat(TokenType::LeftBrace);

    if (templateType == config.getKeyword("TYPE_STYLE")) {
        StyleTemplate style_template;
        while(currentToken.type != TokenType::RightBrace && currentToken.type != TokenType::EndOfFile) {
            while(currentToken.type == TokenType::LineComment || currentToken.type == TokenType::BlockComment) consume();
            if (currentToken.type == TokenType::RightBrace) break;

            std::string property = currentToken.value;
            eat(TokenType::Identifier);

            if (currentToken.type == TokenType::Colon) {
                eat(TokenType::Colon);
                std::vector<Token> value_tokens;
                while(currentToken.type != TokenType::Semicolon && currentToken.type != TokenType::EndOfFile) {
                    value_tokens.push_back(currentToken);
                    consume();
                }

                bool has_operator = false;
                for(const auto& t : value_tokens) {
                    if (t.type >= TokenType::Plus && t.type <= TokenType::Question) {
                        has_operator = true;
                        break;
                    }
                }

                std::string value;
                if (has_operator) {
                    ExpressionParser expr_parser(value_tokens);
                    value = expr_parser.parse()->evaluate({}); // No context for template defs
                } else {
                    std::stringstream ss;
                    for (size_t i = 0; i < value_tokens.size(); ++i) {
                        ss << value_tokens[i].value << (i < value_tokens.size() - 1 ? " " : "");
                    }
                    value = ss.str();
                }
                style_template[property] = value;
            } else {
                style_template[property] = "";
            }
            eat(TokenType::Semicolon);
        }
        TemplateRegistry::registerStyleTemplate(templateName, style_template);

    } else if (templateType == config.getKeyword("TYPE_ELEMENT")) {
        NodeList element_template_nodes;
        while(currentToken.type != TokenType::RightBrace && currentToken.type != TokenType::EndOfFile) {
            while(currentToken.type == TokenType::LineComment || currentToken.type == TokenType::BlockComment) consume();
            if (currentToken.type == TokenType::RightBrace) break;

            NodeList stmts = parseStatement();
            if (!stmts.empty()) {
                element_template_nodes.insert(element_template_nodes.end(), stmts.begin(), stmts.end());
            } else {
                throw std::runtime_error("Unexpected token in element template: " + currentToken.value);
            }
        }
        TemplateRegistry::registerElementTemplate(templateName, element_template_nodes);

    } else {
        throw std::runtime_error("Unsupported template type: @" + templateType);
    }

    eat(TokenType::RightBrace);
    return nullptr; // Definitions don't produce a node in the main AST
}

NodeList Parser::parseElementUsage() {
    auto& config = ConfigurationManager::getInstance();
    eat(TokenType::At);
    if (currentToken.value != config.getKeyword("TYPE_ELEMENT")) {
        throw std::runtime_error("Expected 'Element' keyword for template usage.");
    }
    eat(TokenType::Identifier); // Consume "Element"

    std::string templateName = currentToken.value;
    eat(TokenType::Identifier);

    NodeList clonedNodes;
    NodeList templateNodes = TemplateRegistry::getElementTemplate(templateName);
    for (const auto& node : templateNodes) {
        clonedNodes.push_back(node->clone());
    }

    // Check for specialization block
    if (currentToken.type == TokenType::LeftBrace) {
        eat(TokenType::LeftBrace);

        while (currentToken.type != TokenType::RightBrace && currentToken.type != TokenType::EndOfFile) {
            if (currentToken.type == TokenType::InsertKeyword) {
                eat(TokenType::InsertKeyword);
                eat(TokenType::AfterKeyword); // Assuming 'after' for now

                std::string targetTag = currentToken.value;
                eat(TokenType::Identifier);

                auto it = std::find_if(clonedNodes.begin(), clonedNodes.end(), [&](const NodePtr& n) {
                    auto el = std::dynamic_pointer_cast<ElementNode>(n);
                    return el && el->tagName == targetTag;
                });
                if (it == clonedNodes.end()) throw std::runtime_error("Target for insert not found: " + targetTag);

                eat(TokenType::LeftBrace);
                NodeList newNodes;
                while(currentToken.type != TokenType::RightBrace) {
                    NodeList stmts = parseStatement();
                    newNodes.insert(newNodes.end(), stmts.begin(), stmts.end());
                }
                eat(TokenType::RightBrace);

                clonedNodes.insert(it + 1, newNodes.begin(), newNodes.end());

            } else if (currentToken.type == TokenType::DeleteKeyword) {
                eat(TokenType::DeleteKeyword);
                std::string tagToDelete = currentToken.value;
                eat(TokenType::Identifier);

                clonedNodes.erase(std::remove_if(clonedNodes.begin(), clonedNodes.end(), [&](const NodePtr& n) {
                    auto el = std::dynamic_pointer_cast<ElementNode>(n);
                    return el && el->tagName == tagToDelete;
                }), clonedNodes.end());

                eat(TokenType::Semicolon);

            } else if (currentToken.type == TokenType::Identifier) { // Targeted Styling
                std::string targetTag = currentToken.value;
                auto it = std::find_if(clonedNodes.begin(), clonedNodes.end(), [&](const NodePtr& n) {
                    auto el = std::dynamic_pointer_cast<ElementNode>(n);
                    return el && el->tagName == targetTag;
                });
                if (it == clonedNodes.end()) throw std::runtime_error("Specialization target not found in template: " + targetTag);

                auto targetElement = std::dynamic_pointer_cast<ElementNode>(*it);
                eat(TokenType::Identifier);
                eat(TokenType::LeftBrace);
                if (currentToken.value == config.getKeyword("KEYWORD_STYLE")) {
                    parseStyleBlock(targetElement);
                } else {
                    throw std::runtime_error("Only 'style' blocks are supported in target specialization for now.");
                }
                eat(TokenType::RightBrace);
            } else {
                throw std::runtime_error("Unsupported specialization statement: " + currentToken.value);
            }
        }
        eat(TokenType::RightBrace);
    } else {
        eat(TokenType::Semicolon);
    }

    return clonedNodes;
}

void Parser::parseImportStatement() {
    auto& config = ConfigurationManager::getInstance();
    eat(TokenType::LeftBracket);
    if (currentToken.value != config.getKeyword("KEYWORD_IMPORT")) {
        throw std::runtime_error("Expected 'Import' keyword.");
    }
    eat(TokenType::Identifier);
    eat(TokenType::RightBracket);

    // For now, we only support [Import] @Chtl from "..."
    eat(TokenType::At);
    if (currentToken.value != config.getKeyword("TYPE_CHTL")) {
        throw std::runtime_error("Only '@Chtl' imports are supported for now.");
    }
    eat(TokenType::Identifier);

    if (currentToken.type != TokenType::FromKeyword) {
        throw std::runtime_error("Expected 'from' keyword in import statement.");
    }
    eat(TokenType::FromKeyword);

    std::string filepath = currentToken.value;
    eat(TokenType::StringLiteral);

    eat(TokenType::Semicolon);

    // Recursively parse the imported file
    std::string imported_content = loader.loadFile(filepath);
    Lexer imported_lexer(imported_content);
    Parser imported_parser(imported_lexer, loader, filepath);
    imported_parser.parse(); // This will populate the static TemplateRegistry
}

void Parser::parseConfigurationBlock() {
    auto& config = ConfigurationManager::getInstance();
    eat(TokenType::LeftBracket);
    eat(TokenType::Identifier); // Consume "Configuration"
    eat(TokenType::RightBracket);

    eat(TokenType::LeftBrace);

    while(currentToken.type != TokenType::RightBrace && currentToken.type != TokenType::EndOfFile) {
        std::string key = currentToken.value;
        eat(TokenType::Identifier);
        eat(TokenType::Equal);

        if (currentToken.type == TokenType::Identifier) {
            if (currentToken.value == "true") {
                config.setFlag(key, true);
                eat(TokenType::Identifier);
            } else if (currentToken.value == "false") {
                config.setFlag(key, false);
                eat(TokenType::Identifier);
            } else {
                // Assume it's a keyword string value
                config.setKeyword(key, currentToken.value);
                eat(TokenType::Identifier);
            }
        } else if (currentToken.type == TokenType::StringLiteral) {
             config.setKeyword(key, currentToken.value);
             eat(TokenType::StringLiteral);
        } else {
            throw std::runtime_error("Unsupported value in configuration block.");
        }

        eat(TokenType::Semicolon);
    }

    eat(TokenType::RightBrace);
}