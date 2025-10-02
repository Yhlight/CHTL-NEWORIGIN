/**
 * @file ScriptNode.h
 * @brief Script node for JavaScript
 */

#ifndef CHTL_SCRIPT_NODE_H
#define CHTL_SCRIPT_NODE_H

#include "BaseNode.h"

namespace CHTL {

class ScriptNode : public BaseNode {
public:
    ScriptNode();
    
    void setContent(const std::string& content) { content_ = content; }
    std::string getContent() const { return content_; }
    
    void setIsLocal(bool isLocal) { isLocal_ = isLocal; }
    bool isLocal() const { return isLocal_; }
    
    std::string toHTML() const override;
    std::string toJS() const override;
    void accept(NodeVisitor& visitor) override;
    std::string toString() const override;

private:
    std::string content_;
    bool isLocal_; // Local script (element-level) vs global
};

} // namespace CHTL

#endif // CHTL_SCRIPT_NODE_H
