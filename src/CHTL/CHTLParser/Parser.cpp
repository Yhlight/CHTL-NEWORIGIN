#include "Parser.h"
#include "CHTL/CHTLState/StatementState.h" // Include the concrete state
#include "CHTL/CHTLState/ParserState.h" // Include full definition for destructor
#include "CHTL/CHTLNode/ElementNode.h"
#include "CHTL/CHTLNode/StyleTemplateNode.h"
#include "CHTL/CHTLNode/StaticStyleNode.h"
#include <stdexcept>
#include <sstream>
#include <filesystem>

// The constructor now initializes the token stream and sets the initial state.
Parser::Parser(Lexer& lexer, std::string source_path)
    : lexer(lexer),
      currentToken({TokenType::Unexpected, ""}),
      peekToken({TokenType::Unexpected, ""}),
      peekToken2({TokenType::Unexpected, ""}),
      contextNode(nullptr),
      sourcePath(source_path) {
    // Initialize the token stream to fill all three lookahead tokens.
    advanceTokens();
    advanceTokens();
    advanceTokens();
    // Set the initial state for parsing.
    currentState = std::make_unique<StatementState>();
    namespaceStack.push_back("_global");
}

// The main parse loop now delegates to the current state's handle method.
std::vector<std::unique_ptr<BaseNode>> Parser::parse() {
    std::vector<std::unique_ptr<BaseNode>> statements;
    this->parsedNodes = &statements; // Point to the vector we are building.

    while (currentToken.type != TokenType::EndOfFile) {
        // Handle top-level constructs that are not part of the regular state flow.
        if (currentToken.type == TokenType::OpenBracket && peekToken.value == "Template") {
            parseTemplateDefinition();
            continue; // Template definitions do not produce a node in the main AST.
        }

        if (!currentState) {
            throw std::runtime_error("Parser is not in a valid state.");
        }
        // Delegate parsing to the current state for regular elements.
        auto node = currentState->handle(*this);
        if (node) {
             statements.push_back(std::move(node));
        }
    }

    this->parsedNodes = nullptr; // Clean up the pointer.
    return statements;
}

void Parser::parseTemplateDefinition() {
    expectToken(TokenType::OpenBracket);
    expectKeyword(TokenType::Identifier, "KEYWORD_TEMPLATE", "Template");
    expectToken(TokenType::CloseBracket);

    if (currentToken.type != TokenType::At) {
        throw std::runtime_error("Expected '@' for template type definition.");
    }
    advanceTokens(); // Consume '@'

    if (currentToken.value == "Style") {
        advanceTokens(); // Consume 'Style'
        std::string templateName = currentToken.value;
        expectToken(TokenType::Identifier);
        expectToken(TokenType::OpenBrace);

        auto styleTemplate = std::make_unique<StyleTemplateNode>();

        while (currentToken.type != TokenType::CloseBrace && currentToken.type != TokenType::EndOfFile) {
            std::string propertyName = currentToken.value;
            expectToken(TokenType::Identifier);
            expectToken(TokenType::Colon);

            // This is the new, robust value parser. It collects all tokens for a value
            // until a semicolon and then reconstructs the value string. This correctly
            // handles multi-token values like "16px" or "1px solid black" which are
            // now produced by the updated lexer.
            std::vector<Token> valueTokens;
            while (currentToken.type != TokenType::Semicolon && currentToken.type != TokenType::CloseBrace && currentToken.type != TokenType::EndOfFile) {
                valueTokens.push_back(currentToken);
                advanceTokens();
            }

            if (valueTokens.empty()) {
                throw std::runtime_error("Expected a value for the style property in template definition.");
            }

            std::stringstream value_ss;
            for (size_t i = 0; i < valueTokens.size(); ++i) {
                const auto& token = valueTokens[i];
                value_ss << token.value;

                // Add a space unless it's a number followed by a unit-like identifier.
                if (i < valueTokens.size() - 1) {
                    const auto& nextToken = valueTokens[i+1];
                    bool isUnit = (nextToken.type == TokenType::Identifier || nextToken.type == TokenType::Percent);
                    if (token.type != TokenType::Number || !isUnit) {
                         value_ss << " ";
                    }
                }
            }
            std::string propertyValue = value_ss.str();

            expectToken(TokenType::Semicolon);

            styleTemplate->styles[propertyName] = std::make_unique<StaticStyleNode>(propertyValue);
        }

        expectToken(TokenType::CloseBrace);
        templateManager.addStyleTemplate(getCurrentNamespace(), templateName, std::move(styleTemplate));

    } else {
        throw std::runtime_error("Unsupported template type: " + currentToken.value);
    }
}

// Allows changing the parser's state.
void Parser::setState(std::unique_ptr<ParserState> newState) {
    currentState = std::move(newState);
}

// --- Public methods for states to use ---

void Parser::advanceTokens() {
    currentToken = peekToken;
    peekToken = peekToken2;
    peekToken2 = lexer.getNextToken();
}

