#ifndef CHTL_HTML_GENERATOR_H
#define CHTL_HTML_GENERATOR_H

#include "../CHTLNode/NodeVisitor.h"
#include "../CHTLNode/DocumentNode.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
#include "../CHTLNode/CommentNode.h"
#include "../CHTLNode/StyleNode.h"
#include "../CHTLNode/CssRuleNode.h"
#include "../CHTLNode/ConditionNode.h"
#include <string>
#include <vector>
#include <sstream>
#include <memory>

class HtmlGenerator : public NodeVisitor {
public:
    std::string getResult();

    void visit(DocumentNode& node) override;
    void visit(ElementNode& node) override;
    void visit(TextNode& node) override;
    void visit(CommentNode& node) override;
    void visit(StyleNode& node) override;
    void visit(CssRuleNode& node) override;
    // Non-emitting nodes
    void visit(TemplateStyleNode& node) override {}
    void visit(TemplateElementNode& node) override {}
    void visit(TemplateVarNode& node) override {}
    void visit(CustomStyleNode& node) override {}
    void visit(CustomElementNode& node) override {}
    void visit(CustomVarNode& node) override {}
    void visit(ImportNode& node) override {}
    void visit(NamespaceNode& node) override {}
    void visit(UseNode& node) override {}
    void visit(DeclarationNode& node) override {}
    void visit(ConditionNode& node) override;

private:
    void findStyleNodes(BaseNode* node, std::vector<StyleNode*>& styleNodes);
    std::string generateHead(const std::vector<StyleNode*>& allStyleNodes);
    ElementNode* findElement(BaseNode* startNode, const std::string& selector);
    bool evaluateCondition(const std::string& condition, ElementNode& contextNode);
    double getNumericValue(const std::string& value);

    std::stringstream resultStream;
    std::stringstream hoistedCss;
    int depth = 0;
    DocumentNode* rootNode = nullptr;
};

#endif // CHTL_HTML_GENERATOR_H