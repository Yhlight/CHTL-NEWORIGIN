#ifndef CHTL_SCRIPT_NODE_H
#define CHTL_SCRIPT_NODE_H

#include "BaseNode.h"
#include <string>

namespace CHTL {

class ScriptNode : public BaseNode {
public:
    explicit ScriptNode(const std::string& content) : BaseNode(NodeType::SCRIPT), content(content) {}

    const std::string& getContent() const { return content; }
    void print(int indent = 0) const override;
    std::unique_ptr<BaseNode> clone() const override {
        return std::make_unique<ScriptNode>(content);
    }

private:
    std::string content;
};

} // namespace CHTL

#endif // CHTL_SCRIPT_NODE_H