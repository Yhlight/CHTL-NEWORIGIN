#pragma once

#include "../CHTLNode/BaseNode.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
#include "../CHTLNode/CommentNode.h"
#include "../Bridge/SharedContext.h"
#include <string>
#include <vector>
#include <memory>
#include <sstream>

// The Generator class traverses an Abstract Syntax Tree (AST) and
// produces the final output string (e.g., HTML).

enum class GenerationContext {
    HTML,
    CSS,
    JS
};

class Generator {
public:
    Generator();

    // The main entry point for the generation process.
    std::string generate(const std::vector<std::unique_ptr<BaseNode>>& roots, const std::string& globalCss, const SharedContext& context, bool outputDoctype);

private:
    std::string result;
    int indentLevel;
    std::string globalCssToInject;

#include "../CHTLNode/ScriptNode.h"
    // Visitor-style methods to generate output for each node type.
    void generateNode(const BaseNode* node, GenerationContext context = GenerationContext::HTML);
    void generateElement(ElementNode* node); // Needs to be non-const to add id
    void generateText(const TextNode* node);
    void generateComment(const CommentNode* node, GenerationContext context);
    void generateScript(const ScriptNode* node);
    void generateRuntimeScript(const SharedContext& context);

    // Helper methods for managing indentation and building the result string.
    void append(const std::string& str);
    void appendLine(const std::string& str);
    void indent();
    void outdent();
    std::string getIndent();
};