#ifndef ELEMENT_NODE_H
#define ELEMENT_NODE_H

#include "BaseNode.h"
#include "StyleNode.h"
#include "ScriptNode.h"
#include "ConditionalNode.h"
#include <string>
#include <vector>
#include <memory>
#include <map>

namespace CHTL {

class StyleNode;
class ScriptNode;

class ElementNode : public BaseNode {
public:
    ElementNode(const std::string& tagName);
    void addChild(std::unique_ptr<BaseNode> child);
    void addAttribute(const std::string& key, const std::string& value);
    void setStyle(std::unique_ptr<StyleNode> styleNode);
    void setScript(std::unique_ptr<ScriptNode> scriptNode);
    void addIfBlock(std::unique_ptr<IfNode> ifNode);
    void print(int indent = 0) const override;

    const std::string& getTagName() const;
    const std::vector<std::unique_ptr<BaseNode>>& getChildren() const;
    const std::map<std::string, std::string>& getAttributes() const;
    std::string getAttribute(const std::string& key) const;
    const StyleNode* getStyle() const;
    const ScriptNode* getScript() const;
    const std::vector<std::unique_ptr<IfNode>>& getIfBlocks() const;


private:
    std::string tagName;
    std::vector<std::unique_ptr<BaseNode>> children;
    std::map<std::string, std::string> attributes;
    std::unique_ptr<StyleNode> style;
    std::unique_ptr<ScriptNode> script;
    std::vector<std::unique_ptr<IfNode>> ifBlocks;
};

} // namespace CHTL

#endif // ELEMENT_NODE_H