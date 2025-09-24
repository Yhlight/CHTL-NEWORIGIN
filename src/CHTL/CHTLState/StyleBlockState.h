#pragma once

#include "ParserState.h"
#include "../CHTLNode/StyleValue.h"
#include "../Util/Selector.h" // Include the new selector struct
#include <string>
#include <map> // Add map include
#include <utility> // For std::pair

// Forward declare to avoid circular dependencies
class ElementNode;

// State for parsing the contents of a 'style { ... }' block.
// This state handles parsing both inline styles and selector-based rules.
class StyleBlockState : public ParserState {
public:
    // The handle method will parse the entire style block.
    // It doesn't return a node, but modifies the parser's contextNode
    // (the element containing the style block) and potentially the parser's
    // collection of global styles.
    std::unique_ptr<BaseNode> handle(Parser& parser) override;

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
        std::map<std::string, std::string>& finalStyles,
        const std::vector<std::string>& deletedTemplates,
        std::vector<std::string>& visitedTemplates
    );
};
