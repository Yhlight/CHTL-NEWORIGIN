#include "StatementState.h"

#include "../CHTLParser/Parser.h"
#include "../Util/StyleUtil.h"
#include <algorithm>
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
#include "../CHTLNode/CommentNode.h"
#include "../CHTLNode/FragmentNode.h"
#include "../CHTLNode/OriginNode.h"
#include "../Util/NodeCloner.h"
#include "../CHTLLoader/Loader.h"
#include <stdexcept>
#include <string>
#include <utility> // For std::pair
#include <sstream>

// Forward declare to resolve circular dependency with StyleBlockState.h
class StyleBlockState;
class ConfigurationState;
#include "StyleBlockState.h"
#include "ConfigurationState.h"
#include "TemplateState.h" // Include the new state

// Forward declare to resolve circular dependency between element parsing and statement parsing
class ElementNode;

// The main handler for this state. It acts as a dispatcher.
std::unique_ptr<BaseNode> StatementState::handle(Parser& parser) {
    if (parser.currentToken.type == TokenType::OpenBracket) {
        // Peek ahead to see what's inside the brackets
        if (parser.peekToken.type == TokenType::Import) {
            parseImportStatement(parser);
            return nullptr;
        } else if (parser.peekToken.value == "Origin") {
            return parseOriginDefinition(parser);
        } else if (parser.peekToken.value == "Namespace") {
            parseNamespaceDefinition(parser);
            return nullptr; // Namespace definitions don't produce a node
        } else if (parser.peekToken.value == "Configuration") {
            ConfigurationState configState;
            configState.handle(parser);
            return nullptr; // Configuration does not produce a node
        } else {
            // Delegate template definitions to the new TemplateState
            TemplateState templateState;
            return templateState.handle(parser);
        }
    } else if (parser.currentToken.type == TokenType::Use) {
        parseUseDirective(parser);
        return nullptr; // `use` directive does not produce a node
    } else if (parser.currentToken.type == TokenType::At) {
        return parseElementTemplateUsage(parser);
    } else if (parser.currentToken.type == TokenType::Identifier) {
        // Check for constraints before parsing the element
        if (parser.contextNode && !parser.contextNode->constraints.empty()) {
            // Determine the type of the node we are about to parse
            ConstraintType upcomingType = ConstraintType::TagName;
            std::string upcomingIdentifier = "";
            bool isCustom = false;
            bool isTemplate = false;
            bool isOrigin = false;

            if (parser.currentToken.type == TokenType::OpenBracket) {
                if (parser.peekToken.value == "Custom") isCustom = true;
                else if (parser.peekToken.value == "Template") isTemplate = true;
                else if (parser.peekToken.value == "Origin") isOrigin = true;
            }

            if (parser.currentToken.type == TokenType::At) {
                 if (parser.peekToken.value == "Element") {
                     upcomingType = isCustom ? ConstraintType::CustomElement : ConstraintType::TemplateElement;
                     upcomingIdentifier = parser.peekToken2.value;
                 }
                 // Add other @ types if needed
            } else if (isOrigin) {
                // This is a generic [Origin] block, which we can consider as @Html for constraint purposes
                upcomingType = ConstraintType::Html;
            } else if (parser.currentToken.type == TokenType::Identifier) {
                upcomingType = ConstraintType::TagName;
                upcomingIdentifier = parser.currentToken.value;
            }


            for (const auto& constraint : parser.contextNode->constraints) {
                bool isForbidden = false;
                switch (constraint.type) {
                    case ConstraintType::TagName:
                        if (upcomingType == ConstraintType::TagName && constraint.identifier == upcomingIdentifier) isForbidden = true;
                        break;
                    case ConstraintType::CustomElement:
                        if (upcomingType == ConstraintType::CustomElement && constraint.identifier == upcomingIdentifier) isForbidden = true;
                        break;
                    case ConstraintType::TemplateElement:
                        if (upcomingType == ConstraintType::TemplateElement && constraint.identifier == upcomingIdentifier) isForbidden = true;
                        break;
                    case ConstraintType::Html:
                        // This forbids any plain tag, and also Origin blocks
                        if (upcomingType == ConstraintType::TagName || upcomingType == ConstraintType::Html) isForbidden = true;
                        break;
                    case ConstraintType::Custom:
                        if (isCustom) isForbidden = true;
                        break;
                    case ConstraintType::Template:
                        if (isTemplate) isForbidden = true;
                        break;
                    default:
                        break;
                }

                if (isForbidden) {
                     throw std::runtime_error("Usage of '" + upcomingIdentifier + "' is not allowed inside <" + parser.contextNode->tagName + "> due to an 'except' constraint.");
                }
            }
        }

        if (parser.currentToken.value == "text") {
            return parseTextElement(parser);
        }
        // Any other identifier is assumed to be an element tag.
        return parseElement(parser);
    } else if (parser.currentToken.type == TokenType::HashComment) {
        return parseComment(parser);
    }

    throw std::runtime_error("Statements must begin with '[', an identifier, or hash comment. Found '" + parser.currentToken.value + "' instead.");
}

