#pragma once

#include <memory>
#include <string>
#include <vector>
#include <optional>

namespace chtl::ast {

enum class NodeKind {
	Document,
	Element,
	Attribute,
	Text,
	StyleBlock,
	ScriptBlock
};

struct Node {
	virtual ~Node() = default;
	virtual NodeKind kind() const = 0;
};

using NodePtr = std::unique_ptr<Node>;

struct Attribute : Node {
	std::string name;
	std::string value; // raw, may be unquoted literal or string; expression support later
	NodeKind kind() const override { return NodeKind::Attribute; }
};

struct Text : Node {
	std::string content; // raw text
	NodeKind kind() const override { return NodeKind::Text; }
};

struct StyleBlock : Node {
	std::vector<std::unique_ptr<Attribute>> declarations;
	NodeKind kind() const override { return NodeKind::StyleBlock; }
};

struct ScriptBlock : Node {
	std::string content; // raw JS for now
	NodeKind kind() const override { return NodeKind::ScriptBlock; }
};

struct Element : Node {
	std::string tag;
	std::vector<std::unique_ptr<Attribute>> attributes;
	std::unique_ptr<StyleBlock> style; // optional local style
	std::unique_ptr<ScriptBlock> script; // optional local script
	std::vector<NodePtr> children;
	NodeKind kind() const override { return NodeKind::Element; }
};

struct Document : Node {
	std::vector<NodePtr> nodes;
	NodeKind kind() const override { return NodeKind::Document; }
};

} // namespace chtl::ast

