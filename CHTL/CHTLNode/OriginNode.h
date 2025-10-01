#ifndef CHTL_ORIGIN_NODE_H
#define CHTL_ORIGIN_NODE_H

#include "BaseNode.h"
#include <string>

namespace CHTL {

enum class OriginType {
    HTML,
    STYLE,
    JAVASCRIPT
};

class OriginNode : public BaseNode {
public:
    OriginNode(OriginType type, const std::string& content);

    void print(int indent = 0) const override;

    OriginType getOriginType() const;
    const std::string& getContent() const;

private:
    OriginType m_type;
    std::string m_content;
};

} // namespace CHTL

#endif // CHTL_ORIGIN_NODE_H