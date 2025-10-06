#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>

namespace CHTL {
    class IfNode;

    // Data structures for passing animation data across the bridge
    struct AnimationProperty {
        std::string name;
        std::string value;
    };

    struct AnimationKeyframe {
        std::string at;
        std::vector<AnimationProperty> properties;
    };

    struct AnimationData {
        std::string target;
        std::string duration;
        std::string easing;
        std::vector<AnimationProperty> begin_properties;
        std::vector<AnimationKeyframe> when_keyframes;
        std::vector<AnimationProperty> end_properties;
        std::string loop;
        std::string direction;
        std::string delay;
        std::string callback;
    };


    class SaltBridge {
    public:
        virtual ~SaltBridge() = default;

        // This method will be called by the CHTLGenerator to process a script block.
        // The CHTL JS compiler will provide the implementation.
        virtual std::string processScript(const std::string& raw_script) = 0;

        // This method will be called by the CHTLGenerator to process an AnimateNode.
        virtual std::string processAnimation(const AnimationData& data) = 0;

        // This method will be called by the CHTLGenerator to process a dynamic IfNode.
        virtual std::string processDynamicIf(const std::vector<std::shared_ptr<IfNode>>& if_nodes, const std::string& parent_selector) = 0;
    };

}