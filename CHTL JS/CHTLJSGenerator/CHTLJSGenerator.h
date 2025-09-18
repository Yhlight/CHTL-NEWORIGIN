#pragma once

#include "../CHTLJSNode/CHTLJS_BaseNode.h"
#include <string>
#include <vector>
#include <memory>

namespace CHTL_JS {

class CHTLJSGenerator : public CHTLJS_AstVisitor {
public:
    std::string generate(std::vector<std::unique_ptr<CHTLJS_BaseNode>>& nodes);

    void visit(EnhancedSelectorNode& node) override;
    void visit(RawJSNode& node) override;

private:
    std::string m_output;
};

} // namespace CHTL_JS
