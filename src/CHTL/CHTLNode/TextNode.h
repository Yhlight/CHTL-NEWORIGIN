/**
 * @file TextNode.h
 * @brief Text node for text content
 */

#ifndef CHTL_TEXT_NODE_H
#define CHTL_TEXT_NODE_H

#include "BaseNode.h"

namespace CHTL {

class TextNode : public BaseNode {
public:
    explicit TextNode(const std::string& content = "");
    
    void setContent(const std::string& content) { content_ = content; }
    std::string getContent() const { return content_; }
    
    std::string toHTML() const override;
    void accept(NodeVisitor& visitor) override;
    std::string toString() const override;

private:
    std::string content_;
};

} // namespace CHTL

#endif // CHTL_TEXT_NODE_H
