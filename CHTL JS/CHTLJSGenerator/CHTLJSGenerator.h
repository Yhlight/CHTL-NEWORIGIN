#pragma once

#include "../CHTLJSNode/BaseNode.h"
#include "SaltBridge.h" // Include for AnimationData
#include <string>
#include <memory>
#include <sstream>

namespace CHTLJS {

// Forward declarations for node types
class SelectorNode;
class StringLiteralNode;

class CHTLJSGenerator {
public:
    std::string generate(const std::shared_ptr<BaseNode>& node);
    std::string generateAnimation(const CHTL::AnimationData& data);

private:
    void visit(const std::shared_ptr<BaseNode>& node);
    void visit(const std::shared_ptr<SelectorNode>& node);
    void visit(const std::shared_ptr<StringLiteralNode>& node);

    std::stringstream js_out;
};

}