#pragma once

#include "../CHTLNode/BaseNode.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
#include "../CHTLNode/CommentNode.h" // Added this line
#include "../CHTLNode/TemplateNode.h"
#include "../CHTLNode/OriginNode.h"
#include "../CHTLContext/CHTLContext.h"
#include <vector>
#include <memory>
#include <string>
#include <sstream>

namespace CHTL {

class Generator {
public:
    Generator(const std::vector<std::shared_ptr<BaseNode>>& roots, CHTLContext& context);

    std::string generate();

private:
    const std::vector<std::shared_ptr<BaseNode>>& rootNodes;
    CHTLContext& context;
    std::stringstream output;
    std::map<std::string, std::string> globalCssRules;

    void generateNode(const std::shared_ptr<BaseNode>& node, int indentation = 0);
    void generateElementNode(const std::shared_ptr<ElementNode>& node, int indentation);
    void generateTextNode(const std::shared_ptr<TextNode>& node, int indentation);
    void generateCommentNode(const std::shared_ptr<CommentNode>& node, int indentation);
    void generateTemplateUsageNode(const std::shared_ptr<TemplateUsageNode>& node, int indentation);
    void generateOriginNode(const std::shared_ptr<OriginNode>& node, int indentation);

    std::string getIndent(int level);
};

} // namespace CHTL