#include "../CHTLNode/Constraint.h" // Add include for new Constraint struct

void StatementState::parseExceptClause(Parser& parser, ElementNode& element) {
    // The 'except' keyword has already been consumed by tryExpectKeyword in the caller.

    while (parser.currentToken.type != TokenType::Semicolon && parser.currentToken.type != TokenType::EndOfFile) {
        Constraint constraint;
        bool isCustom = false;
        bool isTemplate = false;

        // Check for [Custom] or [Template]
        if (parser.currentToken.type == TokenType::OpenBracket) {
            if (parser.peekToken.value == "Custom") {
                isCustom = true;
                parser.advanceTokens(); // [
                parser.advanceTokens(); // Custom
                parser.expectToken(TokenType::CloseBracket);
            } else if (parser.peekToken.value == "Template") {
                isTemplate = true;
                parser.advanceTokens(); // [
                parser.advanceTokens(); // Template
                parser.expectToken(TokenType::CloseBracket);
            }
        }

        if (parser.currentToken.type == TokenType::At) {
            parser.advanceTokens(); // @
            std::string typeName = parser.currentToken.value;
            parser.expectToken(TokenType::Identifier);

            if (typeName == "Html") {
                constraint.type = ConstraintType::Html;
            } else if (typeName == "Element") {
                if(isCustom) constraint.type = ConstraintType::CustomElement;
                else constraint.type = ConstraintType::TemplateElement;
                constraint.identifier = parser.currentToken.value;
                parser.advanceTokens();
            } else if (typeName == "Var") {
                constraint.type = ConstraintType::TemplateVar;
                constraint.identifier = parser.currentToken.value;
                parser.advanceTokens();
            }
            // Add other @-types here if needed
        } else if (isCustom) {
            constraint.type = ConstraintType::Custom;
        } else if (isTemplate) {
            constraint.type = ConstraintType::Template;
        } else if (parser.currentToken.type == TokenType::Identifier) {
            // Simple tag name constraint
            constraint.type = ConstraintType::TagName;
            constraint.identifier = parser.currentToken.value;
            parser.advanceTokens();
        } else {
            throw std::runtime_error("Invalid token in 'except' clause: " + parser.currentToken.value);
        }

        element.constraints.push_back(constraint);

        // Consume comma if it exists
        if (parser.currentToken.type == TokenType::Comma) {
            parser.advanceTokens();
        } else {
            break; // End of list
        }
    }
    parser.expectToken(TokenType::Semicolon);
}


// Parses a full element, including its body.
std::unique_ptr<BaseNode> StatementState::parseElement(Parser& parser) {
    auto element = std::make_unique<ElementNode>(parser.currentToken.value);
    parser.expectToken(TokenType::Identifier);
    parser.expectToken(TokenType::OpenBrace);

    parseElementBody(parser, *element);

    parser.expectToken(TokenType::CloseBrace);
    return element;
}

