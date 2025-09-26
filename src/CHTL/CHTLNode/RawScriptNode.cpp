#include "RawScriptNode.h"

RawScriptNode::RawScriptNode(std::string content)
    : content(std::move(content)) {}