void Parser::expectToken(TokenType type) {
    if (currentToken.type == type) {
        advanceTokens();
    } else {
        throw std::runtime_error("Unexpected token: '" + currentToken.value + "' at line " +
                                 std::to_string(currentToken.line) + ", column " +
                                 std::to_string(currentToken.column) + ". Expected type " +
                                 std::to_string(static_cast<int>(type)));
    }
}

bool Parser::tryExpectKeyword(TokenType type, const std::string& internalName, const std::string& defaultValue) {
    const auto& aliases = configManager.getKeywordAliases(internalName);

    if (!aliases.empty()) {
        // User has defined aliases. Check only against them.
        for (const auto& alias : aliases) {
            if (currentToken.value == alias) {
                advanceTokens();
                return true;
            }
        }
        // If aliases are defined, we don't fall back to the default string value.
        return false;
    }

    // No aliases defined. Check against the default string value.
    if (currentToken.value == defaultValue) {
        advanceTokens();
        return true;
    }
    // As a final fallback for non-string-based tokens, check the token type.
    if (currentToken.type == type) {
        advanceTokens();
        return true;
    }

    return false;
}

void Parser::expectKeyword(TokenType type, const std::string& internalName, const std::string& defaultValue) {
    if (!tryExpectKeyword(type, internalName, defaultValue)) {
        const auto& aliases = configManager.getKeywordAliases(internalName);
        std::string expectedMessage;
        if (!aliases.empty()) {
            std::stringstream ss;
            ss << "one of [";
            for (size_t i = 0; i < aliases.size(); ++i) {
                ss << "'" << aliases[i] << "'" << (i < aliases.size() - 1 ? ", " : "");
            }
            ss << "]";
            expectedMessage = ss.str();
        } else {
            expectedMessage = "'" + defaultValue + "'";
        }
        throw std::runtime_error("Unexpected token: '" + currentToken.value + "'. Expected keyword " + expectedMessage + ".");
    }
}

std::string Parser::getCurrentNamespace() const {
    if (namespaceStack.empty()) {
        return "_global"; // Default namespace for top-level items
    }
    std::string result;
    for (size_t i = 0; i < namespaceStack.size(); ++i) {
        result += namespaceStack[i];
        if (i < namespaceStack.size() - 1) {
            result += ".";
        }
    }
    return result;
}

ElementNode* Parser::findElementBySelector(const std::string& selector) {
    if (!parsedNodes) {
        return nullptr;
    }
    return findElementRecursive(selector, *parsedNodes);
}

// This is the new, enhanced implementation of the recursive selector search.
// It now supports descendant selectors (e.g., ".container div") and
// indexed access (e.g., "div[1]").
ElementNode* Parser::findElementRecursive(const std::string& selector, const std::vector<std::unique_ptr<BaseNode>>& nodes) {
    std::stringstream ss(selector);
    std::string segment;
    std::vector<std::string> segments;
    while (ss >> segment) {
        segments.push_back(segment);
    }

    std::vector<ElementNode*> current_matches;
    for (const auto& node : nodes) {
        if (node->getType() == NodeType::Element) {
            current_matches.push_back(static_cast<ElementNode*>(node.get()));
        }
    }

    for (const auto& seg : segments) {
        std::vector<ElementNode*> next_matches;
        std::string tag_or_class_or_id = seg;
        int index = -1;

        // Check for indexed access, e.g., "div[1]"
        size_t bracket_pos = seg.find('[');
        if (bracket_pos != std::string::npos) {
            tag_or_class_or_id = seg.substr(0, bracket_pos);
            std::string index_str = seg.substr(bracket_pos + 1, seg.find(']') - bracket_pos - 1);
            try {
                index = std::stoi(index_str);
            } catch (const std::invalid_argument& e) {
                // Handle error: invalid index format
                return nullptr;
            }
        }

        std::vector<ElementNode*> segment_found_nodes;
        for (auto* element : current_matches) {
            // Search direct children for matches
            for (const auto& child_node : element->children) {
                 if (child_node->getType() == NodeType::Element) {
                    auto* child_element = static_cast<ElementNode*>(child_node.get());
                    bool match = false;
                    if (tag_or_class_or_id[0] == '#') {
                        auto it = child_element->attributes.find("id");
                        if (it != child_element->attributes.end() && it->second->toString() == tag_or_class_or_id.substr(1)) match = true;
                    } else if (tag_or_class_or_id[0] == '.') {
                        auto it = child_element->attributes.find("class");
                        if (it != child_element->attributes.end() && it->second->toString() == tag_or_class_or_id.substr(1)) match = true;
                    } else {
                        if (child_element->tagName == tag_or_class_or_id) match = true;
                    }
                    if(match) segment_found_nodes.push_back(child_element);
                 }
            }
        }

        if (index != -1) {
            if (index >= 0 && index < segment_found_nodes.size()) {
                next_matches.push_back(segment_found_nodes[index]);
            }
        } else {
            next_matches = segment_found_nodes;
        }

        current_matches = next_matches;
        if (current_matches.empty()) {
            return nullptr; // No match found for this segment
        }
    }

    return current_matches.empty() ? nullptr : current_matches.front();
}


Parser::~Parser() = default;