// Parses the body of an element, dispatching to helpers for attributes, styles, or child nodes.
void StatementState::parseElementBody(Parser& parser, ElementNode& element) {
    // Set the context node so that child parsing can be aware of the parent.
    parser.contextNode = &element;

    while (parser.currentToken.type != TokenType::CloseBrace && parser.currentToken.type != TokenType::EndOfFile) {
        if (parser.currentToken.type == TokenType::Identifier && parser.currentToken.value == "style" && parser.peekToken.type == TokenType::OpenBrace) {
            StyleBlockState styleState;
            styleState.handle(parser); // This will parse the entire style block.
        } else if (parser.tryExpectKeyword(TokenType::Except, "KEYWORD_EXCEPT", "except")) {
            parseExceptClause(parser, element);
        }
        else if (parser.currentToken.type == TokenType::Identifier && (parser.peekToken.type == TokenType::Colon || parser.peekToken.type == TokenType::Equals)) {
            parseAttribute(parser, element);
        } else {
            auto childNode = handle(parser);
            if (childNode) {
                 element.children.push_back(std::move(childNode));
            }
        }
    }
    // Reset the context node after parsing the body.
    parser.contextNode = nullptr;
}

// Parses a 'text { ... }' block, allowing unquoted literals.
std::unique_ptr<BaseNode> StatementState::parseTextElement(Parser& parser) {
    parser.expectToken(TokenType::Identifier); // consume 'text'
    parser.expectToken(TokenType::OpenBrace);

    std::stringstream textContent;
    bool firstToken = true;
    while (parser.currentToken.type != TokenType::CloseBrace && parser.currentToken.type != TokenType::EndOfFile) {
        if (!firstToken) {
            textContent << " ";
        }
        textContent << parser.currentToken.value;
        parser.advanceTokens();
        firstToken = false;
    }

    parser.expectToken(TokenType::CloseBrace);
    return std::make_unique<TextNode>(textContent.str());
}

// Parses a '# comment' line.
std::unique_ptr<BaseNode> StatementState::parseComment(Parser& parser) {
    auto comment = std::make_unique<CommentNode>(parser.currentToken.value);
    parser.expectToken(TokenType::HashComment);
    return comment;
}

// Parses an attribute 'key: value;'. The value is parsed as a style expression.
void StatementState::parseAttribute(Parser& parser, ElementNode& element) {
    std::string key = parser.currentToken.value;
    parser.expectToken(TokenType::Identifier);

    if (parser.currentToken.type != TokenType::Colon && parser.currentToken.type != TokenType::Equals) {
        throw std::runtime_error("Expected ':' or '=' after attribute key '" + key + "'.");
    }
    parser.advanceTokens(); // Consume ':' or '='

    // Use the global expression parser
    StyleValue value = parseStyleExpression(parser);

    parser.expectToken(TokenType::Semicolon);

    if (key == "text") {
        // For the special 'text' attribute, convert the value to a string and create a TextNode.
        element.children.push_back(std::make_unique<TextNode>(styleValueToString(value)));
    } else {
        // For all other attributes, store the parsed StyleValue.
        element.attributes[key] = value;
    }
}

std::unique_ptr<BaseNode> StatementState::parseElementTemplateUsage(Parser& parser) {
    parser.expectToken(TokenType::At);
    if (parser.currentToken.value != "Element") {
        throw std::runtime_error("Expected 'Element' after '@' for template usage.");
    }
    parser.expectToken(TokenType::Identifier); // consume "Element"

    std::string templateName = parser.currentToken.value;
    parser.expectToken(TokenType::Identifier);

    // Handle optional 'from <namespace>' clause
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

    // Get the template from the manager
    ElementTemplateNode* tmpl = parser.templateManager.getElementTemplate(ns, templateName);
    if (!tmpl) {
        throw std::runtime_error("Element template not found: " + templateName);
    }

    // Create a fragment node to hold the cloned children.
    auto fragment = std::make_unique<FragmentNode>();
    for (const auto& child : tmpl->children) {
        auto clonedNode = NodeCloner::clone(child.get());
        clonedNode->sourceTemplateName = templateName;
        fragment->children.push_back(std::move(clonedNode));
    }

    // Handle specialization block for custom templates
    if (tmpl->isCustom) {
        if (parser.currentToken.type == TokenType::OpenBrace) {
            parseElementSpecializationBlock(parser, *fragment);
        }
    } else {
        parser.expectToken(TokenType::Semicolon);
    }


    return fragment;
}

