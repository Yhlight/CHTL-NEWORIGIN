#ifndef CHTL_HTML_GENERATOR_H
#define CHTL_HTML_GENERATOR_H

#include "../CHTLNode/DocumentNode.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
#include "../CHTLNode/CommentNode.h"
#include "../CHTLNode/StyleNode.h"
#include <string>
#include <vector>
#include <sstream>

class HtmlGenerator {
public:
    explicit HtmlGenerator(std::unique_ptr<DocumentNode> doc);
    std::string generate();

private:
    void findStyleNodes(BaseNode* node, std::vector<const StyleNode*>& styleNodes);
    std::string generateHead(const std::vector<const StyleNode*>& allStyleNodes);
    std::string generateBody(const std::vector<std::unique_ptr<BaseNode>>& nodes, int depth);
    std::string generateNode(const BaseNode* node, int depth);
    std::string generateElement(const ElementNode* node, int depth);
    std::string generateInlineStyle(const StyleNode* styleNode) const;

    std::unique_ptr<DocumentNode> document;
};

#endif // CHTL_HTML_GENERATOR_H