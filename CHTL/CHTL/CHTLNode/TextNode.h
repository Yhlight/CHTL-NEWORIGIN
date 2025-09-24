#ifndef CHTL_TEXT_NODE_H
#define CHTL_TEXT_NODE_H

#include "CHTLNode.h"
#include <string>

// Forward declaration
class AstVisitor;

class TextNode : public CHTLNode {
public:
    explicit TextNode(const std::string& content);

    void accept(AstVisitor& visitor) override;

    const std::string& getContent() const;

private:
    std::string content;
};

#endif // CHTL_TEXT_NODE_H