// Forward declarations for helpers
void parseDeleteInSpecialization(Parser& parser, FragmentNode& fragment);
void parseStyleModificationInSpecialization(Parser& parser, FragmentNode& fragment);
void parseInsertInSpecialization(Parser& parser, FragmentNode& fragment);


void StatementState::parseElementSpecializationBlock(Parser& parser, FragmentNode& fragment) {
    parser.expectToken(TokenType::OpenBrace);

    while(parser.currentToken.type != TokenType::CloseBrace) {
        if (parser.currentToken.type == TokenType::Delete) {
            parseDeleteInSpecialization(parser, fragment);
        } else if (parser.currentToken.type == TokenType::Insert) {
            parseInsertInSpecialization(parser, fragment);
        } else if (parser.currentToken.type == TokenType::Identifier) {
            parseStyleModificationInSpecialization(parser, fragment);
        } else {
            // To prevent infinite loops on unexpected tokens
            parser.advanceTokens();
        }
    }

    parser.expectToken(TokenType::CloseBrace);
}

void parseInsertInSpecialization(Parser& parser, FragmentNode& fragment) {
    parser.expectKeyword(TokenType::Insert, "KEYWORD_INSERT", "insert");

    // 1. Parse position
    TokenType posToken = TokenType::Unexpected;
    if (parser.tryExpectKeyword(TokenType::After, "KEYWORD_AFTER", "after")) {
        posToken = TokenType::After;
    } else if (parser.tryExpectKeyword(TokenType::Before, "KEYWORD_BEFORE", "before")) {
        posToken = TokenType::Before;
    } else if (parser.tryExpectKeyword(TokenType::Replace, "KEYWORD_REPLACE", "replace")) {
        posToken = TokenType::Replace;
    } else if (parser.tryExpectKeyword(TokenType::AtTop, "KEYWORD_ATTOP", "at top")) {
        posToken = TokenType::AtTop;
    } else if (parser.tryExpectKeyword(TokenType::AtBottom, "KEYWORD_ATBOTTOM", "at bottom")) {
        posToken = TokenType::AtBottom;
    } else {
        throw std::runtime_error("Invalid position for insert statement. Found '" + parser.currentToken.value + "'.");
    }

    // 2. Find target node iterator (if needed)
    std::vector<std::unique_ptr<BaseNode>>::iterator targetIt;
    bool needsTarget = (posToken == TokenType::After || posToken == TokenType::Before || posToken == TokenType::Replace);
    if (needsTarget) {
        std::string tagName = parser.currentToken.value;
        parser.expectToken(TokenType::Identifier);
        parser.expectToken(TokenType::OpenBracket);
        int index = std::stoi(parser.currentToken.value);
        parser.expectToken(TokenType::Number);
        parser.expectToken(TokenType::CloseBracket);

        int current_tag_index = 0;
        bool found = false;
        for (targetIt = fragment.children.begin(); targetIt != fragment.children.end(); ++targetIt) {
            if ((*targetIt)->getType() == NodeType::Element) {
                auto* element = static_cast<ElementNode*>((*targetIt).get());
                if (element->tagName == tagName) {
                    if (current_tag_index == index) {
                        found = true;
                        break;
                    }
                    current_tag_index++;
                }
            }
        }
        if (!found) {
            throw std::runtime_error("Could not find target for insert statement.");
        }
    }

    // 3. Parse the block of new nodes
    parser.expectToken(TokenType::OpenBrace);
    std::vector<std::unique_ptr<BaseNode>> newNodes;
    while(parser.currentToken.type != TokenType::CloseBrace) {
        StatementState s; // Create a temp state to parse the inner nodes
        newNodes.push_back(s.handle(parser));
    }
    parser.expectToken(TokenType::CloseBrace);

    // 4. Perform the insertion
    if (posToken == TokenType::AtTop) {
        fragment.children.insert(fragment.children.begin(), std::make_move_iterator(newNodes.begin()), std::make_move_iterator(newNodes.end()));
    } else if (posToken == TokenType::AtBottom) {
        fragment.children.insert(fragment.children.end(), std::make_move_iterator(newNodes.begin()), std::make_move_iterator(newNodes.end()));
    } else if (posToken == TokenType::Before) {
        fragment.children.insert(targetIt, std::make_move_iterator(newNodes.begin()), std::make_move_iterator(newNodes.end()));
    } else if (posToken == TokenType::After) {
        fragment.children.insert(std::next(targetIt), std::make_move_iterator(newNodes.begin()), std::make_move_iterator(newNodes.end()));
    } else if (posToken == TokenType::Replace) {
        targetIt = fragment.children.erase(targetIt);
        fragment.children.insert(targetIt, std::make_move_iterator(newNodes.begin()), std::make_move_iterator(newNodes.end()));
    }
}

