#ifndef CHTL_SCRIPT_NODE_H
#define CHTL_SCRIPT_NODE_H

#include "BaseNode.h"
#include <string>

namespace CHTL {

class ScriptNode : public BaseNode {
public:
    explicit ScriptNode(const std::string& content) : content(content) {}

    const std::string& getContent() const { return content; }
    void print(int indent = 0) const override;

private:
    std::string content;
};

} // namespace CHTL

#endif // CHTL_SCRIPT_NODE_H