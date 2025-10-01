#ifndef CHTL_COMMENT_NODE_H
#define CHTL_COMMENT_NODE_H

#include "BaseNode.h"
#include <string>

namespace CHTL {

class CommentNode : public BaseNode {
public:
    explicit CommentNode(const std::string& value);
    void print(int indent = 0) const override;
    const std::string& getValue() const;

private:
    std::string value;
};

} // namespace CHTL

#endif // CHTL_COMMENT_NODE_H