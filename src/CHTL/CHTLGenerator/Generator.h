#pragma once

#include "../CHTLNode/BaseNode.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
#include "../CHTLNode/CommentNode.h"
#include "../CHTLNode/FragmentNode.h"
#include "../CHTLNode/OriginNode.h"
#include <string>
#include <vector>
#include <memory>

// The Generator class traverses an Abstract Syntax Tree (AST) and
// produces the final output string (e.g., HTML).
class Generator {
public:
    Generator();

    // The main entry point for the generation process.
    // Takes the AST, global CSS, and a flag for the doctype.
    std::string generate(const std::vector<std::unique_ptr<BaseNode>>& roots, const std::string& globalCss, bool outputDoctype);

private:
    std::string result;
    int indentLevel;
    std::string globalCssToInject;

    // Visitor-style methods to generate output for each node type.
    void generateNode(const BaseNode* node);
    void generateElement(const ElementNode* node);
    void generateText(const TextNode* node);
    void generateComment(const CommentNode* node);

    // Helper methods for managing indentation and building the result string.
    void append(const std::string& str);
    void appendLine(const std::string& str);
    void indent();
    void outdent();
    std::string getIndent();
};
