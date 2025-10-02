#include "ASTPrinter.h"
#include "../../CHTL/CHTLNode/ElementNode.h"
#include "../../CHTL/CHTLNode/TextNode.h"
#include "../../CHTL/CHTLNode/CommentNode.h"

void ASTPrinter::print(BaseNode* root) {
    if (!root) {
        std::cout << "AST is empty." << std::endl;
        return;
    }
    printNode(root, 0);
}

void ASTPrinter::printNode(BaseNode* node, int indent) {
    if (!node) return;

    std::string indentation(indent * 2, ' ');

    switch (node->getType()) {
        case NodeType::Element: {
            auto* elementNode = static_cast<ElementNode*>(node);
            std::cout << indentation << "Element: " << elementNode->getTagName() << std::endl;
            for (const auto& attr : elementNode->getAttributes()) {
                std::cout << indentation << "  Attribute: " << attr.first << " = \"" << attr.second << "\"" << std::endl;
            }
            break;
        }
        case NodeType::Text: {
            auto* textNode = static_cast<TextNode*>(node);
            std::cout << indentation << "Text: \"" << textNode->getText() << "\"" << std::endl;
            break;
        }
        case NodeType::Comment: {
            auto* commentNode = static_cast<CommentNode*>(node);
            std::cout << indentation << "Comment: \"" << commentNode->getComment() << "\"" << std::endl;
            break;
        }
        case NodeType::Base:
        default:
            std::cout << indentation << "RootNode" << std::endl;
            break;
    }

    for (const auto& child : node->getChildren()) {
        if (child) {
            printNode(child.get(), indent + 1);
        }
    }
}