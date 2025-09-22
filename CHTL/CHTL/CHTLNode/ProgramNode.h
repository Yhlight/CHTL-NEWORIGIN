#ifndef CHTL_PROGRAM_NODE_H
#define CHTL_PROGRAM_NODE_H

#include "BaseNode.h"
#include "../CHTLGenerator/Generator.h"
#include <vector>
#include <memory>
#include <sstream>

namespace CHTL {

class ProgramNode : public BaseNode {
public:
    std::vector<std::unique_ptr<BaseNode>> children;

    std::string toString() const override {
        std::stringstream ss;
        ss << "ProgramNode {\n";
        for (const auto& child : children) {
            ss << child->toString() << "\n";
        }
        ss << "}";
        return ss.str();
    }

    void accept(Generator& visitor) const override {
        visitor.visit(*this);
    }
};

} // namespace CHTL

#endif // CHTL_PROGRAM_NODE_H
