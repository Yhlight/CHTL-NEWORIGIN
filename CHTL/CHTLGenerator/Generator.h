#pragma once

#include "Visitor.h"
#include "../CHTLNode/ProgramNode.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
#include <string>
#include <sstream>
#include <set>

namespace CHTL {

// The Generator class traverses the AST using the Visitor pattern
// and builds an HTML string from it.
class Generator : public Visitor {
public:
    // The main entry point. It takes the root of the AST, generates the HTML,
    // and returns it as a string.
    std::string generate(ProgramNode& root);

    // Overridden visit methods for each concrete node type.
    void visit(ProgramNode& node) override;
    void visit(ElementNode& node) override;
    void visit(TextNode& node) override;

private:
    std::stringstream output;

    // A set of HTML tags that are self-closing (e.g., <img>, <br>).
    // This is needed to generate correct HTML.
    static const std::set<std::string> selfClosingTags;
};

} // namespace CHTL
