#pragma once

#include "ParserState.h"
#include "../CHTLNode/StyleValue.h"
#include <string>
#include <map>
#include <memory>
#include <vector>

// Forward declare to avoid circular dependencies
class ElementNode;

// State for parsing the contents of a 'style { ... }' block.
class StyleBlockState : public ParserState {
public:
    std::unique_ptr<BaseNode> handle(Parser& parser) override;

    // --- Expression Parsing ---
    std::unique_ptr<StyleValue> parseStyleExpression(Parser& parser);

private:
    void parseStyleTemplateUsage(Parser& parser);
    void parseClassOrIdSelector(Parser& parser);
    void parseAmpersandSelector(Parser& parser);
    std::string parseCssRuleBlock(Parser& parser);
    void parseInlineProperty(Parser& parser);

    // --- Helper for template style resolution ---
    void applyStyleTemplateRecursive(
        Parser& parser,
        const std::string& ns,
        const std::string& templateName,
        std::map<std::string, std::unique_ptr<StyleValue>>& finalStyles,
        const std::vector<std::string>& deletedTemplates,
        std::vector<std::string>& visitedTemplates
    );
};