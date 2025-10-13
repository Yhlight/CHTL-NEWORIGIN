#pragma once
#include "chtl/Common.hpp"

namespace chtl {

struct Attribute {
    std::string name;
    std::string value; // allow raw or quoted processed value
    SourceRange range{};
};

struct Node {
    virtual ~Node() = default;
    SourceRange range{};
};

struct TextNode : Node {
    std::string text;
};

struct CommentNode : Node {
    std::string text; // content without //
};

struct ElementNode : Node {
    std::string tagName;            // e.g., div
    std::vector<Attribute> attrs;   // e.g., class, id, style...
    std::vector<std::unique_ptr<Node>> children;
};

struct StyleBlock : Node {
    std::string css;
};

struct Document {
    std::vector<std::unique_ptr<Node>> children;
};

} // namespace chtl
