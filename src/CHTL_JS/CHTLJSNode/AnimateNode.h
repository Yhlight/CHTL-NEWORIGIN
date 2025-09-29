#pragma once

#include "CHTLJSNode.h"
#include <string>
#include <vector>
#include <map>
#include <optional>

// Represents a single keyframe in an animation's 'when' block.
struct AnimationKeyframe {
    double at; // The point in time (0.0 to 1.0)
    std::map<std::string, std::string> styles;
};

// Represents an Animate block in the CHTL JS AST.
class AnimateNode : public CHTLJSNode {
public:
    AnimateNode() = default;
    CHTLJSNodeType getType() const override;

    // Animation properties
    std::optional<std::string> target;
    std::optional<int> duration;
    std::optional<std::string> easing;
    std::optional<int> loop;
    std::optional<std::string> direction;
    std::optional<int> delay;
    std::optional<std::string> callback;

    // Style blocks
    std::map<std::string, std::string> begin_styles;
    std::vector<AnimationKeyframe> when_keyframes;
    std::map<std::string, std::string> end_styles;
};