#include "Parser.h"
#include "CHTL/CHTLState/StatementState.h" // Include the concrete state
#include "CHTL/CHTLState/ParserState.h" // Include full definition for destructor
#include "CHTL/CHTLNode/ElementNode.h"
#include "CHTL/CHTLNode/StyleTemplateNode.h"
#include "CHTL/CHTLNode/ElementTemplateNode.h"
#include "CHTL/CHTLNode/StaticStyleNode.h"
#include "libzippp/libzippp.h"
#include <stdexcept>
#include <sstream>
#include <filesystem>
#include <fstream>

// The constructor now initializes the token stream and sets the initial state.
Parser::Parser(Lexer& lexer, TemplateManager& tm, ConfigurationManager& cm, std::string source_path)
    : lexer(lexer),
      templateManager(tm),
      configManager(cm),
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
void Parser::parse() {
    this->ast.clear();

    while (currentToken.type != TokenType::EndOfFile) {
        // Handle top-level constructs that are not part of the regular state flow.
        if (currentToken.type == TokenType::OpenBracket && (peekToken.value == "Template" || peekToken.value == "Custom")) {
            parseTemplateDefinition();
            continue; // Template definitions do not produce a node in the main AST.
        }

        if (currentToken.type == TokenType::OpenBracket && peekToken.value == "Import") {
            handleImportStatement();
            continue;
        }

        if (!currentState) {
            throw std::runtime_error("Parser is not in a valid state.");
        }
        // Delegate parsing to the current state for regular elements.
        auto node = currentState->handle(*this);
        if (node) {
             this->ast.push_back(std::move(node));
        }
    }
}

void Parser::parseTemplateDefinition() {
    expectToken(TokenType::OpenBracket);
    bool isCustom = false;
    if (peekToken.value == "Custom") {
        isCustom = true;
        advanceTokens();
    } else {
        expectKeyword(TokenType::Identifier, "KEYWORD_TEMPLATE", "Template");
    }
    expectToken(TokenType::CloseBracket);

    expectToken(TokenType::At);
    if (currentToken.type != TokenType::Identifier) throw std::runtime_error("Expected template type (Style, Element, Var) after '@'.");
    std::string templateType = currentToken.value;
    advanceTokens();

    std::string templateName = currentToken.value;
    expectToken(TokenType::Identifier);

    expectToken(TokenType::OpenBrace);

    std::string currentNs = getCurrentNamespace();

    if (templateType == "Style") {
        auto styleNode = std::make_unique<StyleTemplateNode>();
        styleNode->isCustom = isCustom;
        while (currentToken.type != TokenType::CloseBrace) {
             if (currentToken.type == TokenType::Identifier) {
                std::string key = currentToken.value;
                advanceTokens();
                expectToken(TokenType::Colon);

                // This part requires a StyleBlockState, which we don't have here.
                // For now, we'll keep it simple and parse only static values.
                if (currentToken.type == TokenType::Identifier || currentToken.type == TokenType::Number || currentToken.type == TokenType::String) {
                    styleNode->styles[key] = std::make_unique<StaticStyleNode>(currentToken.value);
                    advanceTokens();
                } else {
                    throw std::runtime_error("Complex style expressions in templates are not fully supported in this context.");
                }

                if (currentToken.type == TokenType::Semicolon) {
                    advanceTokens();
                }
            } else {
                 throw std::runtime_error("Unexpected token in @Style definition: " + currentToken.value);
            }
        }
        templateManager.addStyleTemplate(currentNs, templateName, std::move(styleNode));
    } else if (templateType == "Element") {
        auto elementNode = std::make_unique<ElementTemplateNode>();
        elementNode->isCustom = isCustom;
        while (currentToken.type != TokenType::CloseBrace) {
            auto childNode = currentState->handle(*this);
            if (childNode) {
                 elementNode->children.push_back(std::move(childNode));
            }
        }
        templateManager.addElementTemplate(currentNs, templateName, std::move(elementNode));
    } else {
        throw std::runtime_error("Unknown template type: " + templateType);
    }

    expectToken(TokenType::CloseBrace);
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
    // Now searches the persistent AST owned by the parser.
    return findElementRecursive(selector, this->ast);
}