void parseStyleModificationInSpecialization(Parser& parser, FragmentNode& fragment) {
    std::string tagName = parser.currentToken.value;
    parser.expectToken(TokenType::Identifier);

    int index = 0; // Default to first element if no index provided
    bool indexProvided = false;
    if (parser.currentToken.type == TokenType::OpenBracket) {
        indexProvided = true;
        parser.advanceTokens();
        if (parser.currentToken.type != TokenType::Number) {
            throw std::runtime_error("Expected index number inside [].");
        }
        index = std::stoi(parser.currentToken.value);
        parser.advanceTokens();
        parser.expectToken(TokenType::CloseBracket);
    }

    // Find the target node
    ElementNode* targetNode = nullptr;
    int current_tag_index = 0;
    for (auto& child : fragment.children) {
        if (child->getType() == NodeType::Element) {
            auto* element = static_cast<ElementNode*>(child.get());
            if (element->tagName == tagName) {
                if (current_tag_index == index) {
                    targetNode = element;
                    break;
                }
                current_tag_index++;
            }
        }
    }

    if (!targetNode) {
        throw std::runtime_error("Could not find element '" + tagName + "' with index " + std::to_string(index) + " to modify.");
    }

    // Parse the modification block
    parser.expectToken(TokenType::OpenBrace);
    while(parser.currentToken.type != TokenType::CloseBrace) {
        if (parser.currentToken.type == TokenType::Identifier && parser.currentToken.value == "style") {
            // Use the StyleBlockState to parse the style block, but with the targetNode as context.
            parser.contextNode = targetNode;
            StyleBlockState styleState;
            styleState.handle(parser);
            parser.contextNode = nullptr; // Reset context
        } else {
            throw std::runtime_error("Only 'style' blocks are allowed inside an element specialization.");
        }
    }
    parser.expectToken(TokenType::CloseBrace);
}

void parseDeleteInSpecialization(Parser& parser, FragmentNode& fragment) {
    parser.expectKeyword(TokenType::Delete, "KEYWORD_DELETE", "delete");

    while (parser.currentToken.type != TokenType::Semicolon) {
        if (parser.currentToken.type == TokenType::At) {
            parser.advanceTokens(); // consume '@'
            if (parser.currentToken.value != "Element") {
                throw std::runtime_error("Can only delete @Element templates in this context.");
            }
            parser.advanceTokens(); // consume 'Element'
            std::string templateNameToDelete = parser.currentToken.value;
            parser.advanceTokens();

            // Erase-remove idiom to delete all nodes from the specified template
            fragment.children.erase(
                std::remove_if(fragment.children.begin(), fragment.children.end(),
                               [&](const std::unique_ptr<BaseNode>& node) {
                                   return node->sourceTemplateName == templateNameToDelete;
                               }),
                fragment.children.end());

        } else if (parser.currentToken.type == TokenType::Identifier) {
            std::string tagName = parser.currentToken.value;
            parser.advanceTokens();

            int index = -1; // -1 means delete all matching tags
            if (parser.currentToken.type == TokenType::OpenBracket) {
                parser.advanceTokens();
                if (parser.currentToken.type != TokenType::Number) {
                    throw std::runtime_error("Expected index number inside [].");
                }
                index = std::stoi(parser.currentToken.value);
                parser.advanceTokens();
                parser.expectToken(TokenType::CloseBracket);
            }

            auto& children = fragment.children;
            int current_tag_index = 0;
            for (auto it = children.begin(); it != children.end(); ) {
                bool removed = false;
                if ((*it)->getType() == NodeType::Element) {
                    auto* element = static_cast<ElementNode*>((*it).get());
                    if (element->tagName == tagName) {
                        if (index == -1 || index == current_tag_index) {
                            it = children.erase(it);
                            removed = true;
                            if (index != -1) break;
                        }
                        current_tag_index++;
                    }
                }
                if (!removed) {
                    ++it;
                }
            }
        } else {
             throw std::runtime_error("Expected tag name or @Element after 'delete'.");
        }


        if (parser.currentToken.type == TokenType::Comma) {
            parser.advanceTokens();
        } else {
            break;
        }
    }
    parser.expectToken(TokenType::Semicolon);
}

