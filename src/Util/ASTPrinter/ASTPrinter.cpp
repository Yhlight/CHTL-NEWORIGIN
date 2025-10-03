#include "ASTPrinter.h"
#include "../../CHTL/CHTLNode/ElementNode.h"
#include "../../CHTL/CHTLNode/TextNode.h"
#include "../../CHTL/CHTLNode/CommentNode.h"
#include "../../CHTL/CHTLNode/StyleNode.h"
#include "../../CHTL/CHTLNode/ScriptNode.h"
#include "../../CHTL/CHTLNode/StylePropertyNode.h"
#include "../../CHTL/CHTLNode/ClassSelectorNode.h"
#include "../../CHTL/CHTLNode/IdSelectorNode.h"
#include "../../CHTL/CHTLNode/ContextSelectorNode.h"
#include "../../CHTL/CHTLNode/ValueNode.h"
#include "../../CHTL/CHTLNode/BinaryOpNode.h"

void ASTPrinter::print(BaseNode* root) {
    if (!root) {
        std::cout << "AST is empty." << std::endl;
        return;
    }
    printNode(root, 0);
}

void ASTPrinter::printExpression(const ExpressionNode* node, int indent) {
    if (!node) return;
    std::string indentation(indent * 2, ' ');

    switch (node->getType()) {
        case NodeType::Value: {
            auto* valueNode = static_cast<const ValueNode*>(node);
            std::cout << indentation << "Value: " << valueNode->getValue() << std::endl;
            break;
        }
        case NodeType::BinaryOp: {
            auto* opNode = static_cast<const BinaryOpNode*>(node);
            std::cout << indentation << "BinaryOp: " << static_cast<int>(opNode->getOp()) << std::endl;
            printExpression(opNode->getLeft(), indent + 1);
            printExpression(opNode->getRight(), indent + 1);
            break;
        }
        default:
            std::cout << indentation << "Unknown Expression Node" << std::endl;
            break;
    }
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
        case NodeType::Style: {
            std::cout << indentation << "Style" << std::endl;
            break;
        }
        case NodeType::Script: {
            auto* scriptNode = static_cast<ScriptNode*>(node);
            std::cout << indentation << "Script: \"" << scriptNode->getScript() << "\"" << std::endl;
            break;
        }
        case NodeType::StyleProperty: {
            auto* propNode = static_cast<StylePropertyNode*>(node);
            std::cout << indentation << "Style Property: " << propNode->getName() << std::endl;
            printExpression(propNode->getValue(), indent + 1);
            break;
        }
        case NodeType::ClassSelector: {
            auto* selectorNode = static_cast<ClassSelectorNode*>(node);
            std::cout << indentation << "Class Selector: ." << selectorNode->getName() << std::endl;
            break;
        }
        case NodeType::IdSelector: {
            auto* selectorNode = static_cast<IdSelectorNode*>(node);
            std::cout << indentation << "ID Selector: #" << selectorNode->getName() << std::endl;
            break;
        }
        case NodeType::ContextSelector: {
            auto* selectorNode = static_cast<ContextSelectorNode*>(node);
            std::cout << indentation << "Context Selector: " << selectorNode->getSelector() << std::endl;
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