ElementNode* Parser::findElementRecursive(const std::string& selector, const std::vector<std::unique_ptr<BaseNode>>& nodes) {
    for (const auto& node : nodes) {
        if (node->getType() == NodeType::Element) {
            auto* element = static_cast<ElementNode*>(node.get());
            bool match = false;
            if (selector[0] == '#') {
                auto it = element->attributes.find("id");
                if (it != element->attributes.end() && it->second->toString() == selector.substr(1)) {
                    match = true;
                }
            } else if (selector[0] == '.') {
                auto it = element->attributes.find("class");
                if (it != element->attributes.end() && it->second->toString() == selector.substr(1)) {
                    match = true;
                }
            } else {
                if (element->tagName == selector) {
                    match = true;
                }
            }

            if (match) {
                return element;
            }

            // If not a match, search children
            ElementNode* found_in_children = findElementRecursive(selector, element->children);
            if (found_in_children) {
                return found_in_children;
            }
        }
    }
    return nullptr;
}


Parser::~Parser() = default;

std::string Parser::loadFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + path);
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

std::string Parser::getBasePath() const {
    try {
        if (sourcePath.empty()) return ".";
        return std::filesystem::path(sourcePath).parent_path().string();
    } catch (...) {
        return "."; // Fallback for invalid paths
    }
}

void Parser::processChtlImport(const std::string& path) {
    std::string basePath = getBasePath();
    std::string fullPath = (std::filesystem::path(basePath) / path).string();

    std::string importedContent = loadFile(fullPath);

    Lexer importedLexer(importedContent);
    // The new parser for the imported file shares the *same* managers as this parser.
    Parser importedParser(importedLexer, this->templateManager, this->configManager, fullPath);

    // Parse the imported file. This will populate the shared managers.
    importedParser.parse();
}

void Parser::processCssImport(const std::string& path) {
    std::string basePath = getBasePath();
    std::string fullPath = (std::filesystem::path(basePath) / path).string();
    std::string cssContent = loadFile(fullPath);

    this->globalStyleContent += "\n/* Imported from " + path + " */\n";
    this->globalStyleContent += cssContent;
    this->globalStyleContent += "\n";
}

std::map<std::string, std::string> Parser::loadCmod(const std::string& path) {
    libzippp::ZipArchive zf(path);
    zf.open(libzippp::ZipArchive::ReadOnly);
    std::vector<libzippp::ZipEntry> entries = zf.getEntries();
    std::map<std::string, std::string> fileContents;

    for (const auto& entry : entries) {
        if (!entry.isDirectory()) {
            fileContents[entry.getName()] = entry.readAsText();
        }
    }
    return fileContents;
}

void Parser::handleImportStatement() {
    // Consume '[', 'Import', ']'
    expectToken(TokenType::OpenBracket);
    expectKeyword(TokenType::Identifier, "KEYWORD_IMPORT", "Import");
    expectToken(TokenType::CloseBracket);

    expectToken(TokenType::At);
    std::string importTypeStr = currentToken.value;
    advanceTokens(); // Consume type

    expectKeyword(TokenType::Identifier, "KEYWORD_FROM", "from");

    std::string filePath;
    if (currentToken.type == TokenType::String) {
        filePath = currentToken.value;
        advanceTokens();
    } else if (currentToken.type == TokenType::Identifier) {
        filePath = currentToken.value;
        advanceTokens();
    } else {
        throw std::runtime_error("Expected a file path for [Import] statement.");
    }

    // Optional 'as' alias is ignored for now, but we need to parse it
    if (tryExpectKeyword(TokenType::Identifier, "KEYWORD_AS", "as")) {
        if (currentToken.type == TokenType::Identifier) {
            advanceTokens(); // Consume alias
        } else {
            throw std::runtime_error("Expected an alias identifier after 'as'.");
        }
    }

    if (currentToken.type == TokenType::Semicolon) {
        advanceTokens();
    }

    if (importTypeStr == "Chtl") {
        processChtlImport(filePath);
    } else if (importTypeStr == "Style") {
        processCssImport(filePath);
    } else {
        // For now, we can just ignore unsupported import types or log a warning.
    }
}