void StatementState::parseUseDirective(Parser& parser) {
    parser.expectToken(TokenType::Use);
    if (parser.currentToken.value == "html5") {
        parser.outputHtml5Doctype = true;
        parser.expectToken(TokenType::Identifier);
        parser.expectToken(TokenType::Semicolon);
    } else {
        throw std::runtime_error("Unsupported 'use' directive: " + parser.currentToken.value);
    }
}

std::unique_ptr<BaseNode> StatementState::parseOriginDefinition(Parser& parser) {
    parser.expectToken(TokenType::OpenBracket);
    parser.expectToken(TokenType::Origin);
    parser.expectToken(TokenType::CloseBracket);
    parser.expectToken(TokenType::At);

    std::string type = parser.currentToken.value;
    parser.expectToken(TokenType::Identifier);

    std::string name = parser.currentToken.value;
    parser.expectToken(TokenType::Identifier);

    // Check if this is a definition or a usage
    if (parser.currentToken.type == TokenType::OpenBrace) {
        // --- Definition: [Origin] @Type name { ... } ---
        parser.advanceTokens(); // Consume '{'

        std::stringstream rawContent;
        int braceLevel = 1;

        while (true) {
            if (parser.currentToken.type == TokenType::EndOfFile) {
                throw std::runtime_error("Unmatched braces in [Origin] block.");
            }
            if (parser.currentToken.type == TokenType::OpenBrace) {
                braceLevel++;
            } else if (parser.currentToken.type == TokenType::CloseBrace) {
                braceLevel--;
                if (braceLevel == 0) {
                    break; // Found the end, do not include the closing brace in content.
                }
            }
            rawContent << parser.currentToken.value;
            parser.advanceTokens();
        }

        // Consume the final '}'
        parser.expectToken(TokenType::CloseBrace);

        auto originNode = std::make_unique<OriginNode>(type, rawContent.str(), name);

        // Store a clone in the manager for later use
        auto cloneForManager = NodeCloner::clone(originNode.get());
        parser.templateManager.addNamedOrigin(parser.getCurrentNamespace(), name, std::unique_ptr<OriginNode>(static_cast<OriginNode*>(cloneForManager.release())));

        return originNode;

    } else if (parser.currentToken.type == TokenType::Semicolon) {
        // --- Usage: [Origin] @Type name; ---
        parser.advanceTokens(); // Consume ';'

        std::string ns = parser.getCurrentNamespace(); // For now, only look in current namespace
        OriginNode* storedNode = parser.templateManager.getNamedOrigin(ns, name);
        if (!storedNode) {
            throw std::runtime_error("Named origin block '" + name + "' not found.");
        }
        // Return a clone of the stored node
        return NodeCloner::clone(storedNode);
    } else {
        throw std::runtime_error("Invalid syntax for [Origin] block. Expected '{' for definition or ';' for usage.");
    }
}

void StatementState::parseNamespaceDefinition(Parser& parser) {
    // 1. Expect [Namespace] name
    parser.expectToken(TokenType::OpenBracket);
    parser.expectToken(TokenType::Namespace);
    parser.expectToken(TokenType::CloseBracket);
    std::string ns_name = parser.currentToken.value;
    parser.expectToken(TokenType::Identifier);

    // 2. Push namespace onto stack
    parser.namespaceStack.push_back(ns_name);

    // 3. Parse body
    parser.expectToken(TokenType::OpenBrace);
    while(parser.currentToken.type != TokenType::CloseBrace) {
        // A namespace can contain template definitions, other namespaces, etc.
        // We can just recursively call the main handle method.
        handle(parser);
    }
    parser.expectToken(TokenType::CloseBrace);

    // 4. Pop namespace from stack
    parser.namespaceStack.pop_back();
}

