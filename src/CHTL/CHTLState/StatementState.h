#pragma once

#include "ParserState.h"
#include "../CHTLNode/FragmentNode.h"

class ElementNode; // Forward declaration

// The state for parsing a single CHTL statement.
// This state is the main dispatcher that determines the type of statement
// (e.g., element, text block, comment) and contains the logic to parse it.
class StatementState : public ParserState {
public:
    // The handle method orchestrates the parsing of one full statement.
    std::unique_ptr<BaseNode> handle(Parser& parser) override;

private:
    // Helper methods to parse specific types of statements.
    // These contain logic moved from the original Parser class.
    std::unique_ptr<BaseNode> parseScriptElement(Parser& parser);
    std::unique_ptr<BaseNode> parseTextElement(Parser& parser);
    std::unique_ptr<BaseNode> parseComment(Parser& parser);
    void parseTemplateDefinition(Parser& parser);
    void parseUseDirective(Parser& parser);
    std::unique_ptr<BaseNode> parseOriginDefinition(Parser& parser);
    void parseNamespaceDefinition(Parser& parser);

    std::unique_ptr<BaseNode> parseElementTemplateUsage(Parser& parser);

    // Helper for specializing an element template usage
    void parseElementSpecializationBlock(Parser& parser, FragmentNode& fragment);
    void parseDeleteInSpecialization(Parser& parser, FragmentNode& fragment);
    void parseElementModificationInSpecialization(Parser& parser, FragmentNode& fragment);
    void parseInsertInSpecialization(Parser& parser, FragmentNode& fragment);

    // Helper for parsing an import statement
    void parseImportStatement(Parser& parser);

    // Helper for parsing an 'if' block
    std::unique_ptr<BaseNode> parseIfStatement(Parser& parser);

    // Helper for parsing an '[Export]' block
    void parseExportBlock(Parser& parser);
};
