/**
 * @file CommentNode.h
 * @brief Comment node
 */

#ifndef CHTL_COMMENT_NODE_H
#define CHTL_COMMENT_NODE_H

#include "BaseNode.h"

namespace CHTL {

class CommentNode : public BaseNode {
public:
    CommentNode(const std::string& content = "", bool isGenerator = false);
    
    void setContent(const std::string& content) { content_ = content; }
    std::string getContent() const { return content_; }
    
    void setIsGenerator(bool isGenerator) { isGenerator_ = isGenerator; }
    bool isGenerator() const { return isGenerator_; }
    
    std::string toHTML() const override;
    void accept(NodeVisitor& visitor) override;
    std::string toString() const override;

private:
    std::string content_;
    bool isGenerator_; // Generator comment (#) vs regular comment (//, /**/)
};

} // namespace CHTL

#endif // CHTL_COMMENT_NODE_H