void StatementState::parseImportStatement(Parser& parser) {
    parser.expectToken(TokenType::OpenBracket);
    parser.expectToken(TokenType::Import);

    // This is where we'd parse the optional [Custom], [Template] part
    // For now, we'll skip this and assume a simple @Type syntax for all imports.
    if (parser.currentToken.type == TokenType::CloseBracket) {
         parser.advanceTokens(); // Consume ']'
    } else {
        // In a full implementation, parse [Custom] etc. here
        parser.advanceTokens();
        parser.advanceTokens();
    }

    parser.expectToken(TokenType::At);
    std::string importType = parser.currentToken.value;
    parser.expectToken(TokenType::Identifier);

    // For precise CHTL imports, the name of the item to import comes next.
    std::string itemName;
    if (parser.currentToken.type == TokenType::Identifier && parser.peekToken.type == TokenType::From) {
        itemName = parser.currentToken.value;
        parser.advanceTokens();
    }

    parser.expectKeyword(TokenType::From, "KEYWORD_FROM", "from");

    if (parser.currentToken.type != TokenType::String) {
        throw std::runtime_error("Import path must be a string literal.");
    }
    std::string path = parser.currentToken.value;
    parser.advanceTokens();

    std::string alias;
    if (parser.tryExpectKeyword(TokenType::Identifier, "KEYWORD_AS", "as")) {
        alias = parser.currentToken.value;
        parser.expectToken(TokenType::Identifier);
    }

    parser.expectToken(TokenType::Semicolon);

    // --- Import Logic ---
    if (importType == "Chtl") {
        // This is now a full file import
        try {
            std::string fileContent = Loader::loadFile(path);
            Lexer importLexer(fileContent);
            Parser importParser(importLexer);
            importParser.parse();
            parser.templateManager.merge(importParser.templateManager);
        } catch (const std::runtime_error& e) {
            throw std::runtime_error("Failed to import file '" + path + "': " + e.what());
        }
    } else if (importType == "Html" || importType == "Style" || importType == "JavaScript") {
        if (alias.empty()) throw std::runtime_error("Import for non-CHTL types must use 'as'.");
        try {
            std::string fileContent = Loader::loadFile(path);
            auto originNode = std::make_unique<OriginNode>(importType, fileContent, alias);
            parser.templateManager.addNamedOrigin(parser.getCurrentNamespace(), alias, std::move(originNode));
        } catch (const std::runtime_error& e) {
            throw std::runtime_error("Failed to import file '" + path + "' as origin block '" + alias + "': " + e.what());
        }
    } else if (importType == "Element" || importType == "Var" || importType == "Style") { // Precise imports
         if (itemName.empty()) throw std::runtime_error("Precise import requires an item name before 'from'.");
         try {
            std::string fileContent = Loader::loadFile(path);
            Lexer importLexer(fileContent);
            Parser importParser(importLexer);
            importParser.parse();

            std::string finalName = alias.empty() ? itemName : alias;

            if (importType == "Element") {
                ElementTemplateNode* nodeToImport = importParser.templateManager.getElementTemplate("_global", itemName);
                if (!nodeToImport) throw std::runtime_error("Element template '" + itemName + "' not found in " + path);
                auto cloned_node = NodeCloner::clone(nodeToImport);
                parser.templateManager.addElementTemplate(parser.getCurrentNamespace(), finalName, std::unique_ptr<ElementTemplateNode>(static_cast<ElementTemplateNode*>(cloned_node.release())));
            }
            // Add similar logic for Style and Var templates here...

         } catch (const std::runtime_error& e) {
            throw std::runtime_error("Failed to import '" + itemName + "' from file '" + path + "': " + e.what());
        }

    } else {
        throw std::runtime_error("Unsupported import type: @" + importType);
    }
}