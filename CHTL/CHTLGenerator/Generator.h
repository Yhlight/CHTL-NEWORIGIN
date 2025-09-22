#pragma once

#include "../CHTLNode/BaseNode.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
#include "../CHTLNode/AttributeNode.h"
#include "../CHTLNode/LiteralNode.h"
#include "../CHTLNode/StyleNode.h"
#include <string>
#include <sstream>

class Generator {
public:
    Generator();
    std::string Generate(Program* program);

private:
    void visit(Node* node);
    void visitElementNode(ElementNode* node);
    void visitTextNode(TextNode* node);
    void visitStyleNode(StyleNode* node);
    // Attributes are handled inside visitElementNode, so a separate visit might not be needed yet.
    // void visitAttributeNode(AttributeNode* node);
    std::string visitLiteralNode(LiteralNode* node);

    std::stringstream output;
};
