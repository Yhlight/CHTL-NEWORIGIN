#ifndef CHTL_DELETE_NODE_H
#define CHTL_DELETE_NODE_H

#include "BaseNode.h"
#include <string>
#include <vector>

namespace CHTL {

class DeleteNode : public BaseNode {
public:
    explicit DeleteNode(const std::vector<std::string>& targets);

    void print(int indent = 0) const override;
    const std::vector<std::string>& getTargets() const;

private:
    std::vector<std::string> m_targets;
};

} // namespace CHTL

#endif // CHTL_DELETE_NODE_H