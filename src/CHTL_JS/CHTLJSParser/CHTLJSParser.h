#pragma once

#include "../CHTLJSNode/CHTLJSBaseNode.h"
#include <string>
#include <memory>

class CHTLJSParser {
public:
    std::unique_ptr<CHTLJSBaseNode> parse(const std::string& source);
};