#ifndef CHTL_ORIGINNODE_H
#define CHTL_ORIGINNODE_H

#include "BaseNode.h"
#include <string>

namespace CHTL {

class OriginNode : public BaseNode {
public:
    OriginNode(const std::string& type, const std::string& content);
    ~OriginNode() override = default;
    OriginNode(OriginNode&&) noexcept = default;
    OriginNode& operator=(OriginNode&&) noexcept = default;

    void print(int indent = 0) const override;
    std::string getType() const;
    std::string getContent() const;

private:
    std::string type;
    std::string content;
};

} // namespace CHTL

#endif // CHTL_ORIGINNODE_H