#pragma once

#include "BaseNode.h"
#include <string>
#include <vector>
#include <memory>

namespace CHTL {

    class ScriptNode : public BaseNode {
    public:
        std::string content;

        ScriptNode() : BaseNode(NodeType::NODE_SCRIPT) {}
        explicit ScriptNode(const std::string& content) : BaseNode(NodeType::NODE_SCRIPT), content(content) {}

        std::string ToString(const std::string& indent = "", bool isLast = true) const override {
            std::string str = indent;
            if (isLast) {
                str += "└─";
            } else {
                str += "├─";
            }
            str += "ScriptNode\n";
            std::string newIndent = indent + (isLast ? "    " : "│   ");

            // Print content if it exists
            if (!content.empty()) {
                str += newIndent + "Content: \"" + content + "\"\n";
            }

            // Print children
            for (size_t i = 0; i < children.size(); ++i) {
                str += children[i]->ToString(newIndent, i == children.size() - 1);
            }

            return str;
        }
    };

}