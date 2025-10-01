#ifndef CHTL_DELETENODE_H
#define CHTL_DELETENODE_H

#include "BaseNode.h"
#include <string>
#include <vector>

namespace CHTL {

class DeleteNode : public BaseNode {
public:
    explicit DeleteNode(const std::vector<std::string>& targets)
        : BaseNode(NodeType::DELETE_ACTION), targets(targets) {}

    const std::vector<std::string>& getTargets() const { return targets; }

    void print(int indent = 0) const override {
        for (int i = 0; i < indent; ++i) {
            std::cout << "  ";
        }
        std::cout << "Delete: ";
        for (size_t i = 0; i < targets.size(); ++i) {
            std::cout << targets[i] << (i < targets.size() - 1 ? ", " : "");
        }
        std::cout << std::endl;
    }

    std::unique_ptr<BaseNode> clone() const override {
        return std::make_unique<DeleteNode>(targets);
    }

private:
    std::vector<std::string> targets;
};

} // namespace CHTL

#endif // CHTL_DELETENODE_H