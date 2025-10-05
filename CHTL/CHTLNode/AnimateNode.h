#pragma once

#include "BaseNode.h"
#include "PropertyNode.h"
#include <vector>
#include <string>
#include <memory>

namespace CHTL {

    class AnimateNode : public BaseNode {
    public:
        AnimateNode() : BaseNode(NodeType::NODE_ANIMATE) {}

        std::string target;
        std::string duration;
        std::string easing;
        std::vector<std::shared_ptr<PropertyNode>> begin_properties;
        std::vector<std::pair<std::string, std::vector<std::shared_ptr<PropertyNode>>>> when_keyframes;
        std::vector<std::shared_ptr<PropertyNode>> end_properties;
        std::string loop;
        std::string direction;
        std::string delay;
        std::string callback;

        std::string
        ToString(const std::string &indent = "", bool isLast = true) const override {
            std::string str = indent;
            if (isLast) {
                str += "└─";
            } else {
                str += "├─";
            }
            str += "AnimateNode\n";

            // You can add more details about the node's properties here for better debugging
            // For example, printing the target, duration, etc.

            return str;
        }
    };

}