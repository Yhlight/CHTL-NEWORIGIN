#pragma once

#include "../CHTLNode/BaseNode.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
#include "../CHTLNode/CommentNode.h"
#include "../CHTLNode/ConditionalNode.h"
#include "../Bridge/SharedContext.h"
#include <string>
#include <vector>
#include <memory>
#include <sstream>

// The Generator class traverses an Abstract Syntax Tree (AST) and
// produces the final output string (e.g., HTML).
class Generator {
public:
    Generator();

    // The main entry point for the generation process.
    std::string generate(
        std::vector<std::unique_ptr<BaseNode>>& roots,
        const std::string& globalCss,
        const std::string& globalJs,
        SharedContext& context,
        bool outputDoctype,
        bool inline_mode,
        const std::string& css_output_filename,
        const std::string& js_output_filename
    );

private:
    std::string result;
    int indentLevel;
    std::string globalCssToInject;
    std::string globalJsToInject;
    bool inlineMode = false;
    std::string cssOutputFilename;
    std::string jsOutputFilename;
    SharedContext* mutableContext = nullptr;

#include "../CHTLNode/ScriptNode.h"
    // Visitor-style methods to generate output for each node type.
    void generateNode(BaseNode* node);
    void generateElement(ElementNode* node);
    void generateText(const TextNode* node);
    void generateComment(const CommentNode* node);
    void generateScript(const ScriptNode* node);
    void generateConditional(ConditionalNode* node);
    void generateRuntimeScript(const SharedContext& context);

    // Helper methods for managing indentation and building the result string.
    void append(const std::string& str);
    void appendLine(const std::string& str);
    void indent();
    void outdent();
    std::string getIndent();
};